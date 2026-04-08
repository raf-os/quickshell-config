pragma Singleton

import qs.config
import qs.utils
import MyShellPlugin
import Quickshell
import Quickshell.Io
import QtQuick

Singleton {
    id: root

    property string currentLayout
    property int currentIndex: 0
    property list<string> userLayouts: Config.keymap.layouts

    Component.onCompleted: {
        getLayoutProcess.running = true;
    }

    HyprExtras {
        configPath: `${Paths.home}/.config/hypr`
        keyboardLayoutHandler: kbLHandler

        Component.onCompleted: {
            debugParseInput();
        }
    }

    KeyboardLayoutHandler {
        id: kbLHandler

        // onLayoutsChanged: {
        //     debugPrintLayouts();
        // }
    }

    FileView {
        id: layoutWatcher
        path: Qt.resolvedUrl("/etc/X11/xorg.conf.d/00-keyboard.conf")
        watchChanges: true

        onFileChanged: {
            getLayoutProcess.running = true;
        }
    }

    Process {
        id: getLayoutProcess

        command: ["sh", "-c", "localectl status | grep 'X11 Layout' | awk '{print $3}'"]
        stdout: StdioCollector {
            onStreamFinished: {
                root.currentLayout = this.text;
            }
        }
    }
}
