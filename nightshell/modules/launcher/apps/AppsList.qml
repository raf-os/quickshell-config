pragma ComponentBehavior: Bound

import qs.components

import org.nightshell.DesktopEntries
import org.nightshell.Configs

import QtQuick

Item {
	id: root

	required property DesktopEntriesModel model

	signal requestClose

	function moveForwards() {
		listView.view.incrementCurrentIndex();
	}

	function moveBackwards() {
		listView.view.decrementCurrentIndex();
	}

	function selectCurrent() {
		const current = (listView.currentItem as AppsItem);
		if (!current)
			return;

		current.select();
	}

	SListView {
		id: listView
		model: root.model.entryList

		anchors.fill: parent
		spacing: Styles.spacing_md
		padding: Styles.padding_sm
		scrollBarWidth: 12
		scrollBarSpacing: Styles.spacing_lg

		keyNavigationWraps: true
		highlightFollowsCurrentItem: true
		highlightResizeVelocity: -1
		highlightResizeDuration: -1
		highlightMoveVelocity: -1
		highlightMoveDuration: -1

		delegate: AppsItem {
			id: appsItemDelegate

			onSelected: {
				root.requestClose();
			}
		}
	}
}
