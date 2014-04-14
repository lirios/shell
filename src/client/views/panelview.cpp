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

#include <QtCore/QDebug>
#include <QtGui/QGuiApplication>
#include <QtGui/QWindow>
#include <QtGui/QScreen>
#include <QtGui/qpa/qplatformnativeinterface.h>
#include <QtQml/QQmlContext>

#include <Hawaii/PluginLoader>
#include <HawaiiShell/Containment>
#include <HawaiiShell/ContainmentItem>
#include <HawaiiShell/Element>
#include <HawaiiShell/ElementItem>

#include "panelconfigview.h"
#include "panelview.h"
#include "panelsurface.h"
#include "shellmanager.h"

using namespace Hawaii;

#define KEY_LOCATION      QStringLiteral("location")
#define KEY_IMMUTABLE     QStringLiteral("immutable")
#define KEY_MAXIMIZED     QStringLiteral("maximized")
#define KEY_ALIGNMENT     QStringLiteral("alignment")
#define KEY_OFFSET        QStringLiteral("offset")
#define KEY_LENGTH        QStringLiteral("length")
#define KEY_MINLENGTH     QStringLiteral("minLength")
#define KEY_MAXLENGTH     QStringLiteral("maxLength")
#define KEY_THICKNESS     QStringLiteral("thickness")

#define DEFAULT_LOCATION  Types::BottomEdge
#define DEFAULT_IMMUTABLE false
#define DEFAULT_MAXIMIZED true
#define DEFAULT_ALIGNMENT Qt::AlignLeft
#define DEFAULT_OFFSET    0
#define DEFAULT_THICKNESS 60
#define DEFAULT_LENGTH    640
#define DEFAULT_MINLENGTH 640
#define DEFAULT_MAXLENGTH 640

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

PanelView::PanelView(ShellUi *mantle, QScreen *screen)
    : QuickView(mantle, new QWindow(screen))
    , m_screen(screen)
    , m_screenGeometry(screen->geometry())
    , m_maximized(DEFAULT_MAXIMIZED)
    , m_alignment(DEFAULT_ALIGNMENT)
    , m_offset(DEFAULT_OFFSET)
    , m_thickness(DEFAULT_THICKNESS)
    , m_length(DEFAULT_LENGTH)
    , m_minimumLength(DEFAULT_MINLENGTH)
    , m_maximumLength(DEFAULT_MAXLENGTH)
    , m_surface(new PanelSurface())
{
    // Let QML see us
    rootContext()->setContextProperty("panel", this);

    // Resize root object to view
    setResizeMode(QuickView::SizeRootObjectToView);

    // Load containment package
    Package package = PluginLoader::instance()->loadPackage(
                PluginLoader::ContainmentPlugin);
    package.setPath("org.hawaii.containments.panel");

    // Create and load containment
    Containment *containment = new Containment(mantle, this);
    containment->setPackage(package);

    // Create the containment graphical representation
    ContainmentItem *containmentItem = new ContainmentItem(containment);
    containmentItem->initialize();

    // Load QML source file
    setSource(QUrl::fromLocalFile(mantle->shellPackage().filePath(
                                      "views", QStringLiteral("PanelView.qml"))));

    // Set containment
    setContainment(containment);

    // Make sure the panel is initially hidden
    setVisible(false);

    // Determine the screen number
    const QList<QScreen *> screens = QGuiApplication::screens();
    for (int i = 0; i < screens.length(); i++) {
        if (screens.at(i) == screen) {
            m_screenNumber = i;
            break;
        }
    }

    // Determine the panel number by incrementing the number of panels
    // for this screen; this would work because we have up to 4 panels
    // for each screen (one for every edge)
    int panelCounter = 0;
    for (PanelView *view: ShellManager::instance()->mantle()->panels()) {
        if (view->screen() == screen)
            panelCounter++;
    }
    m_panelNum = panelCounter + 1;

    // Setup configuration
    m_settings = new QStaticConfiguration(this);
    m_settings->setCategory(settingsCategory());
    loadSettings();

    // Set Wayland window type
    setWindowType();

    // React to changes
    connect(this, &QWindow::visibleChanged,
            this, &PanelView::dockPanel);
    connect(this, &QWindow::screenChanged,
            this, &PanelView::dockPanel);
    connect(screen, &QScreen::geometryChanged,
            this, &PanelView::dockPanel);
    connect(this, &QuickView::locationChanged,
            this, &PanelView::containmentLocationChanged);
    connect(this, &QuickView::immutableChanged,
            this, &PanelView::containmentImmutableChanged);

    // Debugging message
    qDebug() << "-> Created PanelView with geometry"
             << geometry();
}

