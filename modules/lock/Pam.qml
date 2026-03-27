import Quickshell

import Quickshell.Wayland
import Quickshell.Services.Pam
import QtQuick

Scope {
    id: root

    required property WlSessionLock lock

    readonly property alias passwordContext: passwordContext
    readonly property alias maxTries: pamSettings.maxTries
    property string buffer
    property string lockMessage
    property var pamResult
    property int totalTries: 0

    signal flashMessage

    function handleKey(event: KeyEvent): void {
        if (pamResult === PamResult.MaxTries || passwordContext.active)
            return;

        if (event.key === Qt.Key_Enter || event.key === Qt.Key_Return) {
            passwordContext.start();
        } else if (event.key === Qt.Key_Backspace) {
            if (event.modifiers & Qt.ControlModifier) {
                buffer = "";
            } else {
                buffer = buffer.slice(0, -1);
            }
        } else if (" abcdefghijklmnopqrstuvwxyz1234567890`~!@#$%^&*()-_=+[{]}\\|;:'\",<.>/?".includes(event.text.toLowerCase())) {
            // Only allow unicode
            buffer += event.text;
        }
    }

    QtObject {
        id: pamSettings

        property int maxTries: 3
        property int failInterval: 900
        property int unlockTime: 600
    }

    Timer {
        id: msgReset

        interval: 5000
        onTriggered: {
            if (root.pamResult !== PamResult.MaxTries)
                root.pamResult = null;
        }
    }

    PamContext {
        id: passwordContext

        config: "pwd"
        configDirectory: `${Quickshell.shellDir}/assets/pam.d`

        onMessageChanged: {
            root.lockMessage = message;
        }

        onResponseRequiredChanged: {
            if (!responseRequired)
                return;

            respond(root.buffer);
            root.buffer = "";
        }

        onCompleted: res => {
            if (res === PamResult.Success)
                return root.lock.unlock();
            else if (res === PamResult.Failed)
                root.totalTries += 1;

            root.pamResult = res;
            root.flashMessage();
            msgReset.restart();
        }
    }

    Connections {
        target: root.lock

        function onSecureChanged(): void {
            if (root.lock.secure) {
                root.buffer = "";
                root.pamResult = null;
                root.lockMessage = "";
            }
        }
    }
}
