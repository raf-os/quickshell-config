import org.nightshell.Configs

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

RowLayout {
	id: root

	implicitWidth: StackView.view ? StackView.view.width : 0
	implicitHeight: StackView.view ? StackView.view.height : 0

	spacing: Styles.spacing_lg
}
