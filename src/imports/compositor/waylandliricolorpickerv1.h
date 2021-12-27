// SPDX-FileCopyrightText: 2020-2021 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef LIRI_SHELL_COMPOSITOR_WAYLANDLIRICOLORPICKERV1_H
#define LIRI_SHELL_COMPOSITOR_WAYLANDLIRICOLORPICKERV1_H

#include <LiriAuroraCompositor/WaylandCompositorExtension>

class WaylandLiriColorPickerManagerV1Private;

using namespace Aurora::Compositor;

class WaylandLiriColorPickerManagerV1
        : public WaylandCompositorExtensionTemplate<WaylandLiriColorPickerManagerV1>
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(WaylandLiriColorPickerManagerV1)
    Q_PROPERTY(QString layerName READ layerName WRITE setLayerName NOTIFY layerNameChanged)
public:
    WaylandLiriColorPickerManagerV1();
    WaylandLiriColorPickerManagerV1(WaylandCompositor *compositor);
    ~WaylandLiriColorPickerManagerV1();

    QString layerName() const;
    void setLayerName(const QString &layer);

    void initialize() override;

    static const wl_interface *interface();
    static QByteArray interfaceName();

Q_SIGNALS:
    void layerNameChanged();

private:
    QScopedPointer<WaylandLiriColorPickerManagerV1Private> const d_ptr;
};

#endif // LIRI_SHELL_COMPOSITOR_WAYLANDLIRICOLORPICKERV1_H
