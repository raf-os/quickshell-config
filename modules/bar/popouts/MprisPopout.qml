pragma ComponentBehavior: Bound

import "mpris"
import qs.config
import qs.components
import qs.services
import Quickshell
import Quickshell.Services.Mpris
import Quickshell.Widgets
import QtQuick
import QtQuick.Layouts

Item {
    id: root

    readonly property MprisPlayer currentActive: MprisService.currentActive
    readonly property list<MprisPlayer> playerList: MprisService.playerList.filter(mp => mp !== currentActive)

    implicitWidth: 320
    implicitHeight: layoutRoot.implicitHeight

    StackLayout {
        id: layoutRoot

        currentIndex: 0

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        MprisItem {
            modelData: root.currentActive
        }
    }

    component MprisItem: ColumnLayout {
        id: mpItem

        required property MprisPlayer modelData
        readonly property int padding: Config.appearance.padding.sm

        readonly property DesktopEntry desktopEntry: modelData !== null ? DesktopEntries.heuristicLookup(modelData?.desktopEntry) : null
        readonly property string mediaIcon: desktopEntry?.icon ?? null

        property bool iconLoaded: false

        Layout.fillWidth: true

        QtObject {
            id: trackData

            property string trackTitle: mpItem.modelData.trackTitle ?? "Unknown track"
            property string trackArtist: mpItem.modelData.trackArtist ?? "Unknown artist"
            property string trackArtUrl: mpItem.modelData.trackArtUrl ?? ""

            function getArtUrl() {
                trackArtUrl = MprisService.getArtUrl(mpItem.modelData);
            }
        }

        RowLayout {
            id: playerTitle

            Layout.fillWidth: true
            Layout.margins: mpItem.padding

            spacing: Config.appearance.spacing.sm

            Loader {
                active: mpItem.iconLoaded === false

                sourceComponent: StyledText {
                    id: playerIcon

                    text: "󰈣"

                    font.family: Config.appearance.fontFamily.mono
                    font.pixelSize: 18
                }
            }

            IconImage {
                source: Quickshell.iconPath(mpItem.mediaIcon)
                implicitSize: 18
                asynchronous: true

                visible: status === Image.Ready

                onStatusChanged: {
                    if (status === Image.Error) {} else if (status === Image.Ready) {
                        mpItem.iconLoaded = true;
                    }
                }
            }

            StyledText {
                id: mprisName

                Layout.fillWidth: true

                text: mpItem.modelData.identity

                font.family: Config.appearance.fontFamily.sans
                font.pointSize: Config.appearance.fontSize.sm
                font.weight: 600
            }
        }

        RowLayout {
            id: currentMediaInfo

            Layout.fillWidth: true
            Layout.margins: mpItem.padding

            spacing: Config.appearance.spacing.xl

            Item {
                id: mediaArtWrapper

                implicitWidth: 64
                implicitHeight: 64

                Loader {
                    id: mprisArtLoader

                    anchors.fill: parent

                    active: trackData.trackArtUrl !== ""

                    sourceComponent: Image {
                        anchors.fill: parent
                        source: trackData.trackArtUrl
                        sourceSize.width: width
                        sourceSize.height: height
                        asynchronous: true
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
                    font.family: Config.appearance.fontFamily.sans
                    font.pointSize: Config.appearance.fontSize.md
                    font.weight: 600

                    Layout.fillWidth: true
                    wrapMode: Text.Wrap

                    maximumLineCount: 3
                }

                Loader {
                    id: progressBarLoader

                    Layout.fillWidth: true
                    Layout.topMargin: Config.appearance.spacing.xxs

                    active: mpItem.modelData.positionSupported && mpItem.modelData.lengthSupported

                    sourceComponent: Item {
                        id: progressBar

                        readonly property real trackLength: mpItem.modelData.length ?? 1
                        readonly property real trackPosition: mpItem.modelData.position ?? 0
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
                            running: mpItem.modelData.isPlaying
                            onTriggered: {
                                MprisService.updateTrackPositions();
                            }
                        }
                    }
                }

                RowLayout {
                    id: mediaControls

                    readonly property bool isPlaying: mpItem.modelData.isPlaying

                    Layout.alignment: Qt.AlignHCenter
                    Layout.topMargin: Config.appearance.spacing.xxs
                    spacing: 0

                    MediaControlButton {
                        id: goPrevious
                        isEnabled: mpItem.modelData.canGoPrevious

                        iconText: "󰒮"

                        function clickAction() {
                            mpItem.modelData.previous();
                        }
                    }

                    MediaControlButton {
                        id: playPause
                        isEnabled: mpItem.modelData.canPlay && mpItem.modelData.canPause

                        iconText: mediaControls.isPlaying ? "󰏤" : "󰐊"

                        function clickAction() {
                            if (mediaControls.isPlaying) {
                                mpItem.modelData.pause();
                            } else {
                                mpItem.modelData.play();
                            }
                        }
                    }

                    MediaControlButton {
                        id: goNext
                        isEnabled: mpItem.modelData.canGoNext

                        iconText: "󰒭"

                        function clickAction() {
                            mpItem.modelData.next();
                        }
                    }
                }
            }
        }
    }
}
