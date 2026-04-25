pragma Singleton

import Quickshell
import Quickshell.Io
import QtQuick

Singleton {
    id: root

    property string userName: {
        if (internal.userName === null) {
            tryFetchUsername();
            return "";
        } else {
            return internal.userName ?? "";
        }
    }

    function tryFetchUsername(): void {
        if (fetchUserNameProcess.running)
            return;
        if (internal.userName === null) {
            fetchUserNameProcess.running = true;
        }
    }

    QtObject {
        id: internal

        property var userName: null
    }

    Process {
        id: fetchUserNameProcess
        command: ["sh", "-c", "echo $USER"]
        stdout: StdioCollector {
            onStreamFinished: {
                internal.userName = this.text;
                console.log(this.text);
            }
        }
    }
}
