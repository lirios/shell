Hawaii Shell
============

[![GitHub release](https://img.shields.io/github/release/hawaii-desktop/hawaii-shell.svg)](https://github.com/hawaii-desktop/hawaii-shell)
[![GitHub issues](https://img.shields.io/github/issues/hawaii-desktop/hawaii-shell.svg)](https://github.com/hawaii-desktop/hawaii-shell/issues)

This is the Hawaii desktop environment shell.

It contains a Qt platform theme plugin, shells for different form
factors such as desktop, netbook and tablet and QML plugins.

Only the desktop shell is implemented at the moment though.

## Dependencies

Compiler requirements:

* [gcc >= 4.8](https://gcc.gnu.org/gcc-4.8/) or
* [Clang](http://clang.llvm.org/)

Qt >= 5.4 with at least the following modules is required:

* [qtbase](http://code.qt.io/cgit/qt/qtbase.git)
* [qtdeclarative](http://code.qt.io/cgit/qt/qtdeclarative.git)
* [qtgraphicaleffects](http://code.qt.io/cgit/qt/qtgraphicaleffects.git)
* [qtquickcontrols](http://code.qt.io/cgit/qt/qtquickcontrols.git)
* [qtsvg](http://code.qt.io/cgit/qt/qtsvg.git)

You may also need [Weston >= 1.5.0](http://wayland.freedesktop.org) for nested mode
to work. If you have Qt >= 5.5 you won't likely need nested mode.

The following modules and their dependencies are required:

* [ECM >= 1.4.0](http://quickgit.kde.org/?p=extra-cmake-modules.git)
* [greenisland](https://github.com/greenisland/greenisland)
* [kconfig](http://quickgit.kde.org/?p=kconfig.git)
* [kcoreaddons](http://quickgit.kde.org/?p=kcoreaddons.git)
* [solid](http://quickgit.kde.org/?p=solid.git)
* [pam](http://www.linux-pam.org/)
* [libqtxdg](https://github.com/lxde/libqtxdg)

Optional dependencies:

* Volume control
  * [alsa](http://www.alsa-project.org/main/index.php/Main_Page) and/or
  * [libpulse](https://wiki.freedesktop.org/www/Software/PulseAudio/)
* Networking
  * Network Manager support
    * [networkmanager-qt](http://quickgit.kde.org/?p=networkmanager-qt.git)
    * [modemmanager-qt](http://quickgit.kde.org/?p=modemmanager-qt.git) (optional)

Optional, but recommended dependencies:

* [hawaii-icon-themes](https://github.com/hawaii-desktop/hawaii-icon-themes) for the default icon theme
* [hawaii-wallpapers](https://github.com/hawaii-desktop/hawaii-wallpapers) for the default wallpaper

If you do not install those dependencies, please configure Hawaii with an
alternative icon theme and wallpaper.

## Dependency on KDE Frameworks

We use a few modules from KDE to decrease technology debt.

We used more in the past but they were dropped because they either
pull in too many dependencies or their performance are not acceptable.

A lot has been done to reduce dependencies from KDE, one notable example
is KService which is really slow and I/O intensive.  It was replaced by
libqtxdg for the applications menu with good results.

Chances are that the dependency on kcoreaddons will go away as well.

## Build

Building Hawaii Shell is a piece of cake.

Assuming you are in the source directory, just create a build directory
and run cmake:

```sh
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=/opt/hawaii ..
```

To do a debug build the last command can be:

```sh
cmake -DCMAKE_INSTALL_PREFIX=/opt/hawaii -DCMAKE_BUILD_TYPE=Debug ..
```

To do a release build instead it can be:

```sh
cmake -DCMAKE_INSTALL_PREFIX=/opt/hawaii -DCMAKE_BUILD_TYPE=Release ..
```

If not passed, the `CMAKE_INSTALL_PREFIX` parameter defaults to /usr/local.
You have to specify a path that fits your needs, /opt/hawaii is just an example.

Package maintainers would pass `-DCMAKE_INSTALL_PREFIX=/usr`.

The `CMAKE_BUILD_TYPE` parameter allows the following values:

* **Debug:** debug build
* **Release:** release build
* **RelWithDebInfo:** release build with debugging information

## Installation

It's really easy, it's just a matter of typing:

```sh
make install
```

from the build directory.

# Notes

## Logging categories

Qt 5.2 introduced logging categories and Hawaii takes advantage of
them to make debugging easier.

Please refer to the [Qt](http://doc.qt.io/qt-5/qloggingcategory.html) documentation
to learn how to enable them.

### Available categories

* Session manager:
  * **hawaii.session.processcontroller:** Starts the compositor
  * **hawaii.session.manager:** Manages the session
  * **hawaii.session.socket:** Session manager/Compositor communication
  * **hawaii.session.launcher:** Process launcher and application tracker
  * **hawaii.session.screensaver:** Lock, idle and inhibit interface
  * **hawaii.session.loginmanager:** login manager subsystem
  * **hawaii.session.loginmanager.logind:** login manager subsystem (logind backend)

* Mixer QML plugin:
  * **hawaii.qml.mixer:** Mixer service
  * **hawaii.qml.mixer.alsa:** Alsa backend
  * **hawaii.qml.mixer.pulseaudio:** PulseAudio backend

* Launcher QML plugin:
  * **hawaii.qml.launcher:**" Launcher model and items

* MPRIS2 QML plugin:
  * **hawaii.qml.mpris2:** MPRIS2 engine
  * **hawaii.qml.mpris2.player:** MPRIS2 player interface

* Notifications QML plugin:
  * **hawaii.qml.notifications:** Notifications service.

* Network QML plugin:
  * **hawaii.qml.networkmanager:** NetworkManager support.

* Hardware QML plugin:
  * **hawaii.qml.hardware:** Hardware service.
  * **hawaii.qml.hardware.battery:** Battery information.
  * **hawaii.qml.hardware.storagedevice:** Storage device information.

* Settings QML plugin:
  * **hawaii.qml.settings:** Settings.

## Components

*hawaii*

Compositor executable that links to Green Island.

*hawaii-session*

Manages the session, drives the compositor, runs autostart programs
and launches applications for the application launcher.

Autostarts the D-Bus session if needed and can logout an existing
session with:

```sh
hawaii-session --logout
```

Supports the following modes:

* **eglfs:** runs the compositor directly on KMS or other supported systems
* **hwcomposer:** runs the compositor directly with Android drivers
* **nested:** runs the compositor inside Weston

For **eglfs** mode the user must be in the ``video`` and ``input`` groups.
KMS support requires Qt 5.5 or better.
libinput is automatically used with Qt 5.5 or better, built with libinput support.

For **hwcomposer** mode the user must be in the ``video`` and ``input`` groups.
libinput is automatically used with Qt 5.5 or better, built with libinput support.

The mode can be specified with the ``--mode`` argument, here's an example:

```sh
hawaii-session --mode=eglfs
```

The best mode is automatically detected if you run ``hawaii-session``
without the ``--mode`` argument.

## QML JavaScript debugger

Developers can debug Hawaii Shell with Qt Creator and the QML JavaScript debugger.

Run Hawaii setting the debugger port:

```sh
HAWAII_SHELL_DEBUG_PORT=3768 hawaii
```

In the example above we are using the default port which is 3768.
Now from Qt Creator click on Debug -> Start Debugging -> Attach to QML port and specify
the 3768 port.

See the [Qt Creator manual](http://qt-project.org/doc/qtcreator-3.0/creator-debugging-qml.html) for more information.
