pragma ComponentBehavior: Bound

import qs.utils
import qs.components
import MyShellPlugin
import MyShellPlugin.Configs
import org.nightshell.DesktopEntries
import org.nightshell.Components
import org.nightshell.Wayland
import org.nightshell.Hyprland
import QtQuick

MouseArea {
	id: root

	required property ToplevelInstance modelData

	cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor

	signal closeRequested

	function select() {
		if (modelData) {
			modelData.activate();
		}
		closeRequested();
	}

	onClicked: ev => {
		root.select();
	}

	Item {
		id: viewWrapper

		anchors {
			top: parent.top
			left: parent.left
			right: parent.right
			bottom: footer.top
		}

		anchors.margins: 16

		Loader {
			active: GlobalStateManager.isGameMode
			anchors.centerIn: parent

			sourceComponent: Rectangle {
				anchors.centerIn: parent

				implicitWidth: Math.min(viewWrapper.width, viewWrapper.height)
				implicitHeight: implicitWidth

				color: Colors.colors.base0
				radius: Config.appearance.rounding.sm

				AppIconComponent {
					anchors.centerIn: parent
					size: 24
				}
			}
		}

		Loader {
			active: !GlobalStateManager.isGameMode
			anchors.centerIn: parent

			sourceComponent: RoundedClipRect {
				// On top of making the toplevel view rounded, using a qtquick layer means window transparency is handled more gracefully as a bonus
				id: roundedClip
				radius: Config.appearance.rounding.sm
				anchors.centerIn: parent

				implicitWidth: toplevelViewer.implicitWidth
				implicitHeight: toplevelViewer.implicitHeight

				ScreencopyQMLView {
					id: toplevelViewer

					anchors.centerIn: parent

					captureSource: root.modelData
					constraints: Qt.size(viewWrapper.width, viewWrapper.height)

					isLive: true
				}

				Rectangle {
					anchors.fill: toplevelViewer

					color: "transparent"
					radius: roundedClip.radius

					border.width: 2
					border.color: Colors.colors.baseContent
				}
			}
		}
	}

	Item {
		id: footer

		implicitHeight: 32

		anchors {
			left: parent.left
			right: parent.right
			bottom: parent.bottom
		}

		Item {
			id: iconWrapper

			anchors {
				left: parent.left
				top: parent.top
				bottom: parent.bottom
			}

			implicitWidth: 24 + Config.appearance.padding.sm * 2

			AppIconComponent {
				anchors.centerIn: parent
				size: 16
			}
		}

		StyledText {
			anchors {
				left: iconWrapper.right
				right: parent.right
				top: parent.top
				bottom: parent.bottom
			}

			text: root.modelData.title
			elide: Text.ElideRight

			font.pointSize: Config.appearance.fontSize.xs

			padding: Config.appearance.padding.sm

			horizontalAlignment: Text.AlignHCenter
			verticalAlignment: Text.AlignVCenter
		}
	}

	component AppIconComponent: Image {
		required property int size
		asynchronous: true
		source: `image://qicons/qt/${EntryManager.findEntry(root.modelData.appId).icon}?fallback=application-octet-stream`

		width: size
		height: size
	}
}
