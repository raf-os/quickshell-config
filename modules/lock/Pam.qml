import Quickshell

import Quickshell.Wayland
import Quickshell.Services.Pam
import QtQuick

Scope {
    id: root

    required property WlSessionLock lock

    property string buffer
    property string lockMessage
    property PamResult pamResult

    function handleKey(event: KeyEvent): void {
        if (pamResult === PamResult.MaxTries)
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

            root.pamResult = res;
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
