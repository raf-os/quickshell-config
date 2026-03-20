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
    property alias launcher: adapter.launcher

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

                config = root.serializeConfig();
                fileView.setText(JSON.stringify(config, null, 2));
            } catch (e) {
                // console.log("Failed to serialize config.");
                Quickshell.execDetached(["notify-send", "Failed serializing config."]);
            }
        }
    }

    Timer {
        id: saveCooldown

        interval: 2000
        onTriggered: {
            root.recentlySaved = false;
        }
    }

    function serializeConfig(): var {
        return {
            appearance: serializeAppearance(),
            bar: serializeBar(),
            border: serializeBorder(),
            launcher: serializeLauncher()
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
            },
            padding: {
                scale: appearance.padding.scale
            }
        };
    }

    function serializeBar(): var {
        return {
            sizes: bar.sizes
        };
    }

    function serializeBorder(): var {
        return {
            thickness: border.thickness,
            rounding: border.rounding
        };
    }

    function serializeLauncher(): var {
        return {
            enabled: launcher.enabled,
            width: launcher.width,
            commandPrefix: launcher.commandPrefix,
            favoriteApps: launcher.favoriteApps
        };
    }

    FileView {
        id: fileView

        path: `${Paths.config}/shell.json`
        watchChanges: true
        onFileChanged: {
            if (!root.recentlySaved) {
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
                Quickshell.execDetached(["notify-send", "Failed loading shell configs."]);
            }
        }

        onLoadFailed: err => {
            if (err === FileViewError.FileNotFound) {
                console.log("No config file exists. Creating our own...");
                root.save();
                Quickshell.execDetached(["notify-send", qsTr(`Setting up initial shell configurations at ${Paths.config}/myshell`)]);
            } else {
                Quickshell.execDetached(["notify-send", qsTr("Failed loading shell configs.")]);
            }
        }

        onSaveFailed: err => {
            console.log("Failed to save config", FileViewError.toString(err));
        }

        JsonAdapter {
            id: adapter

            property AppearanceConfig appearance: AppearanceConfig {}
            property BorderConfig border: BorderConfig {}
            property BarConfig bar: BarConfig {}
            property LauncherConfig launcher: LauncherConfig {}
        }
    }
}
