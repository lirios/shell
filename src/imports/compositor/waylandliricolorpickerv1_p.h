// SPDX-FileCopyrightText: 2020-2022 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef LIRI_SHELL_COMPOSITOR_WAYLANDLIRICOLORPICKERV1_P_H
#define LIRI_SHELL_COMPOSITOR_WAYLANDLIRICOLORPICKERV1_P_H

#include <QLoggingCategory>
#include <QPointer>

#include <LiriAuroraCompositor/WaylandOutput>
#include <LiriAuroraCompositor/WaylandSeat>

#include "waylandliricolorpickerv1.h"
#include "aurora-server-liri-color-picker-unstable-v1.h"

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Liri API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

Q_DECLARE_LOGGING_CATEGORY(gLcWaylandLiriColorPickerV1)

class WaylandLiriColorPickerV1EventFilter;

class WaylandLiriColorPickerV1
        : public QObject
        , public PrivateServer::zliri_color_picker_v1
{
    Q_OBJECT
public:
    explicit WaylandLiriColorPickerV1(QObject *parent = nullptr);

    WaylandLiriColorPickerManagerV1 *manager = nullptr;
    WaylandOutput *output = nullptr;
    QPointer<WaylandLiriColorPickerV1EventFilter> filter;

protected:
    void zliri_color_picker_v1_bind_resource(Resource *resource) override;
    void zliri_color_picker_v1_destroy_resource(Resource *resource) override;
    void zliri_color_picker_v1_destroy(Resource *resource) override;
};

class WaylandLiriColorPickerV1EventFilter : public QObject
{
    Q_OBJECT
public:
    explicit WaylandLiriColorPickerV1EventFilter(WaylandLiriColorPickerV1 *picker,
                                                 WaylandSeat *seat,
                                                 QObject *parent = nullptr);

private:
    WaylandLiriColorPickerV1 *m_picker = nullptr;
    WaylandSeat *m_seat = nullptr;
    QColor m_color;

protected:
    bool eventFilter(QObject *receiver, QEvent *event) override;
};

class WaylandLiriColorPickerManagerV1Private
        : public PrivateServer::zliri_color_picker_manager_v1
{
    Q_DECLARE_PUBLIC(WaylandLiriColorPickerManagerV1)
public:
    explicit WaylandLiriColorPickerManagerV1Private(WaylandLiriColorPickerManagerV1 *self);

    static WaylandLiriColorPickerManagerV1Private *get(WaylandLiriColorPickerManagerV1 *self) { return self ? self->d_func() : nullptr; }

    QString layerName;
    QVector<WaylandLiriColorPickerV1 *> pickers;

protected:
    WaylandLiriColorPickerManagerV1 *q_ptr = nullptr;

    void zliri_color_picker_manager_v1_pick_at_location(
            Resource *resource,
            uint32_t id,
            struct ::wl_resource *output_res,
            uint32_t x, uint32_t y) override;
    void zliri_color_picker_manager_v1_pick_interactively(
            Resource *resource,
            uint32_t id,
            struct ::wl_resource *seat_res) override;
};

#endif // LIRI_SHELL_COMPOSITOR_WAYLANDLIRICOLORPICKERV1_P_H
