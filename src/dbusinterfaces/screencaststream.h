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

#ifndef SCREENCASTSTREAM_H
#define SCREENCASTSTREAM_H

#include <QObject>
#include <QSize>

#include <spa/support/type-map.h>
#include <spa/param/format-utils.h>
#include <spa/param/video/format-utils.h>
#include <spa/param/video/raw-utils.h>
#include <spa/param/props.h>

#include <pipewire/factory.h>
#include <pipewire/pipewire.h>
#include <pipewire/remote.h>
#include <pipewire/stream.h>

#ifdef __has_include
#  if __has_include(<pipewire/version.h>)
#    include <pipewire/version.h>
#    define PW_API_PRE_0_2_0 false
#  else
#    define PW_API_PRE_0_2_0 true
#  endif
#else
#  define PW_API_PRE_0_2_0 true
#endif

#if !PW_API_PRE_0_2_0
#  if (PW_MAJOR==0) && (PW_MINOR==2) && (PW_MICRO<3)
#    include <spa/lib/debug.h>
#  endif
#endif

class QSocketNotifier;

class PwType {
public:
    spa_type_media_type mediaType;
    spa_type_media_subtype mediaSubType;
    spa_type_format_video formatVideo;
    spa_type_video_format videoFormat;
};

class ScreenCastStream : public QObject
{
    Q_OBJECT
public:
    explicit ScreenCastStream(QObject *parent = nullptr);
    ~ScreenCastStream();

    quint32 nodeId() const;

    bool createStream(const QSize &resolution);
    bool recordFrame(quint8 *screenData);
    void removeStream();

Q_SIGNALS:
    void streamReady(quint32 nodeId);
    void startStreaming();
    void stopStreaming();

public:
    pw_core *pwCore = nullptr;
    pw_loop *pwLoop = nullptr;
    pw_node *pwNode = nullptr;
    pw_stream *pwStream = nullptr;
    pw_type *pwCoreType = nullptr;
    pw_remote *pwRemote = nullptr;
    PwType *pwType = nullptr;

    spa_hook remoteListener;
    spa_hook streamListener;

    spa_video_info_raw videoFormat;

private:
    QScopedPointer<QSocketNotifier> m_socketNotifier;

    void initialize();

private Q_SLOTS:
    void processPipeWireEvents();
};

#endif // SCREENCASTSTREAM_H
