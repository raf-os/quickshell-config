pragma Singleton

import org.nightshell.Services
import QtQuick

Item {
	id: root

	readonly property string timeStr: format("hh:mm")
	readonly property string dateStr: format("dd/MM/yy")

	function format(fmt: string): string {
		return Qt.formatDateTime(SystemTime.date, fmt);
	}
}
