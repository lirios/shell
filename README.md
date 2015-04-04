Hawaii Shell
============

This is the Hawaii desktop environment shell.

It contains a Qt platform theme plugin, shells for different form
factors such as desktop, netbook and tablet and QML plugins.

Only the desktop shell is implemented at the moment though.

## Dependencies

gcc >= 4.8 is required to build this software.

You need the following modules and their dependencies:

* [greenisland](https://github.com/greenisland/greenisland)

Optional, but recommended dependencies:

* [hawaii-icon-themes](https://github.com/hawaii-desktop/hawaii-icon-themes) (for the default icon theme)
* [hawaii-wallpapers](https://github.com/hawaii-desktop/hawaii-wallpapers) (for the default wallpaper)

If you do not install those dependencies, please configure Hawaii with an
alternative icon theme and wallpaper.

You may also need [Weston](http://wayland.freedesktop.org) for nested mode
to work.

Make sure you also have Qt 5.4 or better with the following modules:

* qtbase
* qtdeclarative
* qtgraphicaleffects
* qtquickcontrols
* qtsvg

And the following modules from KDE:

* kconfig
* kcoreaddons
* solid

These are runtime dependencies from KDE:

* plasma-workspace
* plasma-nm

Other dependencies:

* pam
* libqtxdg

Optional dependencies:

* alsa and/or libpulse for volume control

## Notes for packagers

We use modules from KDE to decrease technology debt.
However some of those modules pull in too many dependencies.

In particular we currently need:

* Networking (plasma-nm, QML plugin only not plasmoid)

More actions will be taken in the future to cut down those dependencies.

KService is really slow and was replaced by libqtxdg for the applications menu.
For networking we need plasma-nm and plasma-workspace as runtime dependencies.

Tier 1 frameworks on the other hand are usually safe.

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

* Hardware QML plugin:
  * **hawaii.qml.hardware:** Hardware service.
  * **hawaii.qml.hardware.battery:** Battery information.
  * **hawaii.qml.hardware.storagedevice:** Storage device information.

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

Supports a couple of modes:

* **eglfs:** runs the compositor directly on KMS
* **hwcomposer:** runs the compositor directly with Android drivers
* **nested:** runs the compositor inside Weston

The **eglfs** mode requires Qt 5.5 and your user must be in the video and input groups
or you should use the root account.

The **hwcomposer** mode requires root privileges only when using
the libinput input plugin with Qt 5.5.

The mode can be specified with the ``--mode`` argument, here's an example:

```sh
hawaii-session --mode=eglfs
```

The ``--mode`` argument can be avoid only on X11 where a specific mode
is not required.

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