PanelView::~PanelView()
{
    // Delete panel surface
    delete m_surface;
    m_surface = nullptr;

    // Save settings
    saveSettings();
}

bool PanelView::isMaximized() const
{
    return m_maximized;
}

void PanelView::setMaximized(bool value)
{
    if (m_maximized == value)
        return;

    // A maximized panel doesn't need alignment, neither offset nor length
    m_alignment = Qt::AlignLeft;
    m_offset = 0;
    if (formFactor() == Types::Vertical)
        m_length = screen()->size().height();
    else
        m_length = screen()->size().width();
    m_minimumLength = m_length;
    m_maximumLength = m_length;

    // Set value
    m_maximized = value;
    Q_EMIT maximizedChanged();

    // Save settings
    m_settings->setValue(KEY_MAXIMIZED, m_maximized);
    if (m_maximized) {
        m_settings->removeKey(KEY_ALIGNMENT);
        m_settings->removeKey(KEY_OFFSET);
        m_settings->removeKey(KEY_LENGTH);
        m_settings->removeKey(KEY_MINLENGTH);
        m_settings->removeKey(KEY_MAXLENGTH);
    }

    // Dock panel
    dockPanel();
}

Qt::Alignment PanelView::alignment() const
{
    return m_alignment;
}

void PanelView::setAlignment(Qt::Alignment alignment)
{
    // Can't set alignment if the panel is maximized
    if (m_maximized)
        return;

    if (m_alignment != alignment) {
        // Set value
        m_alignment = alignment;
        Q_EMIT alignmentChanged();

        // Save settings
        m_settings->setValue(KEY_ALIGNMENT, (int)m_alignment);

        // Dock panel
        dockPanel();
    }
}

int PanelView::offset() const
{
    return m_offset;
}

void PanelView::setOffset(int value)
{
    // Can't set offset if the panel is maximized
    if (m_maximized)
        return;

    if (m_offset != value) {
        // Set value
        m_offset = value;
        Q_EMIT offsetChanged();

        // Save settings
        m_settings->setValue(KEY_OFFSET, m_offset);

        // Dock panel
        dockPanel();
    }
}

int PanelView::thickness() const
{
    return m_thickness;
}

void PanelView::setThickness(int value)
{
    if (m_thickness != value) {
        // Set value
        m_thickness = value;
        Q_EMIT thicknessChanged();

        // Save settings
        m_settings->setValue(KEY_THICKNESS, m_thickness);

        // Dock panel
        dockPanel();
    }
}

int PanelView::length() const
{
    return m_length;
}

void PanelView::setLength(int value)
{
    // Can't set length if the panel is maximized
    if (m_maximized)
        return;

    if (value != length()) {
        // Set value
        m_length = value;
        Q_EMIT lengthChanged();

        // Save settings
        m_settings->setValue(KEY_LENGTH, m_length);

        // Dock panel
        dockPanel();
    }
}

int PanelView::minimumLength() const
{
    return m_minimumLength;
}

void PanelView::setMinimumLength(int value)
{
    // Can't set length if the panel is maximized
    if (m_maximized)
        return;

    // Just ignore if it doesn't change
    if (m_minimumLength == value)
        return;

    // Expand maximum length if needed
    if (m_maximumLength < value) {
        m_maximumLength = value;
        Q_EMIT maximumLengthChanged();
    }

    // Set minimum size
    if (formFactor() == Types::Vertical)
        setMinimumHeight(value);
    else
        setMinimumWidth(value);

    // Set value
    m_minimumLength = value;
    Q_EMIT minimumLengthChanged();

    // Save settings
    m_settings->setValue(KEY_MINLENGTH, m_minimumLength);

    // Dock panel
    dockPanel();
}

int PanelView::maximumLength() const
{
    return m_maximumLength;
}

void PanelView::setMaximumLength(int value)
{
    // Can't set length if the panel is maximized
    if (m_maximized)
        return;

    // Just ignore if it doesn't change
    if (m_maximumLength == value)
        return;

    // Expand minimum length if needed
    if (m_minimumLength > value) {
        m_minimumLength = value;
        Q_EMIT minimumLengthChanged();
    }

    // Set maximum size
    if (formFactor() == Types::Vertical)
        setMaximumHeight(value);
    else
        setMaximumWidth(value);

    // Set value
    m_maximumLength = value;
    Q_EMIT maximumLengthChanged();

    // Save settings
    m_settings->setValue(KEY_MAXLENGTH, m_maximumLength);

    // Dock panel
    dockPanel();
}

