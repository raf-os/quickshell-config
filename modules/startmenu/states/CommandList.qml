import qs.config
import qs.components
import qs.services
import MyShellPlugin
import QtQuick

ListView {
    id: root

    required property string cmdQuery
    model: UserCommandService.entries

    onCmdQueryChanged: {
        UserCommandService.setQuery(cmdQuery.slice(Config.launcher.commandPrefix.length));
    }

    delegateModelAccess: DelegateModel.ReadOnly

    delegate: StyledText {
        required property CmdEntry modelData

        text: `${modelData.label}`
    }
}
