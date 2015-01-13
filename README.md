Hawaii Shell
============

This is the Hawaii desktop environment shell.

It contains a Qt platform theme plugin, shells for different form
factors such as desktop, netbook and tablet and QML plugins.

Only the desktop shell is implemented at the moment though.

## Dependencies

You need the following modules and their dependencies:

* weston (from http://wayland.freedesktop.org)
* greenisland (from https://github.com/greenisland)

Make sure you also have Qt 5.4 or better with the following modules:

* qtbase
* qtdeclarative
* qtgraphicaleffects
* qtquickcontrols
* qtsvg

And the following modules from KDE:

* kconfig
* kcoreaddons
* kio
* kservice
* solid
* plasma-framework
* plasma-workspace
* plasma-nm

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
