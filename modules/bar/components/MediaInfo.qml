pragma ComponentBehavior: Bound

import qs.services
import qs.config
import qs.components
import qs.utils
import Quickshell
import Quickshell.Widgets
import QtQuick
import QtQuick.Effects

Loader {
    id: root

    readonly property bool isMediaActive: MprisService.currentActive !== null
    readonly property DesktopEntry mediaDesktopEntry: isMediaActive ? DesktopEntries.heuristicLookup(MprisService.currentActive?.desktopEntry) : null
    readonly property string mediaIcon: mediaDesktopEntry?.icon ?? null
    readonly property int padding: Config.appearance.padding.sm
    readonly property bool shouldBeActive: MprisService.playerList.length > 0

    active: Config.media.enabled

    sourceComponent: Item {
        id: content

        readonly property int animDuration: 400
        readonly property int initialWidth: Config.bar.sizes.mediaInfoWidth

        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter

        implicitWidth: 0
        implicitHeight: Config.bar.sizes.innerHeight
        opacity: 0
        visible: width > 0

        clip: true

        states: State {
            name: "visible"
            when: root.shouldBeActive === true
        }

        transitions: [
            Transition {
                from: ""
                to: "visible"

                ParallelAnimation {
                    NAnim {
                        target: content
                        property: "implicitWidth"
                        to: content.initialWidth
                        duration: content.animDuration
                    }
                    NAnim {
                        target: content
                        property: "opacity"
                        to: 1
                        duration: content.animDuration
                        easing.bezierCurve: Config.appearance.animCurves.linear
                    }
                }
            },
            Transition {
                from: "visible"
                to: ""

                ParallelAnimation {
                    NAnim {
                        target: content
                        property: "implicitWidth"
                        to: 0
                        duration: content.animDuration
                    }
                    NAnim {
                        target: content
                        property: "opacity"
                        to: 0
                        duration: content.animDuration
                        easing.bezierCurve: Config.appearance.animCurves.linear
                    }
                }
            }
        ]

        MouseArea {
            id: rootInteractionArea

            anchors.fill: parent

            cursorShape: Qt.PointingHandCursor

            hoverEnabled: true
        }

        StyledRect {
            id: bgRect

            anchors.fill: parent
            radius: Config.appearance.rounding.sm

            color: ColorService.current.base0
        }

        Loader {
            id: iconLoader

            property string iconBuffer: ""

            active: iconBuffer !== ""

            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            anchors.leftMargin: root.padding

            Component.onCompleted: {
                updateIconBuffer();
            }

            Connections {
                target: root

                function onMediaIconChanged() {
                    iconLoader.updateIconBuffer();
                }

                function onIsMediaActiveChanged() {
                    iconLoader.updateIconBuffer();
                }

                function onShouldBeActiveChanged() {
                    iconLoader.updateIconBuffer();
                }
            }

            function updateIconBuffer() {
                if (root.shouldBeActive == false)
                    return;

                iconLoader.iconBuffer = Quickshell.iconPath(root.mediaIcon, true);
            }

            sourceComponent: IconImage {
                anchors.left: parent.left

                source: iconLoader.iconBuffer
                implicitSize: Config.bar.sizes.innerHeight * 0.75
                asynchronous: true
            }
        }

        Item {
            id: scrollingWrapper

            property int dynamicDuration: 5000

            readonly property bool shouldAnimate: textReference.width >= width
            readonly property int scrollRange: Math.max(textReference.width, scrollingWrapper.width) + Config.appearance.spacing.xl

            anchors.top: parent.top
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.left: iconLoader.right

            anchors.leftMargin: root.padding
            anchors.rightMargin: root.padding

            clip: true

            function fetchTrackData() {
                trackData.trackTitle = MprisService.currentActive?.trackTitle ?? "Unknown Title";
                trackData.trackArtist = MprisService.currentActive?.trackArtist ?? "Unknown Artist";
            }

            Component.onCompleted: {
                scrollingWrapper.fetchTrackData();
            }

            QtObject {
                id: trackData

                property string trackTitle: MprisService.currentActive?.trackTitle ?? "Unknown Title"
                property string trackArtist: MprisService.currentActive?.trackArtist ?? "Unknown Artist"
            }

            Connections {
                target: textReference

                function onTextChanged() {
                    scrollingWrapper.dynamicDuration = Math.max(textReference.width, scrollingWrapper.width) / 32 * 1000;
                    trackTitleText.x = 0;
                    if (scrollingWrapper.shouldAnimate)
                        scrollAnim.restart();
                }
            }

            Connections {
                target: MprisService

                function onTrackChanged() {
                    scrollingWrapper.fetchTrackData();
                }

                function onPlaybackStateChanged() {
                    scrollingWrapper.fetchTrackData();
                }
            }

            TextMetrics {
                id: textReference

                readonly property string currentTrackTitle: trackData.trackTitle
                readonly property string currentTrackArtist: trackData.trackArtist
                readonly property string displayText: `${currentTrackTitle} - ${currentTrackArtist}`
                property string bufferedText: "No media"

                text: displayText

                font.family: Config.appearance.fontFamily.sans
                font.pointSize: Config.appearance.fontSize.xs
            }

            StyledText {
                id: trackTitleText

                anchors.verticalCenter: parent.verticalCenter

                text: textReference.text

                font.family: Config.appearance.fontFamily.sans
                font.pointSize: Config.appearance.fontSize.xs
            }

            StyledText {
                id: trackTitleTextGhost

                readonly property int initialPosition: Math.max(scrollingWrapper.width, textReference.width)

                anchors.left: trackTitleText.right
                anchors.leftMargin: Math.max(scrollingWrapper.scrollRange - textReference.width, 0)
                anchors.verticalCenter: parent.verticalCenter

                text: textReference.text
                visible: scrollingWrapper.shouldAnimate

                font.family: Config.appearance.fontFamily.sans
                font.pointSize: Config.appearance.fontSize.xs
            }

            SequentialAnimation {
                id: scrollAnim
                running: scrollingWrapper.shouldAnimate
                loops: Animation.Infinite

                PropertyAction {
                    target: trackTitleText
                    property: "x"
                    value: 0
                }

                PauseAnimation {
                    duration: 4000
                }

                ParallelAnimation {
                    NumberAnimation {
                        target: trackTitleText

                        property: "x"

                        from: 0
                        to: -scrollingWrapper.scrollRange
                        duration: scrollingWrapper.dynamicDuration
                    }
                }
            }
        }
    }
}
