Shell
=====

[![License](https://img.shields.io/badge/license-GPLv3.0-blue.svg)](https://www.gnu.org/licenses/gpl-3.0.html)
[![GitHub release](https://img.shields.io/github/release/lirios/shell.svg)](https://github.com/lirios/shell)
[![Build Status](https://travis-ci.org/lirios/shell.svg?branch=develop)](https://travis-ci.org/lirios/shell)
[![GitHub issues](https://img.shields.io/github/issues/lirios/shell.svg)](https://github.com/lirios/shell/issues)
[![Maintained](https://img.shields.io/maintenance/yes/2018.svg)](https://github.com/lirios/shell/commits/develop)

Responsive shell for the Liri desktop.

![Desktop](.github/desktop.png)

## Dependencies

Compiler requirements:

* [gcc >= 4.8](https://gcc.gnu.org/gcc-4.8/) or
* [Clang](http://clang.llvm.org/)

Qt >= 5.11.0 with at least the following modules is required:

 * [qtbase](http://code.qt.io/cgit/qt/qtbase.git)
 * [qtdeclarative](http://code.qt.io/cgit/qt/qtdeclarative.git)
 * [qtwayland](http://code.qt.io/cgit/qt/qtwayland.git)
 * [qtgraphicaleffects](http://code.qt.io/cgit/qt/qtgraphicaleffects.git)
 * [qtquickcontrols2](http://code.qt.io/cgit/qt/qtquickcontrols2.git)
 * [qtsvg](http://code.qt.io/cgit/qt/qtsvg.git)

The following modules and their dependencies are required:

 * [cmake](https://gitlab.kitware.com/cmake/cmake) >= 3.10.0
 * [cmake-shared](https://github.com/lirios/cmake-shared.git) >= 1.0.0
 * [fluid](https://github.com/lirios/fluid) >= 1.0.0
 * [qtaccountsservice](https://github.com/lirios/qtaccountsservice) >= 1.2.0
 * [qtgsettings](https://github.com/lirios/qtgsettings) >= 1.1.0
 * [libliri](https://github.com/lirios/libliri)
 * [liri-wayland](https://github.com/lirios/wayland)
 * [pam](http://www.linux-pam.org/)
 * [polkit-qt5](http://quickgit.kde.org/?p=polkit-qt-1.git)
 * [solid](http://quickgit.kde.org/?p=solid.git)

Optional dependencies:

 * [qml-xwayland](https://github.com/lirios/qml-xwayland.git)

## Installation

```sh
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=/path/to/prefix ..
make
make install # use sudo if necessary
```

Replace `/path/to/prefix` to your installation prefix.
Default is `/usr/local`.

You can also append the following options to the `cmake` command:

 * `-DLIRI_SHELL_DEVELOPMENT_BUILD:BOOL=ON`: Enable feature for development such as the
   ability to load arbitrary QML files.
 * `-DLIRI_SHELL_WITH_SCREENCAST:BOOL=ON`: Enable experimental screencast support.
 * `-DINSTALL_SYSTEMDUSERUNITDIR=/path/to/systemd/user`: Path to install systemd user units (default: `/usr/local/lib/systemd/user`).

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
   * **liri.shell:** Shell
   * **liri.gtkshell:** Gtk shell support
   * **liri.outputmanagement:** Output management
   * **liri.decoration:** Window decorations

 * Launcher QML plugin:
   * **liri.launcher:** Launcher model and items
   * **liri.launcher.appsmodel:** Applications model

 * MPRIS2 QML plugin:
   * **liri.mpris2:** MPRIS2 engine
   * **liri.mpris2.player:** MPRIS2 player interface

 * PolicyKit QML plugin:
   * **liri.policykit:** PolicyKit agent

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

The platform plugin to use is automatically detected based on the environment,
it can be one of the following:

 * wayland: Run inside another Wayland compositor
 * xcb: Run inside a X11 session
 * liri: Uses DRM/KMS or a device specific EGL integration

You can override the automatic detection, if it doesn't work as intended.

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

Some compositors, such as Weston, support the fullscreen-shell protocol that
allows a compositor to be nested into another compositor.

Let's take Weston as an example. First you need to run it with the fullscreen-shell
protocol enabled:

```sh
weston --shell=fullscreen-shell.so
```

Then you need to run `liri-session` like this:

```sh
QT_WAYLAND_SHELL_INTEGRATION=fullscreen-shell liri-session -platform wayland 
```

### DRM/KMS

To make sure the Liri session runs without another window system type:

```sh
liri-session -platform liri
```

## Fake screen configuration

You can simulate a fake screen configuration on X11 and Wayland.

Run:

```sh
liri-session -platform xcb --fake-screen screenconfig.json # replace xcb with wayland if you are running on Wayland
```

Here's the contents of `screenconfig.json`:

```json
{
	"outputs": [{
		"name": "Screen 1",
		"primary": false,
		"scale": 1,
		"position": {
			"x": 0,
			"y": 0
		},
		"mode": {
			"size": {
				"width": 1024,
				"height": 768
			},
			"refreshRate": 60000
		},
		"physicalSize": {
			"width": 350,
			"height": 260
		}
	},
	{
		"name": "Screen 2",
		"primary": true,
		"scale": 1,
		"position": {
			"x": 1024,
			"y": 0
		},
		"mode": {
			"size": {
				"width": 1024,
				"height": 768
			},
			"refreshRate": 60000
		},
		"physicalSize": {
			"width": 350,
			"height": 260
		}
	}]
}
```

## QML JavaScript debugger

Developers can debug Liri Shell with Qt Creator and the QML JavaScript debugger.

Set the debug port and run the shell:

```sh
liri-session -qmljsdebugger=port:3768
```

In the example above we are using the default port which is 3768.
Now from Qt Creator click on Debug -> Start Debugging -> Attach to QML port and specify
the 3768 port.

See the [Qt Creator manual](http://doc.qt.io/qtcreator/creator-debugging-qml.html) for more information.
