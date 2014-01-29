/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2013-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 * Copyright (C) 2013 Marco Martin <mart@kde.org>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:LGPL2.1+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
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

#include <QtCore/QDebug>
#include <QtGui/QGuiApplication>
#include <QtGui/QWindow>
#include <QtGui/QScreen>
#include <QtGui/qpa/qplatformnativeinterface.h>
#include <QtQml/QQmlContext>

#include <HawaiiShell/Containment>
#include <HawaiiShell/PluginLoader>

#include "element.h"
#include "elementfactory.h"
#include "panelview.h"
#include "panelsurface.h"
#include "shellmanager.h"

using namespace Hawaii::Shell;

static PanelSurface::alignment convertAlignment(Qt::Alignment alignment)
{
    switch (alignment) {
    case Qt::AlignRight:
        return PanelSurface::alignment_right;
    case Qt::AlignCenter:
        return PanelSurface::alignment_center;
    default:
        break;
    }

    return PanelSurface::alignment_left;
}

PanelView::PanelView(ShellUi *corona, QScreen *screen)
    : QuickView(corona, new QWindow(screen))
    , m_alignment(Qt::AlignLeft)
    , m_offset(0)
    , m_thickness(60)
    , m_minimumLength(0)
    , m_maximumLength(0)
    , m_surface(new PanelSurface())
{
    // Let QML see us
    rootContext()->setContextProperty("view", this);

    // Resize root object to view
    setResizeMode(QuickView::SizeRootObjectToView);

    // Load containment package
    Package package = PluginLoader::instance()->loadPackage(
                QStringLiteral("Hawaii/Shell/Containment"));
    package.setPath("org.hawaii.containments.panel");

    // Create and load containment
    Containment *containment = new Containment(corona, this);
    containment->setContextProperty(QStringLiteral("panel"), QVariant::fromValue(this));
    containment->setPackage(package);

    // Load QML source file
    setSource(QUrl::fromLocalFile(corona->package().filePath(
                                      "views", QStringLiteral("PanelView.qml"))));

    // Set containment
    setContainment(containment);
    setLocation(Types::BottomEdge);

    // Initialize settings
    if (formFactor() == Types::Vertical) {
        m_length = screen->size().height();
        m_minimumLength = screen->size().height();
        m_maximumLength = screen->size().height();
    } else {
        m_length = screen->size().width();
        m_minimumLength = screen->size().width();
        m_maximumLength = screen->size().width();
    }

    // Setup configuration
    static int panelId = 0;
    const QString section = QString("shell/%1/panels/panel%2")
            .arg(ShellManager::instance()->shell())
            .arg(QString::number(panelId++));
    m_configuration = new QConfiguration(this, section, this);

    // Set panel size
    restore();

    // Set Wayland window type
    setWindowType();

    // React to changes
    connect(this, &QWindow::visibleChanged,
            this, &PanelView::dockPanel);
    connect(this, &QWindow::screenChanged,
            this, &PanelView::dockPanel);
    connect(this, &QuickView::locationChanged,
            this, &PanelView::dockPanel);
    connect(screen, &QScreen::geometryChanged,
            this, &PanelView::dockPanel);

    // Debugging message
    qDebug() << "-> Created PanelView with geometry"
             << geometry();
}

PanelView::~PanelView()
{
    delete m_surface;
    qDeleteAll(m_elements);
}

Qt::Alignment PanelView::alignment() const
{
    return m_alignment;
}

void PanelView::setAlignment(Qt::Alignment alignment)
{
    if (m_alignment != alignment) {
        m_alignment = alignment;
        Q_EMIT alignmentChanged();
        dockPanel();
    }
}

int PanelView::offset() const
{
    return m_offset;
}

void PanelView::setOffset(int value)
{
    if (m_offset != value) {
        m_offset = value;
        Q_EMIT offsetChanged();
        dockPanel();
    }
}

int PanelView::thickness() const
{
    return m_thickness;
}

void PanelView::setThickness(int value)
{
    if (value != thickness()) {
        m_thickness = value;
        Q_EMIT thicknessChanged();
        dockPanel();
    }
}

int PanelView::length() const
{
    return m_length;
}

void PanelView::setLength(int value)
{
    if (value != length()) {
        m_length = value;
        Q_EMIT lengthChanged();
        dockPanel();
    }
}

