pragma ComponentBehavior: Bound

import qs.config
import qs.components
import qs.services
import Quickshell
import Quickshell.Widgets
import Quickshell.Services.Mpris
import QtQuick
import QtQuick.Layouts

ColumnLayout {
    id: root

    required property MprisPlayer modelData
    readonly property int padding: Config.appearance.padding.sm

    readonly property DesktopEntry desktopEntry: modelData !== null ? DesktopEntries.heuristicLookup(modelData?.desktopEntry) : null
    readonly property string mediaIcon: desktopEntry?.icon ?? null

    property bool iconLoaded: false

    spacing: 0

    Layout.fillWidth: true

    QtObject {
        id: trackData

        property string trackTitle: root.modelData?.trackTitle ?? "Unknown track"
        property string trackArtist: root.modelData?.trackArtist ?? "Unknown artist"
        property string trackArtUrl: root.modelData?.trackArtUrl ?? ""

        function getArtUrl() {
            trackArtUrl = MprisService.getArtUrl(root.modelData);
        }
    }

    Rectangle {
        id: titleWrapper

        Layout.fillWidth: true
        Layout.margins: root.padding

        implicitHeight: playerTitle.implicitHeight + root.padding * 2

        color: ColorService.current.base
        radius: Config.appearance.rounding.md

        MouseArea {
            anchors.fill: parent

            enabled: root.modelData?.canRaise ?? false

            cursorShape: Qt.PointingHandCursor

            onClicked: {
                if (!root.modelData.canRaise)
                    return;

                root.modelData.raise();
            }
        }

        RowLayout {
            id: playerTitle

            anchors.left: parent.left
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter

            spacing: Config.appearance.spacing.sm

            Loader {
                active: root.iconLoaded === false

                sourceComponent: StyledText {
                    id: playerIcon

                    text: "󰈣"

                    font.family: Config.appearance.fontFamily.mono
                    font.pixelSize: 18
                }
            }

            IconImage {
                source: Quickshell.iconPath(root.mediaIcon)
                implicitSize: 18
                asynchronous: true

                visible: status === Image.Ready

                onStatusChanged: {
                    if (status === Image.Error) {} else if (status === Image.Ready) {
                        root.iconLoaded = true;
                    }
                }
            }

            StyledText {
                id: mprisName

                Layout.fillWidth: true

                text: root.modelData?.identity ?? "Unknown player"
                elide: Text.ElideRight

                font.family: Config.appearance.fontFamily.sans
                font.pointSize: Config.appearance.fontSize.sm
                font.weight: 600
            }
        }
    }

    RowLayout {
        id: currentMediaInfo

        Layout.fillWidth: true
        Layout.margins: root.padding

        spacing: Config.appearance.spacing.xl

        Item {
            id: mediaArtWrapper

            implicitWidth: 96
            implicitHeight: 96

            Rectangle {
                id: mediaArtFallbackBg
                anchors.fill: parent
                color: ColorService.current.base
                radius: Config.appearance.rounding.md

                visible: mprisArtLoader.isImageValid === false || mprisArtLoader.isImageLoading === true

                StyledText {
                    id: mediaArtFallback

                    anchors.fill: parent

                    text: ""
                    font.family: Config.appearance.fontFamily.monoIcon
                    font.pixelSize: mediaArtWrapper.implicitWidth * 0.8

                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }

            Loader {
                id: mprisArtLoader

                property bool isImageValid: false
                property bool isImageLoading: true

                anchors.fill: parent

                active: trackData.trackArtUrl !== ""

                sourceComponent: Image {
                    anchors.fill: parent
                    source: trackData.trackArtUrl
                    sourceSize.width: width
                    sourceSize.height: height
                    asynchronous: true

                    onStatusChanged: {
                        if (status === Image.Error) {
                            mprisArtLoader.isImageValid = false;
                        } else if (status === Image.Loading) {
                            mprisArtLoader.isImageLoading = true;
                        } else if (status === Image.Ready) {
                            mprisArtLoader.isImageValid = true;
                            mprisArtLoader.isImageLoading = false;
                        }
                    }
                }
            }
        }

        ColumnLayout {
            id: mediaData

            Layout.fillWidth: true
            Layout.fillHeight: true

            spacing: Config.appearance.spacing.xxs

            StyledText {
                text: trackData.trackArtist
                font.family: Config.appearance.fontFamily.sans
                font.pointSize: Config.appearance.fontSize.xxs

                Layout.fillWidth: true
                elide: Text.ElideRight
            }

            StyledText {
                text: trackData.trackTitle
                font.family: twoLineTextSample.font.family
                font.pointSize: twoLineTextSample.font.pointSize
                font.weight: 600

                Layout.fillWidth: true
                wrapMode: Text.Wrap
                elide: Text.ElideRight

                // HACK: fake line height so the height is fixed at 2 lines
                // to prevent layout shifting
                Layout.preferredHeight: (twoLineTextSample.height + 2) * 2

                maximumLineCount: 2
            }

            TextMetrics {
                id: twoLineTextSample

                font.family: Config.appearance.fontFamily.sans
                font.pointSize: Config.appearance.fontSize.md
                font.weight: 600

                text: "\n"
            }

            Loader {
                id: progressBarLoader

                Layout.fillWidth: true
                Layout.topMargin: Config.appearance.spacing.xxs

                active: (root.modelData?.positionSupported && root.modelData?.lengthSupported) ?? false

                sourceComponent: Item {
                    id: progressBar

                    readonly property real trackLength: root.modelData?.length ?? 1
                    readonly property real trackPosition: root.modelData?.position ?? 0
                    readonly property real progressPercent: trackPosition / trackLength

                    anchors.left: parent.left
                    anchors.right: parent.right
                    implicitHeight: 10

                    Rectangle {
                        id: barBg

                        anchors.fill: parent
                        radius: Config.appearance.rounding.md
                        color: ColorService.current.base2
                    }

                    Rectangle {
                        id: barFg

                        anchors.left: parent.left

                        implicitHeight: parent.height
                        implicitWidth: progressBar.progressPercent * parent.width

                        radius: barBg.radius
                        color: ColorService.current.primary
                    }

                    FrameAnimation {
                        running: root.modelData?.isPlaying ?? false
                        onTriggered: {
                            MprisService.updateTrackPositions();
                        }
                    }
                }
            }

            RowLayout {
                id: mediaControls

                readonly property bool isPlaying: root.modelData?.isPlaying ?? false

                Layout.alignment: Qt.AlignHCenter
                Layout.topMargin: Config.appearance.spacing.xxs
                spacing: 0

                MediaControlButton {
                    id: goPrevious
                    isEnabled: root.modelData?.canGoPrevious ?? false

                    iconText: "󰒮"

                    function clickAction() {
                        root.modelData.previous();
                    }
                }

                MediaControlButton {
                    id: playPause
                    isEnabled: (root.modelData?.canPlay && root.modelData?.canPause) ?? false

                    iconText: mediaControls.isPlaying ? "󰏤" : "󰐊"

                    function clickAction() {
                        if (mediaControls.isPlaying) {
                            root.modelData.pause();
                        } else {
                            root.modelData.play();
                        }
                    }
                }

                MediaControlButton {
                    id: goNext
                    isEnabled: root.modelData?.canGoNext ?? false

                    iconText: "󰒭"

                    function clickAction() {
                        root.modelData.next();
                    }
                }

                MediaControlButton {
                    id: shuffleButton
                    isEnabled: (root.modelData?.canControl && root.modelData?.shuffleSupported) ?? false
                    isFaded: isEnabled && root.modelData.shuffle !== true

                    visible: isEnabled

                    iconText: "󰒟"

                    function clickAction() {
                        root.modelData.shuffle = !root.modelData.shuffle;
                    }
                }
            }
        }
    }
}
