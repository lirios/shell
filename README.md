Shell
=====

[![ZenHub.io](https://img.shields.io/badge/supercharged%20by-zenhub.io-blue.svg)](https://zenhub.io)

[![License](https://img.shields.io/badge/license-GPLv3.0-blue.svg)](https://www.gnu.org/licenses/gpl-3.0.html)
[![GitHub release](https://img.shields.io/github/release/lirios/shell.svg)](https://github.com/lirios/shell)
[![Build Status](https://travis-ci.org/lirios/shell.svg?branch=develop)](https://travis-ci.org/lirios/shell)
[![GitHub issues](https://img.shields.io/github/issues/lirios/shell.svg)](https://github.com/lirios/shell/issues)
[![Maintained](https://img.shields.io/maintenance/yes/2016.svg)](https://github.com/lirios/shell/commits/develop)

Responsive shell for Liri OS.

![Desktop](.github/desktop.png)

## Dependencies

Compiler requirements:

* [gcc >= 4.8](https://gcc.gnu.org/gcc-4.8/) or
* [Clang](http://clang.llvm.org/)

Qt >= 5.8.0 with at least the following modules is required:

 * [qtbase](http://code.qt.io/cgit/qt/qtbase.git)
 * [qtdeclarative](http://code.qt.io/cgit/qt/qtdeclarative.git)
 * [qtwayland](http://code.qt.io/cgit/qt/qtwayland.git)
 * [qtgraphicaleffects](http://code.qt.io/cgit/qt/qtgraphicaleffects.git)
 * [qtquickcontrols2](http://code.qt.io/cgit/qt/qtquickcontrols2.git)
 * [qtsvg](http://code.qt.io/cgit/qt/qtsvg.git)

The following modules and their dependencies are required:

 * [ECM >= 1.7.0](http://quickgit.kde.org/?p=extra-cmake-modules.git)
 * [fluid](https://github.com/lirios/fluid)
 * [liri-wayland](https://github.com/lirios/wayland)
 * [liri-workspace](https://github.com/lirios/workspace)
 * [pam](http://www.linux-pam.org/)
 * [libqtxdg](https://github.com/lxde/libqtxdg)
 * [qtaccountsservice](https://github.com/hawaii-desktop/qtaccountsservice)

## Installation

From the root of the repository, run:

```sh
mkdir build; cd build
cmake .. -DKDE_INSTALL_USE_QT_SYS_PATHS=ON
make
make install # use sudo if necessary
```

On the `cmake` line, you can specify additional configuration parameters:

 * `-DCMAKE_INSTALL_PREFIX=/path/to/install` (for example, `/opt/liri` or `/usr`)
 * `-DCMAKE_BUILD_TYPE=<build_type>`, where `<build_type>` is one of:
   * **Debug:** debug build
   * **Release:** release build
   * **RelWithDebInfo:** release build with debugging information

## Licensing

Licensed under the terms of the GNU General Public License version 3 or,
at your option, any later version.

## Notes

### Logging categories

Qt 5.2 introduced logging categories and Liri Shell takes advantage of
them to make debugging easier.

Please refer to the [Qt](http://doc.qt.io/qt-5/qloggingcategory.html) documentation
to learn how to enable them.

### Available categories

 * Compositor:
   * **liri.compositor:** Compositor
   * **liri.processlauncher:** Process launcher and application tracker
   * **liri.screensaver:** Lock, idle and inhibit interface
   * **liri.session:** Manages the session
   * **liri.loginmanager:** login manager subsystem
   * **liri.loginmanager.logind:** login manager subsystem (logind backend)

 * Launcher QML plugin:
   * **liri.launcher:** Launcher model and items
   * **liri.launcher.appsmodel:** Applications model

## Components

*liri-shell*

Compositor executable.

*liri-session*

Manages the session, drives the compositor, runs autostart programs
and launches applications for the application launcher.

Autostarts the D-Bus session if needed and can logout an existing
session with:

```sh
liri-session --logout
```

## Running on another window system

By default the Liri session runs without a particular window system, however
you can run it inside another window system.

### X11

To run windowed inside a X11 session:

```sh
liri-session -platform xcb
```

### Wayland

To run windowed inside a Wayland session:

```sh
liri-session -platform wayland
```

Some compositors such as Weston support the fullscreen-shell protocol that
allows the Liri desktop environment to be presented fullscreen nested into
another compositor.

Let's take Weston as an example. First you need to run it with the fullscreen-shell
protocol enabled:

```sh
weston --shell=fullscreen-shell.so
```

Then you need to run `liri-session` like this:

```sh
QT_WAYLAND_SHELL_INTEGRATION=fullscreen-shell liri-session -platform wayland 
```

## QML JavaScript debugger

Developers can debug Liri Shell with Qt Creator and the QML JavaScript debugger.

Set the debug port and run the shell:

```sh
LIRI_SHELL_DEBUG_PORT=3768 liri-shell
```

In the example above we are using the default port which is 3768.
Now from Qt Creator click on Debug -> Start Debugging -> Attach to QML port and specify
the 3768 port.

See the [Qt Creator manual](http://qt-project.org/doc/qtcreator-3.0/creator-debugging-qml.html) for more information.
