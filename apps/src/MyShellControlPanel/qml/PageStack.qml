import MyShellControlPanel.components
import QtQuick
import QtQuick.Controls

import "configs" as RootPage

StackView {
    id: root

    clip: true

    initialItem: RootPage.Index {}

    readonly property PageStackItem currentPage: currentItem as PageStackItem
}
