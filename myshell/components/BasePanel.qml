import Quickshell
import Quickshell.Wayland

PanelWindow {
    required property string name

    WlrLayershell.namespace: `myshell-${name}`
    color: "transparent"
}