int PanelView::minimumLength() const
{
    return m_minimumLength;
}

void PanelView::setMinimumLength(int value)
{
    if (m_minimumLength == value)
        return;

    if (m_maximumLength < value)
        setMaximumLength(value);

    if (formFactor() == Hawaii::Shell::Types::Vertical)
        setMinimumHeight(value);
    else
        setMinimumWidth(value);

    m_minimumLength = value;
    Q_EMIT minimumLengthChanged();

    dockPanel();
}

int PanelView::maximumLength() const
{
    return m_maximumLength;
}

void PanelView::setMaximumLength(int value)
{
    if (m_maximumLength == value)
        return;

    if (m_minimumLength > value)
        setMinimumLength(value);

    if (formFactor() == Hawaii::Shell::Types::Vertical)
        setMaximumHeight(value);
    else
        setMaximumWidth(value);

    m_maximumLength = value;
    Q_EMIT maximumLengthChanged();

    dockPanel();
}

QStringList PanelView::elements() const
{
    return m_elementsSet.toList();
}

void PanelView::addElement(const QString &name)
{
    m_elementsSet.insert(name);

    Hawaii::Shell::Element *element = ElementFactory::createElement(name);
    m_elements.append(element);
    Q_EMIT elementAdded(element);
}

void PanelView::setWindowType()
{
    QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();

    struct ::wl_surface *surface = static_cast<struct ::wl_surface *>(
                native->nativeResourceForWindow("surface", this));

    struct ::wl_hawaii_panel *panel =
            ShellManager::instance()->panelManagerInterface()->set_panel(surface);
    m_surface->init(panel);
}

void PanelView::restore()
{
    // Make sure offset is positive
    if (m_alignment != Qt::AlignCenter)
        m_offset = qMax<int>(0, m_offset);

    // Minimum and maximum size
    if (formFactor() == Types::Vertical) {
        const int minSize = 10;
        const int maxSize = screen()->size().height() - m_offset;
        m_minimumLength = qBound<int>(minSize, m_minimumLength, maxSize);
        m_maximumLength = qBound<int>(minSize, m_maximumLength, maxSize);

        resize(thickness(), qBound<int>(minSize, length(), maxSize));

        setMinimumHeight(m_minimumLength);
        setMaximumHeight(m_maximumLength);
    } else {
        const int minSize = 10;
        const int maxSize = screen()->size().width() - m_offset;
        m_minimumLength = qBound<int>(minSize, m_minimumLength, maxSize);
        m_maximumLength = qBound<int>(minSize, m_maximumLength, maxSize);

        resize(qBound<int>(minSize, length(), maxSize), thickness());

        setMinimumWidth(m_minimumLength);
        setMaximumWidth(m_maximumLength);
    }

    // Inform the compositor
    if (m_surface->isInitialized()) {
        m_surface->set_alignment(convertAlignment(m_alignment));
        m_surface->set_offset(m_offset);
        m_surface->set_thickness(m_thickness);
        m_surface->set_length(m_length);
        m_surface->set_min_length(m_minimumLength);
        m_surface->set_max_length(m_maximumLength);
    }

    // Inform observers
    Q_EMIT offsetChanged();
    Q_EMIT alignmentChanged();
    Q_EMIT minimumLengthChanged();
    Q_EMIT maximumLengthChanged();
}

void PanelView::dockPanel()
{
    restore();

    if (!m_surface->isInitialized())
        return;

    QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();

    struct ::wl_output *output = static_cast<struct ::wl_output *>(
                native->nativeResourceForScreen("output", screen()));

    PanelSurface::edge edge;
    switch (location()) {
    case Hawaii::Shell::Types::LeftEdge:
        edge = PanelSurface::edge_left;
        break;
    case Hawaii::Shell::Types::TopEdge:
        edge = PanelSurface::edge_top;
        break;
    case Hawaii::Shell::Types::RightEdge:
        edge = PanelSurface::edge_right;
        break;
    case Hawaii::Shell::Types::BottomEdge:
        edge = PanelSurface::edge_bottom;
        break;
    default:
        return;
    }

    m_surface->dock(edge, output);
}

#include "moc_panelview.cpp"
