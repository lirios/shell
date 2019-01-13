/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include <QtQml>
#include <QWaylandQuickExtension>
#include <QWaylandWlShell>
#include <QWaylandXdgShellV5>

#include "qmlregistration.h"

#include "declarative/fpscounter.h"
#include "declarative/indicatorsmodel.h"
#include "declarative/inputsettings.h"
#include "declarative/quickoutput.h"
#include "declarative/screenmodel.h"
#include "declarative/shellsurfaceitem.h"
#include "extensions/gtkshell.h"
#include "extensions/kdeserverdecoration.h"
#include "extensions/liridecoration.h"

#include "outputchangeset.h"
#include "outputconfiguration.h"
#include "outputdevice.h"
#include "outputmanagement.h"
#include "outputs.h"
#include "screencast.h"
#include "screenshooter.h"

#ifndef Q_COMPOSITOR_DECLARE_QUICK_PARENT_CLASS
#define Q_COMPOSITOR_DECLARE_QUICK_PARENT_CLASS(className) \
    class Q_WAYLAND_COMPOSITOR_EXPORT className##QuickParent : public className \
    { \
/* qmake ignore Q_OBJECT */ \
        Q_OBJECT \
        Q_PROPERTY(QQmlListProperty<QObject> data READ data DESIGNABLE false) \
        Q_CLASSINFO("DefaultProperty", "data") \
    public: \
        QQmlListProperty<QObject> data() \
        { \
            return QQmlListProperty<QObject>(this, this, \
                                             &className##QuickParent::appendFunction, \
                                             &className##QuickParent::countFunction, \
                                             &className##QuickParent::atFunction, \
                                             &className##QuickParent::clearFunction); \
        } \
        static void appendFunction(QQmlListProperty<QObject> *list, QObject *object) \
        { \
            static_cast<className##QuickParent *>(list->data)->m_children.append(object); \
        } \
        static int countFunction(QQmlListProperty<QObject> *list) \
        { \
            return static_cast<className##QuickParent *>(list->data)->m_children.size(); \
        } \
        static QObject *atFunction(QQmlListProperty<QObject> *list, int index) \
        { \
            return static_cast<className##QuickParent *>(list->data)->m_children.at(index); \
        } \
        static void clearFunction(QQmlListProperty<QObject> *list) \
        { \
            static_cast<className##QuickParent *>(list->data)->m_children.clear(); \
        } \
    private: \
        QVector<QObject *> m_children; \
    };
#endif

Q_COMPOSITOR_DECLARE_QUICK_PARENT_CLASS(QuickOutput)
Q_COMPOSITOR_DECLARE_QUICK_EXTENSION_CLASS(QWaylandWlShell)
Q_COMPOSITOR_DECLARE_QUICK_PARENT_CLASS(QWaylandWlShellSurface)
Q_COMPOSITOR_DECLARE_QUICK_EXTENSION_CLASS(QWaylandXdgShellV5)
Q_COMPOSITOR_DECLARE_QUICK_PARENT_CLASS(QWaylandXdgSurfaceV5)
Q_COMPOSITOR_DECLARE_QUICK_PARENT_CLASS(QWaylandXdgPopupV5)
Q_COMPOSITOR_DECLARE_QUICK_EXTENSION_CLASS(GtkShell)
Q_COMPOSITOR_DECLARE_QUICK_PARENT_CLASS(GtkSurface)
Q_COMPOSITOR_DECLARE_QUICK_EXTENSION_CLASS(KdeServerDecorationManager)
Q_COMPOSITOR_DECLARE_QUICK_EXTENSION_CLASS(LiriDecorationManager)

void registerPrivateTypes()
{
    const char uri[] = "Liri.private.shell";
    const int versionMajor = 1;
    const int versionMinor = 0;

    // Components

    qmlRegisterType<FpsCounter>(uri, versionMajor, versionMinor, "FpsCounter");
    qmlRegisterType<IndicatorsModel>(uri, versionMajor, versionMinor, "IndicatorsModel");
    qmlRegisterType<InputSettings>(uri, versionMajor, versionMinor, "InputSettings");
    qmlRegisterType<QuickOutputQuickParent>(uri, versionMajor, versionMinor, "WaylandOutput");
    qmlRegisterType<ScreenModel>(uri, versionMajor, versionMinor, "ScreenModel");
    qmlRegisterUncreatableType<ScreenMode>(uri, versionMajor, versionMinor, "ScreenMode",
                                           QLatin1String("Cannot create instance of ScreenMode"));
    qmlRegisterUncreatableType<ScreenItem>(uri, versionMajor, versionMinor, "ScreenItem",
                                           QLatin1String("Cannot create instance of ScreenItem"));
    qmlRegisterType<ShellSurfaceItem>(uri, versionMajor, versionMinor, "ShellSurfaceItem");

    // Wayland protocols

    qmlRegisterType<QWaylandWlShellQuickExtension>(uri, versionMajor, versionMinor, "WlShell");
    qmlRegisterType<QWaylandWlShellSurfaceQuickParent>(uri, versionMajor, versionMinor, "WlShellSurface");

    qmlRegisterUncreatableType<QWaylandXdgShellV5>(uri, versionMajor, versionMinor, "XdgShellV5Base",
                                                   QLatin1String("Cannot create instance of XdgShellV5Base"));
    qmlRegisterType<QWaylandXdgShellV5QuickExtension>(uri, versionMajor, versionMinor, "XdgShellV5");
    qmlRegisterType<QWaylandXdgSurfaceV5QuickParent>(uri, versionMajor, versionMinor, "XdgSurfaceV5");
    qmlRegisterType<QWaylandXdgPopupV5QuickParent>(uri, versionMajor, versionMinor, "XdgPopupV5");

    qmlRegisterType<GtkShellQuickExtension>(uri, versionMajor, versionMinor, "GtkShell");
    qmlRegisterType<GtkSurfaceQuickParent>(uri, versionMajor, versionMinor, "GtkSurface");

    qmlRegisterType<KdeServerDecorationManagerQuickExtension>(uri, versionMajor, versionMinor, "KdeServerDecorationManager");
    qmlRegisterUncreatableType<KdeServerDecoration>(uri, versionMajor, versionMinor, "KdeServerDecoration",
                                                    QLatin1String("Cannot create instance of KdeServerDecoration"));

    qmlRegisterType<LiriDecorationManagerQuickExtension>(uri, versionMajor, versionMinor, "LiriDecorationManager");
    qmlRegisterUncreatableType<LiriDecoration>(uri, versionMajor, versionMinor, "LiriDecoration",
                                               QLatin1String("Cannot create instance of LiriDecoration"));

    // D-Bus interfaces

    qmlRegisterType<OutputConfiguration>(uri, versionMajor, versionMinor, "OutputConfiguration");
    qmlRegisterType<OutputDevice>(uri, versionMajor, versionMinor, "OutputDevice");
    qmlRegisterType<OutputManagement>(uri, versionMajor, versionMinor, "OutputManagement");
    qmlRegisterUncreatableType<OutputChangeset>(uri, versionMajor, versionMinor, "OutputChangeset",
                                                QLatin1String("Cannot create instance of OutputChangeset"));

    qmlRegisterType<Outputs>(uri, versionMajor, versionMinor, "Outputs");

    qmlRegisterType<ScreenCast>(uri, versionMajor, versionMinor, "ScreenCast");

    qmlRegisterType<Screenshooter>(uri, versionMajor, versionMinor, "Screenshooter");
    qmlRegisterUncreatableType<Screenshot>(uri, versionMajor, versionMinor, "Screenshot",
                                           QLatin1String("Cannot create instance of Screenshot"));
}

#include "qmlregistration.moc"
