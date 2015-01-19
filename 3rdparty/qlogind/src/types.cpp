#include "types.h"

#include <QDBusArgument>
#include <QDBusMetaType>

void registerTypes() {
    qRegisterMetaType<NamedSeatPath>("NamedSeatPath");
    qDBusRegisterMetaType<NamedSeatPath>();

    qRegisterMetaType<NamedSeatPathList>("NamedSeatPathList");
    qDBusRegisterMetaType<NamedSeatPathList>();

    qRegisterMetaType<NamedSessionPath>("NamedSessionPath");
    qDBusRegisterMetaType<NamedSessionPath>();

    qRegisterMetaType<NamedSessionPathList>("NamedSessionPathList");
    qDBusRegisterMetaType<NamedSessionPathList>();

    qRegisterMetaType<SessionInfo>("SessionInfo");
    qDBusRegisterMetaType<SessionInfo>();

    qRegisterMetaType<SessionInfoList>("SessionInfoList");
    qDBusRegisterMetaType<SessionInfoList>();

    qRegisterMetaType<UserInfo>("UserInfo");
    qDBusRegisterMetaType<UserInfo>();

    qRegisterMetaType<UserInfoList>("UserInfoList");
    qDBusRegisterMetaType<UserInfoList>();

}
