import MyShellPlugin
import MyShellPlugin.Configs
import MyShellControlPanel.components
import QtQuick
import QtQuick.Layouts

PageStackItem {
    id: root
    title: "Index"

    ColumnLayout {
        id: rootLayout

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top

        StyledText {
            textFormat: Text.MarkdownText
            text: `# NightShell Control Panel` + //
            `\n\nWelcome to the NightShell control panel. Configuration options are available on the menu on the left side of this panel.` + //
            `\n\nIf you wish to manually edit the configuration files instead, they can be accessed at \`\$XDG_CONFIG_HOME/myshell/configs.json\`. ` + //
            `Usually, this is located at \`~/.config/myshell/configs.json\`.` + //
            `\n\nFor any additional information, or if you wish to report a bug, please check out the repository at <a href="https://github.com/raf-os/quickshell-config" style="color:${Colors.colors.primary}">https://github.com/raf-os/quickshell-config</a>.`

            font.family: Config.appearance.fontFamily.sans
            font.pointSize: Config.appearance.fontSize.md

            horizontalAlignment: Text.AlignJustify

            Layout.fillWidth: true
            wrapMode: Text.WordWrap

            onLinkActivated: link => {
                Qt.openUrlExternally(link);
            }

            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.NoButton
                cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
            }
        }
    }
}
