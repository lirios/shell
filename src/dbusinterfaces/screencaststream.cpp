/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 * Copyright (C) 2018 Red Hat, Inc
 *
 * $BEGIN_LICENSE:LGPLv3+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

#include <QSocketNotifier>
#include <QTimer>

#include "screencast.h"
#include "screencaststream.h"

#include <glib-object.h>

#include <math.h>
#include <sys/mman.h>
#include <stdio.h>

class PwFraction {
public:
    int num;
    int denom;
};

// Stolen from mutter

#define MAX_TERMS       30
#define MIN_DIVISOR     1.0e-10
#define MAX_ERROR       1.0e-20

#define PROP_RANGE(min, max) 2, (min), (max)

#define BITS_PER_PIXEL  4

static int greatestCommonDivisor(int a, int b)
{
    while (b != 0) {
        int temp = a;

        a = b;
        b = temp % b;
    }

    return ABS(a);
}

static PwFraction pipewireFractionFromDouble(double src)
{
    double V, F;                 /* double being converted */
    int N, D;                    /* will contain the result */
    int A;                       /* current term in continued fraction */
    qint64 N1, D1;              /* numerator, denominator of last approx */
    qint64 N2, D2;              /* numerator, denominator of previous approx */
    int i;
    int gcd;
    gboolean negative = FALSE;

    /* initialize fraction being converted */
    F = src;
    if (F < 0.0) {
        F = -F;
        negative = TRUE;
    }

    V = F;
    /* initialize fractions with 1/0, 0/1 */
    N1 = 1;
    D1 = 0;
    N2 = 0;
    D2 = 1;
    N = 1;
    D = 1;

    for (i = 0; i < MAX_TERMS; i++) {
        /* get next term */
        A = (gint) F;               /* no floor() needed, F is always >= 0 */
        /* get new divisor */
        F = F - A;

        /* calculate new fraction in temp */
        N2 = N1 * A + N2;
        D2 = D1 * A + D2;

        /* guard against overflow */
        if (N2 > G_MAXINT || D2 > G_MAXINT)
            break;

        N = N2;
        D = D2;

        /* save last two fractions */
        N2 = N1;
        D2 = D1;
        N1 = N;
        D1 = D;

        /* quit if dividing by zero or close enough to target */
        if (F < MIN_DIVISOR || fabs (V - ((gdouble) N) / D) < MAX_ERROR)
            break;

        /* Take reciprocal */
        F = 1 / F;
    }

    /* fix for overflow */
    if (D == 0) {
        N = G_MAXINT;
        D = 1;
    }

    /* fix for negative */
    if (negative)
        N = -N;

    /* simplify */
    gcd = greatestCommonDivisor(N, D);
    if (gcd) {
        N /= gcd;
        D /= gcd;
    }

    PwFraction fraction;
    fraction.num = N;
    fraction.denom = D;

    return fraction;
}

static void onStateChanged(void *_data, pw_remote_state old, pw_remote_state state, const char *error)
{
    Q_UNUSED(old);
    Q_UNUSED(_data);

    switch (state) {
    case PW_REMOTE_STATE_ERROR:
        // TODO notify error
        qCWarning(lcScreenCast) << "Remote error:" << error;
        break;
    case PW_REMOTE_STATE_CONNECTED:
        // TODO notify error
        qCDebug(lcScreenCast) << "Remote state:" << pw_remote_state_as_string(state);
        break;
    default:
        qCDebug(lcScreenCast) << "Remote state:" << pw_remote_state_as_string(state);
        break;
    }
}
static void onStreamStateChanged(void *data, pw_stream_state old, pw_stream_state state, const char *error_message)
{
    Q_UNUSED(old);

    ScreenCastStream *pw = static_cast<ScreenCastStream *>(data);

    switch (state) {
    case PW_STREAM_STATE_ERROR:
        qCWarning(lcScreenCast) << "Stream error:" << error_message;
        break;
    case PW_STREAM_STATE_CONFIGURE:
        qCDebug(lcScreenCast) << "Stream state:" << pw_stream_state_as_string(state);
        Q_EMIT pw->streamReady(pw_stream_get_node_id(pw->pwStream));
        break;
    case PW_STREAM_STATE_UNCONNECTED:
    case PW_STREAM_STATE_CONNECTING:
    case PW_STREAM_STATE_READY:
    case PW_STREAM_STATE_PAUSED:
        qCDebug(lcScreenCast) << "Stream state:" << pw_stream_state_as_string(state);
        Q_EMIT pw->stopStreaming();
        break;
    case PW_STREAM_STATE_STREAMING:
        qCDebug(lcScreenCast) << "Stream state:" << pw_stream_state_as_string(state);
        Q_EMIT pw->startStreaming();
        break;
    }
}