void PanelView::resizeEvent(QResizeEvent *event)
{
    //saveSettings();
    QuickView::resizeEvent(event);
}

void PanelView::showConfigurationWindow()
{
    // Recreate the window if necessary and show it
    if (!m_configView)
        m_configView = new PanelConfigView(this);
    m_configView.data()->show();
}

void PanelView::hideConfigurationWindow()
{
    // Hide and destroy the window
    if (m_configView)
        m_configView.data()->hide();
}

QString PanelView::settingsCategory() const
{
    return QString("shell/%1/panels/panel%2_%3")
            .arg(mantle()->shell())
            .arg(QString::number(m_screenNumber))
            .arg(QString::number(m_panelNum));
}

void PanelView::loadSettings()
{
    // Without a containment we don't know the location
    if (!containment())
        return;

    // Load containment settings
    setLocation(static_cast<Types::Location>(m_settings->value(KEY_LOCATION, DEFAULT_LOCATION).toInt()));
    setImmutable(m_settings->value(KEY_IMMUTABLE, DEFAULT_IMMUTABLE).toBool());

    // Load panel settings
    m_alignment = static_cast<Qt::Alignment>(m_settings->value(KEY_ALIGNMENT, DEFAULT_ALIGNMENT).toInt());
    m_maximized = m_settings->value(KEY_MAXIMIZED, DEFAULT_MAXIMIZED).toBool();
    m_thickness = m_settings->value(KEY_THICKNESS, DEFAULT_THICKNESS).toInt();

    // Set length and offset according to the form factor
    if (m_maximized) {
        m_alignment = Qt::AlignLeft;
        m_offset = 0;
        if (formFactor() == Types::Vertical)
            m_length = screen()->size().height();
        else
            m_length = screen()->size().width();
        m_minimumLength = m_length;
        m_maximumLength = m_length;

        // Remove useless configuration keys
        m_settings->removeKey(KEY_ALIGNMENT);
        m_settings->removeKey(KEY_OFFSET);
        m_settings->removeKey(KEY_LENGTH);
        m_settings->removeKey(KEY_MINLENGTH);
        m_settings->removeKey(KEY_MAXLENGTH);
    } else {
        m_length = m_settings->value(KEY_LENGTH, DEFAULT_LENGTH).toInt();
        m_offset = m_settings->value(KEY_OFFSET, DEFAULT_OFFSET).toInt();
        m_minimumLength = m_settings->value(KEY_MINLENGTH, DEFAULT_MINLENGTH).toInt();
        m_maximumLength = m_settings->value(KEY_MAXLENGTH, DEFAULT_MAXLENGTH).toInt();
    }

    // Make sure offset is not negative
    if (m_alignment != Qt::AlignCenter)
        m_offset = qMax(0, m_offset);

    // Bounds checking for min and max length and resize
    const int minSize = 100;
    if (formFactor() == Types::Vertical) {
        const int maxSize = screen()->size().height() - offset();
        m_minimumLength = qBound<int>(minSize, m_minimumLength, maxSize);
        m_maximumLength = qBound<int>(minSize, m_maximumLength, maxSize);

        setMinimumHeight(m_minimumLength);
        setMaximumHeight(m_maximumLength);

        resize(m_thickness, qBound<int>(minSize, m_length, maxSize));
    } else {
        const int maxSize = screen()->size().width() - offset();
        m_minimumLength = qBound<int>(minSize, m_minimumLength, maxSize);
        m_maximumLength = qBound<int>(minSize, m_maximumLength, maxSize);

        setMinimumWidth(m_minimumLength);
        setMaximumWidth(m_maximumLength);

        resize(qBound<int>(minSize, m_length, maxSize), m_thickness);
    }
}

