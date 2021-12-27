// SPDX-FileCopyrightText: 2020-2021 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QGuiApplication>
#include <QQuickItem>
#include <QQuickItemGrabResult>
#include <QQuickWindow>
#include <QScreen>
#include <QTimer>

#include <LiriAuroraCompositor/WaylandCompositor>

#include "waylandliricolorpickerv1_p.h"

#include <wayland-server-protocol.h>

Q_LOGGING_CATEGORY(gLcWaylandLiriColorPickerV1, "liri.shell.liricolorpickerv1")

/*
 * WaylandLiriColorPickerV1
 */

WaylandLiriColorPickerV1::WaylandLiriColorPickerV1(QObject *parent)
    : QObject(parent)
    , PrivateServer::zliri_color_picker_v1()
{
}

void WaylandLiriColorPickerV1::zliri_color_picker_v1_bind_resource(Resource *r)
{
    // Make sure only an authorized client can bind
    pid_t pid;
    wl_client_get_credentials(r->client(), &pid, nullptr, nullptr);
    QFile file(QString::asprintf("/proc/%d/cmdline", pid));
    if (file.open(QFile::ReadOnly)) {
        QTextStream stream(&file);
        QString data = stream.readAll();
        QStringList args = data.split(QLatin1Char('\0'));
        file.close();

        // XXX: We gotta find a better way to do it
        bool allowed =
                args.first().endsWith(QStringLiteral("/xdg-desktop-portal-liri")) ||
                args.first().endsWith(QStringLiteral("/tst_waylandserver"));
        if (!allowed) {
            wl_resource_post_error(r->handle, WL_DISPLAY_ERROR_IMPLEMENTATION,
                                   "unauthorized client program");
            return;
        }
    } else {
        wl_resource_post_error(r->handle, WL_DISPLAY_ERROR_IMPLEMENTATION,
                               "unauthorized client program");
        return;
    }

    // Client can bind only once
    if (resource())
        wl_resource_post_error(r->handle,
                               WL_DISPLAY_ERROR_INVALID_OBJECT,
                               "client can bind only once");
}

void WaylandLiriColorPickerV1::zliri_color_picker_v1_destroy_resource(Resource *resource)
{
    Q_UNUSED(resource)

    WaylandLiriColorPickerManagerV1Private::get(manager)->pickers.removeOne(this);
    deleteLater();
}

void WaylandLiriColorPickerV1::zliri_color_picker_v1_destroy(Resource *resource)
{
    wl_resource_destroy(resource->handle);
}

/*
 * WaylandLiriColorPickerV1EventFilter
 */

WaylandLiriColorPickerV1EventFilter::WaylandLiriColorPickerV1EventFilter(WaylandLiriColorPickerV1 *picker,
                                                                         WaylandSeat *seat,
                                                                         QObject *parent)
    : QObject(parent)
    , m_picker(picker)
    , m_seat(seat)
{
}

bool WaylandLiriColorPickerV1EventFilter::eventFilter(QObject *receiver, QEvent *event)
{
    // When the mouse button is pressed, we select the pixel at the pointer
    // location and then send the result back to the client when the button
    // is released
    if (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseButtonRelease) {
        auto *mouseEvent = static_cast<QMouseEvent *>(event);

        // Find the target window
        QWindow *foundWindow = nullptr;
        auto windows = QGuiApplication::topLevelWindows();
        for (auto *window : qAsConst(windows)) {
            if (window->isVisible() && window->geometry().contains(mouseEvent->globalPos())) {
                foundWindow = window;
                break;
            }
        }
        if (!foundWindow) {
            qCWarning(gLcWaylandLiriColorPickerV1, "Unable to find window from mouse event");
            return false;
        }

        // Verify that this mouse event is for the target seat
        auto *detectedSeat = m_seat->compositor()->seatFor(mouseEvent);
        if (detectedSeat == m_seat) {
            if (event->type() == QEvent::MouseButtonPress) {
                auto pos = mouseEvent->localPos().toPoint();

                auto *quickWindow = qobject_cast<QQuickWindow *>(foundWindow);
                if (quickWindow) {
                    auto layerName = m_picker->manager->layerName();
                    auto *item = quickWindow->contentItem();
                    auto *uiItem = layerName.isEmpty() ? item : item->findChild<QQuickItem *>(layerName);

                    // Synchronously capture the layer contents within 5 seconds
                    QSharedPointer<QQuickItemGrabResult> result = uiItem->grabToImage();
                    if (result.isNull() || result->image().isNull()) {
                        QEventLoop loop;
                        connect(result.data(), &QQuickItemGrabResult::ready,
                                &loop, &QEventLoop::quit);
                        QTimer::singleShot(5000, &loop, &QEventLoop::quit);
                        loop.exec();
                    }

                    m_color = result->image().pixelColor(pos);
                } else {
                    auto winId = foundWindow->winId();
                    auto pixmap = foundWindow->screen()->grabWindow(winId);
                    m_color = pixmap.toImage().pixelColor(pos);
                }

                return true;
            } else if (event->type() == QEvent::MouseButtonRelease) {
                m_picker->send_picked(static_cast<uint32_t>(m_color.rgba()));

                QGuiApplication::instance()->removeEventFilter(this);
                deleteLater();

                return true;
            }
        }
    }

    return QObject::eventFilter(receiver, event);
}

/*
 * WaylandLiriColorPickerManagerV1Private
 */

