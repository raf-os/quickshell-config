import org.nightshell.Services.Pipewire

import QtQuick
import QtQuick.Layouts

Window {
  id: root

  visible: true

  width: 640
  height: 480

  PipewireModel {
    id: pipewire

    trackedTypes: PipewireModel.Type.NodeSinkSource
  }

  DelegateModel {
    id: sinkModel
    rootIndex: pipewire.modelIndexFor(PipewireModel.Type.NodeSink)
    model: pipewire
    delegate: nodeComponent
  }

  DelegateModel {
    id: sourceModel
    rootIndex: pipewire.modelIndexFor(PipewireModel.Type.NodeSource)
    model: pipewire
    delegate: nodeComponent
  }

  RowLayout {
    anchors.fill: parent

    ListView {
      Layout.fillWidth: true
      Layout.fillHeight: true

      model: sinkModel
    }

    ListView {
      Layout.fillWidth: true
      Layout.fillHeight: true
      model: sourceModel
    }
  }

  Component {
    id: nodeComponent

    Item {
      id: nodeItem

      required property int id
      required property string name
      required property string description
      required property string nickname

      implicitWidth: ListView.view ? ListView.view.width : 0
      implicitHeight: nodeMainLayout.height

      ColumnLayout {
        id: nodeMainLayout

        anchors {
          left: parent.left
          right: parent.right
          top: parent.top
        }

        Text {
          text: `[${nodeItem.id}] ${nodeItem.name}`
        }

        Text {
          text: nodeItem.description
        }

        Text {
          text: nodeItem.nickname
        }
      }
    }
  }
}
