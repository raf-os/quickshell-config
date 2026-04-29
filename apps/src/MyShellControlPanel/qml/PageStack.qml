import QtQuick
import QtQuick.Controls

import "configs/keyboard" as KeyboardPage

StackView {
    id: root

    clip: true

    initialItem: KeyboardPage.Index {}
}
