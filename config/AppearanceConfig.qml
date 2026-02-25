import Quickshell
import Quickshell.Io
import QtQuick

JsonObject {
    property FontFamily fontFamily: FontFamily {}
    property FontSize fontSize: FontSize {}
    property Rounding rounding: Rounding {}
    property Spacing spacing: Spacing {}
    property Padding padding: Padding {}

    component FontFamily: JsonObject {
        property string sans: "DejaVu Sans"
        property string mono: "CaskaydiaCove Nerd Font"
        property string material: "Material Symbols Rounded"
    }

    component FontSize: JsonObject {
        property real scale: 1
        property int xs: 7 * scale
        property int sm: 10 * scale
        property int md: 12 * scale
        property int lg: 14 * scale
        property int xl: 17 * scale
    }

    component Rounding: JsonObject {
        property real scale: 1
        property int xs: 4 * scale
        property int sm: 8 * scale
        property int md: 16 * scale
        property int lg: 24 * scale
        property int full: 1000 * scale
    }

    component Spacing: JsonObject {
        property real scale: 1
        property int xs: 7 * scale
        property int sm: 10 * scale
        property int md: 12 * scale
        property int lg: 14 * scale
        property int xl: 20 * scale
    }

    component Padding: JsonObject {
        property real scale: 1
        property int xs: 5 * scale
        property int sm: 7 * scale
        property int md: 10 * scale
        property int lg: 12 * scale
        property int xl: 15 * scale
    }
}
