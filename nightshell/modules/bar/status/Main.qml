import org.nightshell.Configs
import QtQuick
import QtQuick.Layouts

Item {
	id: root

	property BaseStatus currentActive: null

	implicitWidth: mainLayout.width
	Layout.fillHeight: true

	RowLayout {
		id: mainLayout

		anchors {
			top: parent.top
			topMargin: 2
			bottom: parent.bottom
			bottomMargin: 2
			horizontalCenter: parent.horizontalCenter
		}

		WrappedStatus {
			id: languageStatus
			sourceComponent: Language {}
		}

		WrappedStatus {
			id: networkStatus
			sourceComponent: Network {}
		}

		WrappedStatus {
			id: gamemodeStatus
			active: Config.modules.enabledModules.gamemode
			sourceComponent: Gamemode {}
		}

		WrappedStatus {
			id: audioStatus
			sourceComponent: Audio {}
		}

		WrappedStatus {
			id: notificationStatus
			active: Config.modules.enabledModules.notifications
			sourceComponent: Notifications {}
		}
	}

	component WrappedStatus: Loader {
		active: true
		asynchronous: true
		Layout.fillHeight: true
	}
}
