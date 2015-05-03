Hawaii Shell
============

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

* Library:
  * **hawaii.qgsettings:** Qt GSettings wrapper

* Mixer QML plugin:
  * **hawaii.qml.mixer:** Mixer service
  * **hawaii.qml.mixer.alsa:** Alsa backend
  * **hawaii.qml.mixer.pulseaudio:** PulseAudio backend

* Launcher QML plugin:
  * **hawaii.qml.launcher:** Launcher model and items
  * **hawaii.qml.launcher.appsmodel:** Applications model

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
