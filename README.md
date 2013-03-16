Hawaii Shell
============

This is the Hawaii desktop environment shell.

It contains a Qt platform plugin and shells for different form
factors such as desktop, netbook and tablet.

Only the desktop shell is implemented at the moment though.

## Dependencies

In order to build and install this software, you will need a complete
and up to date Wayland, Qt 5 and Vibe development environment.

The Wayland site has some information to bring it up:

  http://wayland.freedesktop.org/building.html

More information about building Qt 5 can be found here:

  http://qt-project.org/wiki/Building-Qt-5-from-Git

Vibe and other Hawaii components can be easily built with our
Continuous Integration tool, read the instructions here:

  https://github.com/hawaii-desktop/hawaii

The Continuous Integration tool builds the whole desktop.

## Build

Building this software is a piece of cake.

Assuming you are in the source directory, just create a build directory
and run cmake:

    mkdir build
    cd build
    cmake -DCMAKE_INSTALL_PREFIX=/opt/hawaii ..

To do a debug build the last command can be:

    cmake -DCMAKE_INSTALL_PREFIX=/opt/hawaii -DCMAKE_BUILD_TYPE=Debug ..

To do a release build instead it can be:

    cmake -DCMAKE_INSTALL_PREFIX=/opt/hawaii -DCMAKE_BUILD_TYPE=Release ..

If not passed, the CMAKE_INSTALL_PREFIX parameter defaults to /usr/local.
You have to specify a path that fits your needs, /opt/hawaii is just an example.

Package maintainers would pass *-DCMAKE_INSTALL_PREFIX=/usr*.

The CMAKE_BUILD_TYPE parameter allows the following values:

    Debug: debug build
    Release: release build
    RelWithDebInfo: release build with debugging information

## Installation

It's really easy, it's just a matter of typing:

    make install

from the build directory.
