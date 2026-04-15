pragma Singleton
pragma ComponentBehavior: Bound

import qs.config
import qs.utils
import Quickshell
import Quickshell.Io
import QtQuick

Singleton {
    id: root

    property ThemeData themeData: ThemeData {}
    property ColorPalette current: themeData.colors

    /**
     * I'm sure this is totally not unsafe...
    */
    function loadFromJson(data: string) {
        const jData = JSON.parse(data);
        for (const [key, value] of Object.entries(defaults.data)) {
            if (typeof value !== "string")
                continue;

            if (jData.hasOwnProperty(key)) {
                themeData[key] = jData[key];
            } else {
                themeData[key] = value;
            }
        }

        if (jData.hasOwnProperty("colors")) {
            for (const [name, color] of Object.entries(defaults.data.colors)) {
                if (typeof value === "function" || name === "objectName")
                    continue;

                if (jData.colors.hasOwnProperty(name)) {
                    themeData.colors[name] = jData.colors[name];
                } else {
                    themeData.colors[name] = color;
                }
            }
        }
    }

    QtObject {
        id: defaults

        readonly property ThemeData data: ThemeData {}
    }

    FileView {
        id: fView

        path: `${Paths.state}/config/theme.json`

        onLoaded: {
            try {
                root.loadFromJson(text());
            } catch (e) {
                console.warn("ColorService: failed parsing JSON data: ", e);
            }
        }
    }

    component ThemeData: QtObject {
        property string name: "Default"
        property string author: "Anonymous"
        property string version: "1.0"

        property ColorPalette colors: ColorPalette {}
    }

    component ColorPalette: QtObject {
        property color base0: "#1b1510"
        property color base: "#25201d"
        property color base2: "#322d28"
        property color base3: "#4a3d34"
        property color base4: "#776156"

        property color baseContent: "#d9d3cd"
        property color baseContentMuted: "#847e78"

        property color primary: "#e25016"
        property color primary5: "#f4b17a"

        property color destructive: "#e32e35"
        property color destructiveHover: "#ed5c4b"

        property color emphasisFavorite: "#efe302"
    }
}
