pragma Singleton

import org.nightshell.Configs
import QtQuick

Item {
	id: root

	readonly property int scaling: Config.appearance.scaling

	readonly property int text_xs: scaling * 8
	readonly property int text_sm: scaling * 10
	readonly property int text_m: scaling * 12
	readonly property int text_lg: scaling * 15
	readonly property int text_xl: scaling * 17

	readonly property int padding_xs: scaling * 2
	readonly property int padding_sm: scaling * 5
	readonly property int padding_md: scaling * 7
	readonly property int padding_lg: scaling * 10
	readonly property int padding_xl: scaling * 13

	readonly property int spacing_xs: scaling * 2
	readonly property int spacing_sm: scaling * 4
	readonly property int spacing_md: scaling * 5
	readonly property int spacing_lg: scaling * 7
	readonly property int spacing_xl: scaling * 10
}
