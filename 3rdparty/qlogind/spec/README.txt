# Original XML files are created with

paths may need adjusting to make sure these interfaces exist

$ qdbus --system org.freedesktop.login1 /org/freedesktop/login1 org.freedesktop.DBus.Introspectable.Introspect > org.freedesktop.login1.Manager.xml
$ qdbus --system org.freedesktop.login1 /org/freedesktop/login1/seat/seat0 org.freedesktop.DBus.Introspectable.Introspect > org.freedesktop.login1.Seat.xml
$ qdbus --system org.freedesktop.login1 /org/freedesktop/login1/session/c2 org.freedesktop.DBus.Introspectable.Introspect > org.freedesktop.login1.Session.xml
$ qdbus --system org.freedesktop.login1 /org/freedesktop/login1/user/_1000 org.freedesktop.DBus.Introspectable.Introspect > org.freedesktop.login1.User.xml

afterwards strip all the other interfaces from each object

Some changes are needed to:
 1) Add annotations for complex types
 2) Remove methods we won't ever use (Manager CreateSession is only used by the PAM module)
 3) avoid names that would be a C++ class (session has a property called "class", which is naturally a problem)


Both original and patched versions are kept in the source tree so that it should be easier to make a diff before updating