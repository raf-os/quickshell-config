import qs.components
import qs.config
import qs.services
import qs.utils
import QtQuick
import QtQuick.Layouts

Item {
    id: root

    property string type

    anchors.bottomMargin: Config.appearance.spacing.lg

    implicitWidth: 320
    implicitHeight: 64

    opacity: 0
    scale: 0.8

    QtObject {
        id: osdProps

        readonly property list<string> validTypes: ["mute", "volumechange", "gamemode"]

        property string title: ""
        property string icon: ""
        property string subtitle: ""

        property bool showVolumeBar: false

        function resetProps(): void {
            title = "";
            subtitle = "";
            icon = "";
            showVolumeBar = false;
        }

        function applyProps(type: string): bool {
            if (!validTypes.includes(type)) {
                return false;
            }

            if (type !== root.type)
                resetProps();

            switch (type) {
            case "mute":
                title = "Muted";
                icon = "";
                break;
            case "volumechange":
                const currentVolume = AudioService.volume;

                title = "Volume";

                if (currentVolume > 0.5) {
                    icon = "";
                } else if (currentVolume > 0) {
                    icon = "";
                } else {
                    icon = "";
                }

                showVolumeBar = true;
                break;
            case "gamemode":
                const gameMode = GlobalStateManager.isGameMode;

                icon = gameMode ? "󰊴" : "󰊵";
                title = "Game mode";
                subtitle = gameMode ? "Turned on" : "Turned off";
                break;
            }

            return true;
        }
    }

    function show(type: string) {
        if (!type || type === "") {
            triggerExitAnim();
            return;
        } else {
            if (osdProps.applyProps(type)) {
                root.type = type;
                triggerEnterAnim();
            }
        }
    }

    function hide() {
        triggerExitAnim();
    }

    function triggerEnterAnim() {
        exitAnim.stop();
        enterAnim.start();
    }

    function triggerExitAnim() {
        enterAnim.stop();
        exitAnim.start();
    }

    Rectangle {
        anchors.fill: parent

        color: ColorService.current.base
        radius: Config.appearance.rounding.md

        opacity: 0.9

        border.width: 1
        border.color: ColorService.current.base3
    }

    RowLayout {
        id: rowLayout

        anchors.fill: parent
        anchors.margins: Config.appearance.padding.xl

        Item {
            id: iconDisplay
            Layout.fillHeight: true
            implicitWidth: 48

            Text {
                id: iconText
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                text: osdProps.icon

                color: ColorService.current.baseContent
                font.pixelSize: 48
                font.family: Config.appearance.fontFamily.monoIcon
            }
        }

        ColumnLayout {
            id: contentDisplay
            Layout.fillWidth: true
            Layout.fillHeight: true

            StyledText {
                id: contentTitle
                Layout.alignment: Qt.AlignLeft
                Layout.fillWidth: true
                text: osdProps.title

                font.family: Config.appearance.fontFamily.sans
                font.pointSize: Config.appearance.fontSize.sm
                font.weight: 600
            }

            StyledText {
                id: contentSubtitle
                Layout.fillWidth: true
                text: osdProps.subtitle

                visible: osdProps.subtitle !== ""
                font.family: Config.appearance.fontFamily.sans
                font.pointSize: Config.appearance.fontSize.xs
            }

            Item {
                id: contentVolumeBar

                Layout.fillWidth: true
                visible: osdProps.showVolumeBar === true

                implicitHeight: 12

                Rectangle {
                    id: bgVolumeRect

                    anchors.fill: parent

                    color: ColorService.current.base0
                    radius: Config.appearance.rounding.md
                }

                Rectangle {
                    id: fgVolumeRect

                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom

                    implicitWidth: Math.max(0, Math.min(1, AudioService.volume)) * parent.width

                    color: ColorService.current.primary
                    radius: Config.appearance.rounding.md

                    Behavior on implicitWidth {
                        NAnim {}
                    }
                }

                Item {
                    id: volSliderThumb
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    anchors.left: fgVolumeRect.right

                    Rectangle {
                        id: volSliderThumbRect
                        anchors.centerIn: parent

                        implicitWidth: 28
                        implicitHeight: 28

                        radius: 1000

                        color: ColorService.current.primary

                        border.width: 4
                        border.color: ColorService.current.base
                    }

                    Text {
                        id: volSliderThumbText
                        anchors.centerIn: parent
                        text: Math.round(AudioService.volume * 100) ?? 0

                        color: ColorService.current.baseContent
                        font.weight: 600
                        font.pointSize: Config.appearance.fontSize.xxs
                        font.family: Config.appearance.fontFamily.sans

                        width: volSliderThumbRect.implicitWidth
                        height: volSliderThumbRect.implicitHeight

                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }
            }
        }
    }

    ParallelAnimation {
        id: enterAnim

        NAnim {
            target: root
            property: "opacity"
            to: 1
        }

        NAnim {
            target: root
            property: "scale"
            to: 1
        }
    }

    ParallelAnimation {
        id: exitAnim

        NAnim {
            target: root
            property: "opacity"
            to: 0
        }
        NAnim {
            target: root
            property: "scale"
            to: 0.9
        }
    }
}
