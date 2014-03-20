/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2013-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 * Copyright (C) 2013 Marco Martin <mart@kde.org>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:GPL2+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>
#include <QtGui/qpa/qplatformnativeinterface.h>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlEngine>
#include <QtQuick/QQuickItem>

#include <HawaiiShell/Containment>

#include "panelconfigview.h"
#include "panelview.h"
#include "shellmanager.h"

PanelConfigView::PanelConfigView(PanelView *parent)
    : ConfigView(parent)
    , m_panelView(parent)
{
    // Configuration view must be on the same screen as the panel view
    connect(m_panelView, &PanelView::screenChanged, [=](QScreen *screen) {
        setScreen(screen);
        setSurfaceRole();
        setupGeometry();
    });

    // Change position when panel size changes
    connect(m_panelView, &PanelView::thicknessChanged,
            this, &PanelConfigView::setupGeometry);

    // Change geometry when the containment changes its form factor or location
    connect(parent->containment(), &Hawaii::Shell::Containment::formFactorChanged,
            this, &PanelConfigView::setupGeometry);
    connect(parent->containment(), &Hawaii::Shell::Containment::locationChanged,
            this, &PanelConfigView::setupGeometry);

    // Set context properties
    engine()->rootContext()->setContextProperty("panel", m_panelView);
    engine()->rootContext()->setContextProperty("configView", this);

    // Load QML code
    QString path = m_panelView->containment()->mantle()->package().filePath("panelconfigurationview");
    setSource(QUrl::fromLocalFile(path));

    // Set surface role
    setSurfaceRole();

    // Set up geometry
    setupGeometry();
}

void PanelConfigView::focusOutEvent(QFocusEvent *event)
{
    m_panelView->setConfiguring(false);
    ConfigView::focusOutEvent(event);
}

void PanelConfigView::setSurfaceRole()
{
    QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();

    struct ::wl_output *output = static_cast<struct ::wl_output *>(
                native->nativeResourceForScreen("output", screen()));
    struct ::wl_surface *surface = static_cast<struct ::wl_surface *>(
                native->nativeResourceForWindow("surface", this));

    ShellManager::instance()->shellInterface()->set_config_surface(output, surface);
}

void PanelConfigView::setSurfacePosition()
{
    QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();

    struct ::wl_surface *surface = static_cast<struct ::wl_surface *>(
                native->nativeResourceForWindow("surface", this));

    ShellManager::instance()->shellInterface()->set_position(
                surface, position().x(), position().y());
}

void PanelConfigView::setupGeometry()
{
    if (!m_panelView->containment())
        return;

    // Resize according to the form factor
    switch (m_panelView->containment()->formFactor()) {
    case Hawaii::Shell::Types::Horizontal:
        resize(screen()->size().width(), rootObject()->implicitHeight());
        break;
    case Hawaii::Shell::Types::Vertical:
        resize(rootObject()->implicitWidth(), screen()->size().height());
        break;
    default:
        break;
    }

    // Set position according to containment's location,
    // we don't need to take the available geometry into
    // account here because we can have only one panel for each edge
    switch (m_panelView->containment()->location()) {
    case Hawaii::Shell::Types::LeftEdge:
        setPosition(screen()->availableGeometry().left() + m_panelView->thickness(),
                    screen()->availableGeometry().top());
        break;
    case Hawaii::Shell::Types::TopEdge:
        setPosition(screen()->availableGeometry().left(),
                    screen()->availableGeometry().top() + m_panelView->thickness());
        break;
    case Hawaii::Shell::Types::RightEdge:
        setPosition(screen()->availableGeometry().right() - width() - m_panelView->thickness(),
                    screen()->availableGeometry().top());
        break;
    case Hawaii::Shell::Types::BottomEdge:
        setPosition(screen()->availableGeometry().left(),
                    screen()->availableGeometry().bottom() - height() - m_panelView->thickness());
        break;
    default:
        break;
    }

    // Tell the compositor to actually move this window
    setSurfacePosition();
}

#include "moc_panelconfigview.cpp"
