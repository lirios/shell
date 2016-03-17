Hawaii Shell
============

# Dependencies

Compiler requirements:

* [gcc >= 4.8](https://gcc.gnu.org/gcc-4.8/) or
* [Clang](http://clang.llvm.org/)

Qt >= 5.6 with at least the following modules is required:

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
* [pam](http://www.linux-pam.org/)
* [gio-2.0](https://git.gnome.org/browse/glib/)
* [libqtxdg](https://github.com/lxde/libqtxdg)
* [solid](http://quickgit.kde.org/?p=solid.git)

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
