pragma ComponentBehavior: Bound

import MyShellPlugin
import MyShellPlugin.Configs
import qs.components
import QtQuick
import QtQuick.Controls

// Abstract component
Item {
	id: root

	required property BaseListView view

	property int tabIdx: -1
	property Content content: null

	property string queryString
	property bool highlightHotkeys
	property int spacing: 0

	implicitWidth: StackView.view ? StackView.view.width : 0
	implicitHeight: StackView.view ? StackView.view.height : 0

	signal requestClose
	signal hotkeyTriggered(key: int)

	function moveListForwards() {
		view.listView.incrementCurrentIndex();
	}
	function moveListBackwards() {
		view.listView.decrementCurrentIndex();
	}
	function moveListToBeginning() {
		view.listView.currentIndex = 0;
	}
	function moveListToEnd() {
		view.listView.currentIndex = Math.max(0, view.listView.count - 1);
	}
	function selectItem() {
	}

	Connections {
		target: root.content
		enabled: root.content !== null

		function onNextItemRequested() {
			root.moveListForwards();
		}
		function onPreviousItemRequested() {
			root.moveListBackwards();
		}
		function onJumpToListStart() {
			root.moveListToBeginning();
		}
		function onJumpToListEnd() {
			root.moveListToEnd();
		}
		function onHotkeyIndexActivated(key: int): void {
			root.hotkeyTriggered(key);
		}
		function onCurrentItemActivated(): void {
			root.selectItem();
		}
	}
}
