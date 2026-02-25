pragma Singleton
pragma ComponentBehavior: Bound

import Quickshell
import Quickshell.Io
import QtQuick

Singleton {
    id: root

    property bool isConnected: false

    Process {
        id: monitorProcess

        running: true
        command: ["sh", "-c", "networkctl status -n 0 | grep \"Online state\""]

        stdout: SplitParser {
            onRead: data => {
                const lines = data.split(": ");
                root.isConnected = lines[1];
            }
        }
        onExited: connectionStatusTimer.start()
    }

    Timer {
        id: connectionStatusTimer
        interval: 10000
        onTriggered: {
            monitorProcess.running = true;
        }
    }
}