#if PW_API_PRE_0_2_0
static void onStreamFormatChanged(void *data, struct spa_pod *format)
#else
static void onStreamFormatChanged(void *data, const struct spa_pod *format)
#endif
{
    qCDebug(lcScreenCast) << "Stream format changed";

    ScreenCastStream *pw = static_cast<ScreenCastStream *>(data);

    uint8_t paramsBuffer[1024];
    int32_t width, height, stride, size;
    struct spa_pod_builder pod_builder;
#if PW_API_PRE_0_2_0
    struct spa_pod *params[1];
#else
    const struct spa_pod *params[1];
#endif
    const int bpp = 4;

    if (!format) {
        pw_stream_finish_format(pw->pwStream, 0, nullptr, 0);
        return;
    }

    spa_format_video_raw_parse(format, &pw->videoFormat, &pw->pwType->formatVideo);

    width = pw->videoFormat.size.width;
    height =pw->videoFormat.size.height;
    stride = SPA_ROUND_UP_N(width * bpp, 4);
    size = height * stride;

    pod_builder = SPA_POD_BUILDER_INIT(paramsBuffer, sizeof(paramsBuffer));

    params[0] = static_cast<spa_pod *>(
                spa_pod_builder_object(&pod_builder,
                                       pw->pwCoreType->param.idBuffers, pw->pwCoreType->param_buffers.Buffers,
                                       ":", pw->pwCoreType->param_buffers.size, "i", size,
                                       ":", pw->pwCoreType->param_buffers.stride, "i", stride,
                                       ":", pw->pwCoreType->param_buffers.buffers, "iru", 16, PROP_RANGE (2, 16),
                                       ":", pw->pwCoreType->param_buffers.align, "i", 16));

    pw_stream_finish_format(pw->pwStream, 0,
                            params, G_N_ELEMENTS(params));
}

static const struct pw_remote_events pwRemoteEvents = {
    .version = PW_VERSION_REMOTE_EVENTS,
    .destroy = nullptr,
    .info_changed = nullptr,
    .sync_reply = nullptr,
    .state_changed = onStateChanged,
};

static const struct pw_stream_events pwStreamEvents = {
    .version = PW_VERSION_STREAM_EVENTS,
    .destroy = nullptr,
    .state_changed = onStreamStateChanged,
    .format_changed = onStreamFormatChanged,
    .add_buffer = nullptr,
    .remove_buffer = nullptr,
#if PW_API_PRE_0_2_0
    .new_buffer = nullptr,
    .need_buffer = nullptr,
#else
    .process = nullptr,
#endif
};

ScreenCastStream::ScreenCastStream(QObject *parent)
    : QObject(parent)
{
    initialize();
}

ScreenCastStream::~ScreenCastStream()
{
    removeStream();

    if (pwType)
        delete pwType;

    if (pwCore)
        pw_core_destroy(pwCore);

    if (pwLoop) {
        pw_loop_leave(pwLoop);
        pw_loop_destroy(pwLoop);
    }
}

quint32 ScreenCastStream::nodeId() const
{
    if (pwStream)
        return pw_stream_get_node_id(pwStream);
    return 0;
}

