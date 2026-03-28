pragma ComponentBehavior: Bound

import qs.components
import qs.config
import qs.services
import qs.utils
import QtQuick
import QtQuick.Layouts

Item {
    id: root

    property string type
    property bool isShowing: false

    readonly property int padding: Config.appearance.padding.xl

    anchors.bottomMargin: Config.appearance.spacing.lg

    implicitWidth: 420
    implicitHeight: Math.max(64, rowLayout.height + root.padding * 2)

    opacity: 0
    scale: 0.9

    visible: isShowing

    QtObject {
        id: osdProps

        readonly property list<string> validTypes: ["mute", "volumechange", "mprischange", "gamemode"]

        property string title: ""
        property string icon: ""
        property string subtitle: ""
        property string artUrl: ""

        property bool showVolumeBar: false
        property bool showSeekBar: false

        function resetProps(): void {
            title = "";
            subtitle = "";
            icon = "";
            artUrl = "";
            showVolumeBar = false;
            showSeekBar = false;
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
            case "mprischange":
                const currentPlayer = MprisService.currentActive;
                const trackTitle = currentPlayer?.trackTitle ?? "Unknown title";
                const trackArtist = currentPlayer?.trackArtist ?? "Unknown artist";

                title = trackTitle;
                subtitle = trackArtist;
                artUrl = MprisService.getArtUrl();

                if (currentPlayer?.canSeek && currentPlayer?.positionSupported && currentPlayer?.lengthSupported)
                    showSeekBar = true;
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
                root.isShowing = true;
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

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        anchors.margins: root.padding

        Item {
            id: iconDisplay
            implicitWidth: mprisArtWrapper.implicitWidth
            implicitHeight: mprisArtWrapper.implicitHeight

            Text {
                id: iconText
                anchors.fill: parent
                text: osdProps.icon

                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter

                color: ColorService.current.baseContent
                font.pixelSize: parent.width * 0.75
                font.family: Config.appearance.fontFamily.mono
            }

            Item {
                id: mprisArtWrapper
                implicitWidth: mprisArtLoader.active ? 64 : 48
                implicitHeight: implicitWidth

                anchors.centerIn: parent

                StyledText {
                    id: mprisArtFallback

                    anchors.fill: parent
                    visible: mprisArtLoader.item?.status === Image.Error

                    text: ""

                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter

                    font.family: Config.appearance.fontFamily.mono
                    font.pixelSize: parent.width * 0.75
                }

                Loader {
                    id: mprisArtLoader
                    active: osdProps.artUrl !== ""

                    anchors.fill: parent

                    sourceComponent: Image {
                        id: mprisCoverArtImage
                        anchors.fill: parent

                        source: osdProps.artUrl
                        sourceSize.width: width
                        sourceSize.height: height
                        asynchronous: true
                        fillMode: Image.PreserveAspectCrop
                    }
                }
            }
        }

        ColumnLayout {
            id: contentDisplay
            Layout.fillWidth: true
            Layout.leftMargin: Config.appearance.spacing.md
            Layout.alignment: Qt.AlignVCenter

            spacing: Config.appearance.spacing.xs

            Component.onCompleted: {}

            StyledText {
                id: contentTitle
                Layout.alignment: Qt.AlignLeft
                Layout.fillWidth: true
                text: osdProps.title

                elide: Text.ElideRight

                font.family: Config.appearance.fontFamily.sans
                font.pointSize: Config.appearance.fontSize.sm
                font.weight: 600
            }

            StyledText {
                id: contentSubtitle
                Layout.fillWidth: true
                text: osdProps.subtitle

                elide: Text.ElideRight

                visible: osdProps.subtitle !== ""
                font.family: Config.appearance.fontFamily.sans
                font.pointSize: Config.appearance.fontSize.xs
            }

            Loader {
                id: seekBarLoader

                Layout.fillWidth: true

                active: osdProps.showSeekBar
                visible: osdProps.showSeekBar

                sourceComponent: Item {
                    id: seekBarWrapper

                    property real trackLength: MprisService.currentActive.length ?? 1
                    property real trackPosition: 0
                    readonly property real progressPercent: trackPosition / trackLength

                    anchors.left: parent.left
                    anchors.right: parent.right

                    implicitHeight: 12

                    function getPlayerPosition() {
                        MprisService.updateTrackPositions();
                    }

                    FrameAnimation {
                        running: MprisService.currentActive?.isPlaying
                        onTriggered: {
                            seekBarWrapper.getPlayerPosition();
                        }
                    }

                    Connections {
                        target: MprisService.currentActive

                        function onPositionChanged() {
                            // NOTE: This is likely unnecessary, keep an eye out
                            seekBarWrapper.trackPosition = MprisService.currentActive?.position;
                        }
                    }

                    Rectangle {
                        id: seekBgRect

                        anchors.fill: parent

                        radius: Config.appearance.rounding.md

                        color: ColorService.current.base0
                    }

                    Rectangle {
                        id: seekFgRect

                        anchors.left: parent.left
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom

                        implicitWidth: parent.width * seekBarWrapper.progressPercent

                        color: ColorService.current.primary

                        radius: Config.appearance.rounding.md
                    }
                }
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
                        NAnim {
                            duration: 300
                        }
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
            easing.bezierCurve: Config.appearance.animCurves.linear
            duration: 200
        }

        NAnim {
            target: root
            property: "scale"
            to: 1
            duration: 300
        }
    }

    SequentialAnimation {
        id: exitAnim

        ParallelAnimation {
            NAnim {
                target: root
                property: "opacity"
                to: 0
                easing.bezierCurve: Config.appearance.animCurves.linear
                duration: 200
            }

            NAnim {
                target: root
                property: "scale"
                to: 0.9
                duration: 300
            }
        }

        ScriptAction {
            script: root.isShowing = false
        }
    }
}
