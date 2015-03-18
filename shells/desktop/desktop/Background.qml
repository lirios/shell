import QtQuick 2.2
import Hawaii.Components 1.0 as Components
import Hawaii.Themes 1.0 as Themes
import org.hawaii.misc 0.1 as Misc
import org.hawaii.settings 0.1 as Settings

Loader {
    Settings.ConfigGroup {
        id: config
        file: "hawaii/shellrc"
        group: "Background"
        onConfigChanged: applySettings()

        function applySettings() {
            settings.mode = config.readEntry("Mode", "wallpaper");
            settings.primaryColor = config.readEntry("PrimaryColor", "#336699");
            settings.secondaryColor = config.readEntry("SecondaryColor", "#334455");
            settings.pictureUrl = config.readEntry("PictureUrl", Misc.StandardPaths.locateFile(Misc.StandardPaths.GenericDataLocation, "backgrounds/hawaii/Also_Calm.png"));
            settings.fillMode = config.readEntry("FillMode", Image.Stretch);

            switch (settings.mode) {
            case "solid":
                sourceComponent = solid;
                break;
            case "hgradient":
            case "vgradient":
                sourceComponent = gradient;
                break;
            case "wallpaper":
                sourceComponent = wallpaper;
                break;
            default:
                sourceComponent = null;
                break;
            }
        }
    }

    QtObject {
        id: settings

        property string mode
        property color primaryColor
        property color secondaryColor
        property url pictureUrl
        property int fillMode

        Behavior on primaryColor {
            ColorAnimation {
                easing.type: Easing.OutQuad
                duration: Themes.Units.mediumDuration
            }
        }

        Behavior on secondaryColor {
            ColorAnimation {
                easing.type: Easing.OutQuad
                duration: Themes.Units.mediumDuration
            }
        }

        Component.onCompleted: config.applySettings()
    }

    Component {
        id: solid

        Components.NoiseBackground {
            objectName: "solid"
            color: settings.primaryColor
        }
    }

    Component {
        id: gradient

        Components.NoiseBackground {
            property bool vertical: settings.mode == "vgradient"

            objectName: "gradient"
            gradient: Gradient {
                GradientStop {
                    position: 0
                    color: settings.primaryColor
                }
                GradientStop {
                    position: 1
                    color: settings.secondaryColor
                }
            }
            rotation: vertical ? 270 : 0
            scale: vertical ? 2 : 1
        }
    }

    Component {
        id: wallpaper

        Components.NoiseBackground {
            objectName: "wallpaper"
            color: settings.primaryColor

            Components.SmoothFadeImage {
                readonly property real aspectRatio: width / height

                anchors.fill: parent
                source: settings.pictureUrl
                sourceSize.width: aspectRatio * 1024
                sourceSize.height: 1024
                smooth: true
                clip: fillMode === Image.PreserveAspectCrop
                fillMode: settings.fillMode
            }
        }
    }
}