bool ScreenCastStream::createStream(const QSize &resolution)
{
    if (pw_remote_get_state(pwRemote, nullptr) != PW_REMOTE_STATE_CONNECTED) {
        qCWarning(lcScreenCast) << "Cannot create PipeWire stream";
        return false;
    }

    quint8 buffer[1024];
    spa_pod_builder podBuilder = SPA_POD_BUILDER_INIT(buffer, sizeof(buffer));

    const float frameRate = 25;

    spa_fraction maxFramerate;
    spa_fraction minFramerate;
    const spa_pod *params[1];

    pwStream = pw_stream_new(pwRemote, "liri-screen-cast", nullptr);

    PwFraction fraction = pipewireFractionFromDouble(frameRate);

    minFramerate = SPA_FRACTION(1, 1);
    maxFramerate = SPA_FRACTION((quint32)fraction.num, (quint32)fraction.denom);

    spa_fraction paramFraction = SPA_FRACTION(0, 1);
    spa_rectangle paramRectangle = SPA_RECTANGLE((quint32)resolution.width(), (quint32)resolution.height());

    params[0] = static_cast<spa_pod *>(
                spa_pod_builder_object(&podBuilder,
                                       pwCoreType->param.idEnumFormat,
                                       pwCoreType->spa_format,
                                       "I", pwType->mediaType.video,
                                       "I", pwType->mediaSubType.raw,
                                       ":", pwType->formatVideo.format, "I", pwType->videoFormat.RGBx,
                                       ":", pwType->formatVideo.size, "R", &paramRectangle,
                                       ":", pwType->formatVideo.framerate, "F", &paramFraction,
                                       ":", pwType->formatVideo.max_framerate, "Fr", &maxFramerate,
                                       PROP_RANGE (&minFramerate, &maxFramerate)));

    pw_stream_add_listener(pwStream, &streamListener, &pwStreamEvents, this);

#if PW_API_PRE_0_2_0
    if (pw_stream_connect(pwStream, PW_DIRECTION_OUTPUT, nullptr, PW_STREAM_FLAG_NONE, params, G_N_ELEMENTS(&params)) != 0) {
#else
    if (pw_stream_connect(pwStream, PW_DIRECTION_OUTPUT, nullptr, static_cast<pw_stream_flags>(PW_STREAM_FLAG_DRIVER | PW_STREAM_FLAG_MAP_BUFFERS), params, G_N_ELEMENTS(&params)) != 0) {
#endif
        qCWarning(lcScreenCast) << "Could not connect to stream";
        return false;
    }

    return true;
}

bool ScreenCastStream::recordFrame(quint8 *screenData)
{
#if PW_API_PRE_0_2_0
    quint32 bufferId;
#else
    struct pw_buffer *buffer;
#endif
    struct spa_buffer *spaBuffer;
    quint8 *map = nullptr;
    quint8 *data = nullptr;

    // TODO check timestamp like mutter does?

    if (!pwStream)
        return false;

#if PW_API_PRE_0_2_0
    bufferId = pw_stream_get_empty_buffer(pwStream);

    if (bufferId == SPA_ID_INVALID) {
        qCWarning(lcScreenCast) << "Failed to get empty stream buffer: " << strerror(errno);
        return false;
    }

    spaBuffer = pw_stream_peek_buffer(pwStream, bufferId);
#else
    buffer = pw_stream_dequeue_buffer(pwStream);
#endif

#if PW_API_PRE_0_2_0
    if (spaBuffer->datas[0].type == pwCoreType->data.MemFd) {
#else
    spaBuffer = buffer->buffer;

    if (spaBuffer->datas[0].data) {
        data = static_cast<quint8 *>(spaBuffer->datas[0].data);
    } else if (spaBuffer->datas[0].type == pwCoreType->data.MemFd) {
#endif
        map = static_cast<quint8 *>(mmap(nullptr, spaBuffer->datas[0].maxsize +
                                    spaBuffer->datas[0].mapoffset,
                PROT_READ | PROT_WRITE, MAP_SHARED, spaBuffer->datas[0].fd, 0));

        if (map == MAP_FAILED) {
            qCWarning(lcScreenCast) << "Failed to mmap PipeWire stream buffer: " << strerror(errno);
            return false;
        }

        data = SPA_MEMBER(map, spaBuffer->datas[0].mapoffset, quint8);
#if PW_API_PRE_0_2_0
    } else if (spaBuffer->datas[0].type == pwCoreType->data.MemPtr) {
        data = static_cast<quint8 *>(spaBuffer->datas[0].data);
#endif
    } else {
        return false;
    }

    memcpy(data, screenData, BITS_PER_PIXEL * videoFormat.size.height * videoFormat.size.width * sizeof(quint8));

    if (map)
        munmap(map, spaBuffer->datas[0].maxsize + spaBuffer->datas[0].mapoffset);

    spaBuffer->datas[0].chunk->size = spaBuffer->datas[0].maxsize;

#if PW_API_PRE_0_2_0
    pw_stream_send_buffer(pwStream, bufferId);
#else
    pw_stream_queue_buffer(pwStream, buffer);
#endif

    return true;
}

void ScreenCastStream::initialize()
{
    pw_init(nullptr, nullptr);

    pwLoop = pw_loop_new(nullptr);
    m_socketNotifier.reset(new QSocketNotifier(pw_loop_get_fd(pwLoop), QSocketNotifier::Read));
    connect(m_socketNotifier.data(), &QSocketNotifier::activated,
            this, &ScreenCastStream::processPipeWireEvents);

    pwCore = pw_core_new(pwLoop, nullptr);
    pwCoreType = pw_core_get_type(pwCore);
    pwRemote = pw_remote_new(pwCore, nullptr, 0);

    auto map = pwCoreType->map;

#if !PW_API_PRE_0_2_0
#  if (PW_MAJOR==0) && (PW_MINOR==2) && (PW_MICRO<3)
    spa_debug_set_type_map(map);
#  endif
#endif

    pwType = new PwType();

    spa_type_media_type_map(map, &pwType->mediaType);
    spa_type_media_subtype_map(map, &pwType->mediaSubType);
    spa_type_format_video_map(map, &pwType->formatVideo);
    spa_type_video_format_map(map, &pwType->videoFormat);

    pw_remote_add_listener(pwRemote, &remoteListener, &pwRemoteEvents, this);

    pw_remote_connect(pwRemote);
}

void ScreenCastStream::removeStream()
{
    pw_stream_destroy(pwStream);
    pwStream = nullptr;
}

void ScreenCastStream::processPipeWireEvents()
{
    int result = pw_loop_iterate(pwLoop, 0);
    if (result < 0)
        qCWarning(lcScreenCast) << "Failed to iterate over PipeWire loop:" << spa_strerror(result);
}
