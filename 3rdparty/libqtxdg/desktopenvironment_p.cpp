
#include <QtCore/QByteArray>

static inline QByteArray detectDesktopEnvironment()
{
    const QByteArray _desktop = qgetenv("XDG_CURRENT_DESKTOP");
    if (!_desktop.isEmpty()) {
        return _desktop.toUpper();
    }
    return QByteArray("UNKNOWN");
}
