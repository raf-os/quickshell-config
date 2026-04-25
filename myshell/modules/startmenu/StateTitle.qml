pragma ComponentBehavior: Bound

import qs.components
import qs.config
import QtQuick
import QtQuick.Layouts

Item {
    id: root

    required property string currentState
    property int padding: 10

    readonly property int fontPointsize: Config.appearance.fontSize.lg
    readonly property string fontFamilyName: Config.appearance.fontFamily.sans
    readonly property int fontWeightAmt: 600

    Layout.fillWidth: true
    implicitHeight: txtMetric.height + padding * 2

    clip: true

    WrappedLoader {
        stateName: "apps"
        stateLabel: "Apps"
    }

    WrappedLoader {
        stateName: "command"
        stateLabel: "Command"
    }

    component WrappedLoader: Loader {
        id: wrappedLoader
        required property string stateName
        required property string stateLabel

        property bool isActive
        property bool shouldBeActive: root.currentState === stateName

        visible: isActive
        active: isActive

        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: root.padding
        anchors.rightMargin: root.padding

        onShouldBeActiveChanged: {
            if (shouldBeActive) {
                exitAnimation.stop();
                enterAnimation.start();
            } else {
                enterAnimation.stop();
                exitAnimation.start();
            }
        }

        SequentialAnimation {
            id: enterAnimation

            PropertyAction {
                target: wrappedLoader
                property: "isActive"
                value: true
            }
            ParallelAnimation {
                NAnim {
                    target: wrappedLoader
                    property: "anchors.verticalCenterOffset"
                    from: -root.implicitHeight
                    to: 0
                }
                NumberAnimation {
                    target: wrappedLoader.item
                    property: "opacity"
                    from: 0
                    to: 1
                    duration: 400
                }
            }
        }

        SequentialAnimation {
            id: exitAnimation

            ParallelAnimation {
                NAnim {
                    target: wrappedLoader
                    property: "anchors.verticalCenterOffset"
                    from: 0
                    to: root.implicitHeight
                }
                NumberAnimation {
                    target: wrappedLoader.item
                    property: "opacity"
                    from: 1
                    to: 0
                    duration: 100
                }
            }
            PropertyAction {
                target: wrappedLoader
                property: "isActive"
                value: false
            }
        }

        sourceComponent: StyledText {
            id: stateTxt
            text: wrappedLoader.stateLabel

            opacity: wrappedLoader.isActive ? 1 : 0

            font.pointSize: root.fontPointsize
            font.family: root.fontFamilyName
            font.weight: root.fontWeightAmt
        }
    }

    TextMetrics {
        id: txtMetric
        text: "Lorem Ipsum Dolor Sit Amet"
        font.pointSize: root.fontPointsize
        font.family: root.fontFamilyName
    }
}
