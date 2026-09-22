import qs.components
import org.nightshell.Wallpaper
import QtQuick
import QtQuick.Controls

Item {
	id: root

	property string currentSource

	Connections {
		target: WallpaperManager

		function onCurrentChanged() {
			root.fetchAndApplyWallpaper();
		}
	}

	Component.onCompleted: {
		root.fetchAndApplyWallpaper(true);
	}

	function fetchAndApplyWallpaper(isInitial = false): void {
		if (!WallpaperManager.current) {
			wpStack.clear();
			return;
		} else {
			const src = WallpaperManager.current.path;
			const fillMode = root.mapFillMode(WallpaperManager.current.fillMode);

			if (src === "" || src === undefined || src === null) {
				wpStack.clear();
				return;
			}

			if (src === root.currentSource) {
				return;
			}

			wpStack.replaceCurrentItem(wpImageComponent, {
				source: src,
				fillMode
			}, isInitial ? StackView.Immediate : StackView.ReplaceTransition);

			root.currentSource = src;
			return;
		}
	}

	function mapFillMode(mode: int): int {
		switch (mode) {
		case WallpaperFillMode.Fit:
			return Image.PreserveAspectFit;
		case WallpaperFillMode.Fill:
			return Image.PreserveAspectCrop;
		case WallpaperFillMode.Stretch:
			return Image.Stretch;
		default:
			return Image.Stretch;
		}
	}

	StackView {
		id: wpStack

		anchors.fill: parent

		replaceEnter: Transition {
			NAnim {
				property: "opacity"
				from: 0
				to: 1
				duration: 1000
			}
		}

		replaceExit: Transition {
			PauseAnimation {
				duration: 1000
			}
		}
	}

	Component {
		id: wpImageComponent

		Image {
			width: StackView.view ? StackView.view.width : 0
			height: StackView.view ? StackView.view.height : 0

			clip: true

			asynchronous: true
			cache: false

			// sourceSize.width: parent.implicitWidth
			// sourceSize.height: parent.implicitHeight

			horizontalAlignment: Image.AlignHCenter
			verticalAlignment: Image.AlignVCenter
		}
	}
}
