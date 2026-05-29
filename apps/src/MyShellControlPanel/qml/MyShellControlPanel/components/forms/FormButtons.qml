import MyShellPlugin.Configs
import MyShellControlPanel.plugin
import MyShellControlPanel.components
import QtQuick
import QtQuick.Layouts

RowLayout {
    id: root

    Layout.fillWidth: true

    required property FormController controller
    spacing: Config.appearance.spacing.md

    function onSaveForm() {
    }

    function onCancelForm() {
        root.controller.resetForm();
    }

    SButton {
        id: saveButton
        text: "Save"
        autoWidth: true
        Layout.fillWidth: true
        disabled: !root.controller.isDirty

        onActivated: {
            if (disabled)
                return;
            root.onSaveForm();
        }
    }

    SButton {
        id: cancelButton
        text: "Cancel"
        autoWidth: true
        Layout.fillWidth: true
        type: SButton.ButtonType.Destructive
        disabled: !root.controller.isDirty

        onActivated: {
            if (disabled)
                return;
            root.onCancelForm();
        }
    }
}
