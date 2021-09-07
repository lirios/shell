// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QtQml>

#ifndef QT_WAYLAND_COMPOSITOR_QUICK
#  define QT_WAYLAND_COMPOSITOR_QUICK
#endif

#include "abstracttitlebar.h"
#include "appidandicon.h"
#include "dbus/multimediakeysserver.h"
#include "dbus/osdserver.h"
#include "chromeitem.h"
#include "desktoplayout.h"
#include "extensionsmodel.h"
#include "fpscounter.h"
#include "hotspot.h"
#include "inputsettings.h"
#include "keyeventfilter.h"
#include "quickoutput.h"
#include "screencast.h"
#include "screenmodel.h"
#include "waylandcompositor.h"
#include "waylandcursorgrabber.h"
#include "windowmousetracker.h"

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

Q_COMPOSITOR_DECLARE_QUICK_PARENT_CLASS(WaylandCompositor)

class ShellPrivatePlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")
public:
    void registerTypes(const char *uri)
    {
        // @uri Liri.private.shell
        Q_ASSERT(QLatin1String(uri) == QLatin1String("Liri.private.shell"));

        const int versionMajor = 1;
        const int versionMinor = 0;

        // Components

        qmlRegisterType<AbstractTitleBar>(uri, versionMajor, versionMinor, "AbstractTitleBar");
        qmlRegisterType<AppIdAndIcon>(uri, versionMajor, versionMinor, "AppIdAndIcon");
        qmlRegisterType<ChromeItem>(uri, versionMajor, versionMinor, "ChromeItem");
        qmlRegisterType<DesktopLayout>(uri, versionMajor, versionMinor, "DesktopLayout");
        qmlRegisterType<ExtensionsModel>(uri, versionMajor, versionMinor, "ExtensionsModel");
        qmlRegisterType<FpsCounter>(uri, versionMajor, versionMinor, "FpsCounter");
        qmlRegisterType<HotSpot>(uri, versionMajor, versionMinor, "HotSpot");
        qmlRegisterType<InputSettings>(uri, versionMajor, versionMinor, "InputSettings");
        qmlRegisterType<KeyEventFilter>(uri, versionMajor, versionMinor, "KeyEventFilter");
        qmlRegisterType<QuickOutput>(uri, versionMajor, versionMinor, "WaylandOutput");
        qmlRegisterType<ScreenCast>(uri, versionMajor, versionMinor, "ScreenCast");
        qmlRegisterType<ScreenModel>(uri, versionMajor, versionMinor, "ScreenModel");
        qmlRegisterUncreatableType<ScreenMode>(uri, versionMajor, versionMinor, "ScreenMode",
                                               QStringLiteral("Cannot create instance of ScreenMode"));
        qmlRegisterUncreatableType<ScreenItem>(uri, versionMajor, versionMinor, "ScreenItem",
                                               QStringLiteral("Cannot create instance of ScreenItem"));
        qmlRegisterType<WaylandCompositorQuickParent>(uri, versionMajor, versionMinor, "WaylandCompositor");
        qmlRegisterType<WaylandCursorGrabber>(uri, versionMajor, versionMinor, "WaylandCursorGrabber");
        qmlRegisterType<WindowMouseTracker>(uri, versionMajor, versionMinor, "WindowMouseTracker");

        // D-Bus interfaces

        qmlRegisterType<OsdServer>(uri, versionMajor, versionMinor, "OsdServer");
        qmlRegisterType<MultimediaKeysServer>(uri, versionMajor, versionMinor, "MultimediaKeysServer");
    }
};

#include "plugin.moc"
