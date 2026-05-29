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

    property bool useSliderForNumbers: false

    DelegateChoice {
        roleValue: "double"

        delegate: DelegateFloatType {}
    }

    DelegateChoice {
        roleValue: "int"

        delegate: DelegateIntegerType {}
    }

    DelegateChoice {
        roleValue: "QString"

        delegate: DelegateStringType {}
    }

    DelegateChoice {
        roleValue: "bool"

        delegate: DelegateBoolType {}
    }

    DelegateChoice {
        roleValue: "QStringList"
        delegate: null
    }

    DelegateChoice {
        id: defaultChoice

        delegate: InvalidChoice {}
    }

    component DelegateBoolType: SCheckBox {
        required property FieldController modelData
        controller: modelData

        name: root.includeClassName ? `${modelData.className}.${modelData.name}` : modelData.name

        Layout.fillWidth: root.layoutFillWidth
    }

    component DelegateStringType: SStringInput {
        required property FieldController modelData
        controller: modelData

        name: root.includeClassName ? `${modelData.className}.${modelData.name}` : modelData.name
        boxLayoutFillWidth: root.layoutFillWidth
    }

    component DelegateIntegerType: SIntegerInput {
        required property FieldController modelData
        controller: modelData

        name: root.includeClassName ? `${modelData.className}.${modelData.name}` : modelData.name

        Layout.fillWidth: root.layoutFillWidth
        boxLayoutFillWidth: root.layoutFillWidth
        useSliderForNumbers: root.useSliderForNumbers

        from: 0
        to: 1000
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

    component InvalidChoice: Item {
        required property FieldController modelData
        Component.onCompleted: {
            console.warn(`AutoFieldDelegate: invalid delegate choice for field "${modelData.name}" - type ${modelData.type} is not mapped.`);
        }
    }
}