WaylandLiriColorPickerManagerV1Private::WaylandLiriColorPickerManagerV1Private(WaylandLiriColorPickerManagerV1 *self)
    : PrivateServer::zliri_color_picker_manager_v1()
    , q_ptr(self)
{
}

void WaylandLiriColorPickerManagerV1Private::zliri_color_picker_manager_v1_pick_at_location(
        Resource *resource, uint32_t id, struct ::wl_resource *output_res,
        uint32_t x, uint32_t y)
{
    Q_Q(WaylandLiriColorPickerManagerV1);

    auto *output = WaylandOutput::fromResource(output_res);
    auto outputId = wl_resource_get_id(output_res);
    if (!output) {
        qCWarning(gLcWaylandLiriColorPickerV1, "Resource wl_output@%d doesn't exist", outputId);
        wl_resource_post_error(resource->handle, WL_DISPLAY_ERROR_INVALID_OBJECT,
                               "resource wl_output@%d doesn't exit", outputId);
        return;
    }

    bool found = false;
    for (auto *picker : qAsConst(pickers)) {
        if (picker->output == output) {
            found = true;
            break;
        }
    }
    if (found) {
        qCWarning(gLcWaylandLiriColorPickerV1, "A color picker for the same output already exist");
        wl_resource_post_error(resource->handle, error_already_constructed,
                               "a color picker already exist for wl_output@%d", outputId);
        return;
    }

    auto *picker = new WaylandLiriColorPickerV1(q);
    picker->manager = q;
    picker->output = output;
    picker->init(resource->client(), id, resource->version());
    pickers.append(picker);

    QColor color;

    if (output->window()) {
        auto *quickWindow = qobject_cast<QQuickWindow *>(output->window());
        if (quickWindow) {
            color = quickWindow->grabWindow().pixelColor(x, y);
        } else {
            auto winId = output->window()->winId();
            auto pixmap = output->window()->screen()->grabWindow(winId);
            color = pixmap.toImage().pixelColor(x, y);
        }
    }

    if (color.isValid())
        picker->send_picked(static_cast<uint32_t>(color.rgba()));
}

void WaylandLiriColorPickerManagerV1Private::zliri_color_picker_manager_v1_pick_interactively(Resource *resource, uint32_t id,
                                                                                              struct ::wl_resource *seat_res)
{
    Q_Q(WaylandLiriColorPickerManagerV1);

    auto *seat = WaylandSeat::fromSeatResource(seat_res);
    if (!seat) {
        auto seatId = wl_resource_get_id(seat_res);
        qCWarning(gLcWaylandLiriColorPickerV1, "Resource wl_seat@%d doesn't exist", seatId);
        wl_resource_post_error(resource->handle, WL_DISPLAY_ERROR_INVALID_OBJECT,
                               "resource wl_seat@%d doesn't exit", seatId);
        return;
    }

    auto *picker = new WaylandLiriColorPickerV1(q);
    picker->manager = q;
    picker->output = nullptr;
    picker->init(resource->client(), id, resource->version());
    pickers.append(picker);

    if (picker->filter) {
        qCWarning(gLcWaylandLiriColorPickerV1, "Previous interactive color picking is still in progress");
        wl_resource_post_error(resource->handle, error_still_in_progress,
                               "previous interactive color picking is still in progress");
        return;
    }

    picker->filter = new WaylandLiriColorPickerV1EventFilter(picker, seat, picker);
    QGuiApplication::instance()->installEventFilter(picker->filter.data());
}

/*
 * WaylandLiriColorPickerManagerV1
 */

WaylandLiriColorPickerManagerV1::WaylandLiriColorPickerManagerV1()
    : WaylandCompositorExtensionTemplate<WaylandLiriColorPickerManagerV1>()
    , d_ptr(new WaylandLiriColorPickerManagerV1Private(this))
{
}

WaylandLiriColorPickerManagerV1::WaylandLiriColorPickerManagerV1(WaylandCompositor *compositor)
    : WaylandCompositorExtensionTemplate<WaylandLiriColorPickerManagerV1>(compositor)
    , d_ptr(new WaylandLiriColorPickerManagerV1Private(this))
{
}

WaylandLiriColorPickerManagerV1::~WaylandLiriColorPickerManagerV1()
{
}

QString WaylandLiriColorPickerManagerV1::layerName() const
{
    Q_D(const WaylandLiriColorPickerManagerV1);
    return d->layerName;
}

void WaylandLiriColorPickerManagerV1::setLayerName(const QString &layer)
{
    Q_D(WaylandLiriColorPickerManagerV1);

    if (d->layerName == layer)
        return;

    d->layerName = layer;
    Q_EMIT layerNameChanged();
}

void WaylandLiriColorPickerManagerV1::initialize()
{
    Q_D(WaylandLiriColorPickerManagerV1);

    WaylandCompositorExtensionTemplate::initialize();
    WaylandCompositor *compositor = static_cast<WaylandCompositor *>(extensionContainer());
    if (!compositor) {
        qCWarning(gLcWaylandLiriColorPickerV1) << "Failed to find WaylandCompositor when initializing WaylandLiriColorPickerManager";
        return;
    }
    d->init(compositor->display(), WaylandLiriColorPickerManagerV1Private::interfaceVersion());
}

const wl_interface *WaylandLiriColorPickerManagerV1::interface()
{
    return WaylandLiriColorPickerManagerV1Private::interface();
}

QByteArray WaylandLiriColorPickerManagerV1::interfaceName()
{
    return WaylandLiriColorPickerManagerV1Private::interfaceName();
}
