Hawaii Shell
============

This is the Hawaii desktop environment shell.

It contains a Qt platform theme plugin, shells for different form
factors such as desktop, netbook and tablet and QML plugins.

Only the desktop shell is implemented at the moment though.

## Dependencies

In order to build and install Hawaii Shell you need Wayland 1.2 and Qt 5.2 or better with
at least the following modules:

* qtbase
* qtdeclarative
* qtwayland

At the time of this writing, the qtwayland module doesn't support Wayland 1.3
for the QtCompositor API.

The Wayland site has some information on how to build it:

  http://wayland.freedesktop.org/building.html

You can either build Qt from git yourself or download binaries.

More information about building Qt from git can be found here:

  http://qt-project.org/wiki/Building-Qt-5-from-Git

Qt 5 binaries can be downloaded from http://qt-project.org/downloads

The qtwayland module has not been released yet, please clone the `stable` branch from:

  http://qt.gitorious.org/qt/qtwayland

and build it yourself.

You also need the following modules:

* weston
* fluid
* qtaccountsservice
* qtconfiguration
* greenisland (optional; used by the QML compositor)

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