void PanelView::saveSettings()
{
    if (containment()) {
        m_settings->setValue(KEY_LOCATION, (int)location());
        m_settings->setValue(KEY_IMMUTABLE, isImmutable());
    }

    m_settings->setValue(KEY_MAXIMIZED, m_maximized);
    m_settings->setValue(KEY_ALIGNMENT, (int)m_alignment);

    if (m_maximized) {
        m_settings->removeKey(KEY_OFFSET);
        m_settings->removeKey(KEY_LENGTH);
        m_settings->removeKey(KEY_MINLENGTH);
        m_settings->removeKey(KEY_MAXLENGTH);
    } else {
        m_settings->setValue(KEY_OFFSET, m_offset);
        m_settings->setValue(KEY_MINLENGTH, m_minimumLength);
        m_settings->setValue(KEY_MAXLENGTH, m_maximumLength);
    }

    if (formFactor() == Types::Vertical) {
        m_settings->setValue(KEY_THICKNESS, size().width());
        if (!m_maximized)
            m_settings->setValue(KEY_LENGTH, size().height());
    } else {
        m_settings->setValue(KEY_THICKNESS, size().height());
        if (!m_maximized)
            m_settings->setValue(KEY_LENGTH, size().width());
    }
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

void PanelView::updateCurrentScreen()
{
    m_screen = screen();
    m_screenGeometry = m_screen->geometry();

    const QList<QScreen *> screens = QGuiApplication::screens();
    for (int i = 0; i < screens.length(); i++) {
        if (screens.at(i) == m_screen) {
            m_screenNumber = i;
            break;
        }
    }

    connect(m_screen, &QScreen::geometryChanged,
            this, &PanelView::dockPanel);

    // TODO: Rename category because the screen is changed and
    // the panel number might have been changes as well
    m_settings->setCategory(settingsCategory());
}

void PanelView::applySizeChanges()
{
    if (formFactor() == Types::Vertical) {
        setMinimumHeight(m_minimumLength);
        setMaximumHeight(m_maximumLength);

        resize(m_thickness, m_length);
    } else {
        setMinimumWidth(m_minimumLength);
        setMaximumWidth(m_maximumLength);

        resize(m_length, m_thickness);
    }
}

void PanelView::recalculateLength()
{
    // Save old values
    int oldLength = m_length;
    int oldMinLength = m_minimumLength;
    int oldMaxLength = m_maximumLength;

    // Recalculate length
    if (formFactor() == Types::Vertical) {
        m_length = (m_length * screen()->size().height()) / m_screenGeometry.height();
        m_minimumLength = (m_minimumLength * screen()->size().height()) / m_screenGeometry.height();
        m_maximumLength = (m_length * screen()->size().height()) / m_screenGeometry.height();
    } else {
        m_length = (m_length * screen()->size().width()) / m_screenGeometry.width();
        m_minimumLength = (m_minimumLength * screen()->size().width()) / m_screenGeometry.width();
        m_maximumLength = (m_maximumLength * screen()->size().width()) / m_screenGeometry.width();
    }

    // Determine if something has changed
    if (oldLength != m_length)
        Q_EMIT lengthChanged();
    if (oldMinLength != m_minimumLength)
        Q_EMIT minimumLengthChanged();
    if (oldMaxLength != m_maximumLength)
        Q_EMIT maximumLengthChanged();
}

void PanelView::containmentLocationChanged()
{
    m_settings->setValue(KEY_LOCATION, (int)location());
    dockPanel();
}

void PanelView::containmentImmutableChanged()
{
    m_settings->setValue(KEY_IMMUTABLE, isImmutable());
}

void PanelView::dockPanel()
{
    // Sanity check
    if (!m_surface || !m_surface->isInitialized())
        return;

    // Don't dock hidden panels
    if (!isVisible())
        return;

    // Is the screen changed?
    if (screen() != m_screen) {
        // Recalculate length
        recalculateLength();

        // Update current screen
        updateCurrentScreen();
    }

    // Resize the panel if necessary
    applySizeChanges();

    // Setup surface on the compositor size
    m_surface->set_alignment(convertAlignment(m_alignment));
    m_surface->set_offset(m_offset);
    m_surface->set_thickness(m_thickness);
    m_surface->set_length(m_length);
    m_surface->set_min_length(m_minimumLength);
    m_surface->set_max_length(m_maximumLength);

    QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();

    struct ::wl_output *output = static_cast<struct ::wl_output *>(
                native->nativeResourceForScreen("output", screen()));

    PanelSurface::edge edge;
    switch (location()) {
    case Hawaii::Types::LeftEdge:
        edge = PanelSurface::edge_left;
        break;
    case Hawaii::Types::TopEdge:
        edge = PanelSurface::edge_top;
        break;
    case Hawaii::Types::RightEdge:
        edge = PanelSurface::edge_right;
        break;
    case Hawaii::Types::BottomEdge:
        edge = PanelSurface::edge_bottom;
        break;
    default:
        return;
    }

    // Dock the panel
    m_surface->dock(edge, output);
}

#include "moc_panelview.cpp"
