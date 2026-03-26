import qs.components
import Quickshell.Services.Pam
import QtQuick

Item {
    id: root

    required property Pam pam
    property string buffer

    implicitWidth: 420
    implicitHeight: 32

    Connections {
        target: root.pam

        function onBufferChanged(): void {
            root.buffer = root.pam.buffer;
        }
    }

    StyledText {
        id: placeholderText

        anchors.fill: parent

        text: {
            if (root.pam.passwordContext.active)
                return qsTr("Loading...");
            if (root.pam.pamResult === PamResult.MaxTries)
                return qsTr("Maximum amount of tries exceeded.");
            return qsTr("Enter password");
        }
    }
}
