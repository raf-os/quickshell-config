pragma Singleton

import qs.utils
import Quickshell
import Quickshell.Io
import QtQuick

Singleton {
    id: root

    property alias appearance: adapter.appearance
    property alias border: adapter.border
    property alias bar: adapter.bar

    property bool recentlySaved: false

    function save(): void {
        saveTimer.restart();
        recentlySaved = true;
        saveCooldown.restart();
    }

    ElapsedTimer {
        id: timer
    }

    Timer {
        id: saveTimer

        interval: 500

        onTriggered: {
            timer.restart();
            try {
                let config = {};

                try {
                    config = JSON.parse(fileView.text());
                } catch (e) {
                    config = {};
                }

                config = serializeConfig();
                fileView.setText(JSON.stringify(config, null, 2));
            } catch (e) {
                console.log("Failed to serialize config.");
            }
        }
    }

    Timer {
        id: saveCooldown

        interval: 2000
        onTriggered: {
            recentlySaved = false;
        }
    }

    function serializeConfig(): var {
        return {
            appearance: serializeAppearance()
        };
    }

    function serializeAppearance(): var {
        return {
            fontFamily: {
                sans: appearance.fontFamily.sans,
                mono: appearance.fontFamily.mono,
                material: appearance.fontFamily.material
            },
            fontSize: {
                scale: appearance.fontSize.scale
            },
            spacing: {
                scale: appearance.spacing.scale
            }
        };
    }

    FileView {
        id: fileView

        path: `${Paths.config}/shell.json`
        watchChanges: true
        onFileChanged: {
            if (!recentlySaved) {
                timer.restart();
                reload();
            } else {
                reload();
            }
        }

        onLoaded: {
            try {
                JSON.parse(text());
            } catch (e) {
                console.log("Failed to load configs.");
            }
        }

        onLoadFailed: err => {
            if (err !== FileViewError.FileNotFound)
                console.log("Error loading file:", FileViewError.toString(err));
        }

        onSaveFailed: err => {
            console.log("Failed to save config", FileViewError.toString(err));
        }

        JsonAdapter {
            id: adapter

            property AppearanceConfig appearance: AppearanceConfig {}
            property BorderConfig border: BorderConfig {}
            property BarConfig bar: BarConfig {}
        }
    }
}
