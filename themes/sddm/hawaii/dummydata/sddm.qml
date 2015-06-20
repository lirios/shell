import QtQuick 2.0

QtObject {
    property bool canPowerOff: true
    property bool canReboot: true
    property bool canSuspend: true
    property bool canHibernate: true
    property bool canHybridSleep: true
    property string hostname: "MyHostname"

    signal loginFailed()
    signal loginSucceeded()

    function powerOff() {
        console.log("SDDM - POWERING OFF");
    }
    function reboot() {
        console.log("SDDM - REBOOTING");
    }
    function suspend() {
        console.log("SDDM - SUSPEND");
    }
    function hibernate() {
        console.log("SDDM - HIBERNATE");
    }
    function hybridSleep() {
        console.log("SDDM - HYBRID SLEEP");
    }

    function login(user, password, sessionIndex) {
        console.log("SDDM - logging into session index " + sessionIndex + " as [" + user + "] [" + password + "]");

        // Login
        if (user == "plfiorini" && password == "password")
            loginSucceeded();
        else
            loginFailed();
    }
}
