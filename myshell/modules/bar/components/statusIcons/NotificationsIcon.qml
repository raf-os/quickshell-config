import qs.components
import org.nightshell.Notifications
import MyShellPlugin
import MyShellPlugin.Configs
import Quickshell
import Quickshell.Io
import QtQuick

MouseArea {
	id: root
	property int activeNotifications: NotificationServer.model.values.length // qmllint disable unresolved-type

	signal triggerNotifications

	cursorShape: Qt.PointingHandCursor
	implicitWidth: Config.appearance.fontSize.xl
	// implicitHeight: Config.bar.sizes.innerHeight
	acceptedButtons: Qt.LeftButton | Qt.RightButton

	// onClicked: ev => {
	//     Quickshell.execDetached(['swaync-client', '-t']);
	// }

	onClicked: ev => {
		root.triggerNotifications();
	}

	StyledText {
		id: icon
		text: root.activeNotifications > 0 ? "󰂞" : "󰂜"
		color: root.activeNotifications > 0 ? Colors.colors.baseContent : Colors.colors.baseContentMuted
		font.pixelSize: Config.appearance.fontSize.xl
		font.family: Config.appearance.fontFamily.monoIcon
		anchors.fill: parent

		horizontalAlignment: Qt.AlignHCenter
		verticalAlignment: Qt.AlignVCenter
	}

	Item {
		id: activeNotifIndicator
		visible: root.activeNotifications >= 1

		anchors {
			right: parent.right
			rightMargin: 1
			bottom: parent.bottom
			bottomMargin: 1
		}

		implicitWidth: Math.max(notifAmountTxt.width, notifAmountTxt.height)
		implicitHeight: implicitWidth

		Rectangle {
			anchors.fill: parent
			radius: width / 2
			color: Colors.colors.primary
		}

		Text {
			id: notifAmountTxt

			anchors.centerIn: parent

			text: root.activeNotifications

			color: Colors.colors.baseContent
			font.family: Config.appearance.fontFamily.sans
			font.weight: 600
			font.pixelSize: 8
		}
	}

	// Process {
	//     running: true
	//     command: ["swaync-client", "-s"]
	//     stdout: SplitParser {
	//         onRead: data => {
	//             try {
	//                 const jData = JSON.parse(data);
	//                 root.activeNotifications = jData.count;
	//             } catch (e) {
	//                 console.log("Failed parsing notification json");
	//             }
	//         }
	//     }
	// }
}
