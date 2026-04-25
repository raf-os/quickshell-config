import qs.modules.bar
import qs.config
import qs.modules.mprisviewer as MprisViewer
import qs.modules.session as Session
import qs.modules.bar.popouts as BPP
import qs.modules.startmenu as StartMenu
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

    BPP.Background {
        wrapper: root.panels.popouts // qmllint disable incompatible-type

        startX: wrapper.x - rounding
        startY: 0
    }

    MprisViewer.MprisBackground {
        wrapper: root.panels.mprisViewer // qmllint disable incompatible-type

        startX: wrapper.x
        startY: 0
    }

    StartMenu.Background {
        wrapper: root.panels.startmenu // qmllint disable incompatible-type

        startX: 0
        startY: 0
    }

    Session.Background {
        wrapper: root.panels.session // qmllint disable incompatible-type

        startX: root.width
        startY: 0
    }
}
