pragma ComponentBehavior: Bound

import MyShellControlPanel.plugin
import QtQml.Models
import QtQuick
import QtQuick.Layouts

DelegateChooser {
    id: root
    role: "type"

    property bool includeClassName: false

    property bool layoutFillWidth: true
    property real floatStepSize: 0.1
    property real floatMinimumValue: 0.0
    property real floatMaximumValue: 1000.0

    DelegateChoice {
        roleValue: "double"

        delegate: DelegateFloatType {}
    }

    DelegateChoice {
        id: defaultChoice

        delegate: InvalidChoice {}
    }

    component DelegateFloatType: SFloatInput {
        required property FieldController modelData
        controller: modelData

        name: root.includeClassName ? `${modelData.className}.${modelData.name}` : modelData.name

        Layout.fillWidth: root.layoutFillWidth
        boxLayoutFillWidth: root.layoutFillWidth

        from: root.floatMinimumValue
        to: root.floatMaximumValue
        stepSize: root.floatStepSize
    }

    component InvalidChoice: QtObject {
        required property FieldController modelData
        Component.onCompleted: {
            console.warn(`AutoFieldDelegate: invalid delegate choice for field "${modelData.name}" - type ${modelData.type} is not mapped.`);
        }
    }
}
