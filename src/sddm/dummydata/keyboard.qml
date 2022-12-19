import QtQuick 2.0

Item {
    property bool numLock: true
    property bool capsLock: false
    property var currentLayout: 0
    property var layouts: [
        {"shortName": "it", "longName": "Italiano"},
        {"shortName": "en", "longName": "English (US)"}
    ]
    property bool enabled: true
}
