import qs.modules.bar
import qs.config
import qs.modules.session as Session
// import qs.modules.bar.popouts as BPP
import Quickshell
import QtQuick
import QtQuick.Shapes

Shape {
    id: root

    required property Panels panels
    required property BarWrapper bar

    anchors.fill: parent
    anchors.margins: Config.border.thickness
    anchors.topMargin: bar.implicitHeight

    preferredRendererType: Shape.CurveRenderer

    // BPP.Background {
    //     wrapper: root.panels.popouts
    //
    //     startX: wrapper.x
    //     startY: 0
    // }

    Session.Background {
        wrapper: root.panels.session

        startX: root.width
        startY: 0
    }
}
