import qs.components
import QtQuick
import QtQuick.Layouts

Item {
    id: root

    required property var lock

    anchors.centerIn: parent

    implicitWidth: 640
    implicitHeight: 320

    focus: true
    onActiveFocusChanged: {
        if (!activeFocus)
            forceActiveFocus();
    }

    Keys.onPressed: event => {
        if (root.lock.unlocking)
            return;

        root.lock.pam.handleKey(event);
    }

    ColumnLayout {
        anchors.fill: parent

        StyledText {
            text: "lock screen, yo"
        }

        InputBar {
            id: inputBar

            pam: root.lock.pam
        }

        StyledText {
            id: pamMessage

            Layout.fillWidth: true

            readonly property Pam pam: root.lock.pam

            readonly property string msg: {
                if (pam.lockMessage)
                    return pam.lockMessage;

                return "";
            }
        }
    }
}
