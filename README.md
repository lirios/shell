Shell
=====

[![License](https://img.shields.io/badge/license-GPLv3.0-blue.svg)](https://www.gnu.org/licenses/gpl-3.0.html)
[![GitHub release](https://img.shields.io/github/release/lirios/shell.svg)](https://github.com/lirios/shell)
[![GitHub issues](https://img.shields.io/github/issues/lirios/shell.svg)](https://github.com/lirios/shell/issues)
[![CI](https://github.com/lirios/shell/workflows/CI/badge.svg?branch=develop)](https://github.com/lirios/shell/actions?query=workflow%3ACI)

Responsive shell for the Liri desktop.

![Desktop](.github/desktop.png)

## Dependencies

Compiler requirements:

* [gcc >= 4.8](https://gcc.gnu.org/gcc-4.8/) or
* [Clang](http://clang.llvm.org/)

Qt >= 6.6.0 with at least the following modules is required:

 * [qtbase](http://code.qt.io/cgit/qt/qtbase.git)
 * [qtdeclarative](http://code.qt.io/cgit/qt/qtdeclarative.git)
 * [qtwayland](http://code.qt.io/cgit/qt/qtwayland.git)
 * [qtgraphicaleffects](http://code.qt.io/cgit/qt/qtgraphicaleffects.git)
 * [qtquickcontrols2](http://code.qt.io/cgit/qt/qtquickcontrols2.git)
 * [qtsvg](http://code.qt.io/cgit/qt/qtsvg.git)

The following modules and their dependencies are required:

 * [cmake](https://gitlab.kitware.com/cmake/cmake) >= 3.19.0
 * [extra-cmake-modules](https://invent.kde.org/frameworks/extra-cmake-modules) >= 5.245.0
 * [cmake-shared](https://github.com/lirios/cmake-shared.git) >= 2.0.99
 * [fluid](https://github.com/lirios/fluid) >= 2.0.0
 * [qtaccountsservice](https://github.com/lirios/qtaccountsservice) >= 2.0.0
 * [qtgsettings](https://github.com/lirios/qtgsettings) >= 2.0.0
 * [qtshellintegration](https://github.com/lirios/qtshellintegration)
 * [libliri](https://github.com/lirios/libliri)
 * [aurora-scanner](https://github.com/lirios/aurora-scanner)
 * [aurora-compositor](https://github.com/lirios/aurora-compositor)
 * [pam](http://www.linux-pam.org/)
 * [polkit-qt-1](https://invent.kde.org/libraries/polkit-qt-1)
 * [solid](https://invent.kde.org/frameworks/solid)
 * [layer-shell-qt](https://invent.kde.org/plasma/layer-shell-qt) >= 5.91.0

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

 * `-DFEATURE_liri_development_build:BOOL=ON`: Enable feature for development such as the
   ability to load arbitrary QML files.
 * `-DFEATURE_enable_systemd:BOOL=OFF`: Disable systemd support.
 * `-DFEATURE_shell_enable_qtquick_compiler:BOOL=ON`: Build with QtQuick compiler.
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
   * **liri.screensaver:** Lock, idle and inhibit interface
   * **liri.session:** Manages the session
   * **liri.loginmanager:** login manager subsystem
   * **liri.loginmanager.logind:** login manager subsystem (logind backend)
   * **liri.shell:** Shell
   * **liri.gtkshell:** Gtk shell support
   * **liri.outputmanagement:** Output management
   * **liri.decoration:** Window decorations
   * **liri.shell.liricolorpickerv1:** LiriColorPickerV1 Wayland protocol
   * **liri.shell.lirilockscreenv1:** LiriLockScreenV1 Wayland protocol
   * **liri.shell.lirimodalv1:** LiriModalV1 Wayland protocol
   * **liri.shell.lirishellv1:** LiriShellV1 Wayland protocol

 * Launcher QML plugin:
   * **liri.launcher:** Launcher model and items
   * **liri.launcher.appsmodel:** Applications model

 * MPRIS2 QML plugin:
   * **liri.mpris2:** MPRIS2 engine
   * **liri.mpris2.player:** MPRIS2 player interface

 * PolicyKit QML plugin:
   * **liri.policykit:** PolicyKit agent

## Fake screen configuration

You can simulate a fake screen configuration on X11 and Wayland.

Run:

```sh
liri-session -- -platform xcb --fake-screen screenconfig.json # replace xcb with wayland if you are running on Wayland
```

Here's the contents of `screenconfig.json`:

```json
{
	"outputs": [{
		"name": "Screen 1",
		"description": "Virtual output 1",
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
		"description": "Virtual output 2",
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
liri-session -- -qmljsdebugger=port:3768
```

In the example above we are using the default port which is 3768.
Now from Qt Creator click on Debug -> Start Debugging -> Attach to QML port and specify
the 3768 port.

See the [Qt Creator manual](http://doc.qt.io/qtcreator/creator-debugging-qml.html) for more information.
