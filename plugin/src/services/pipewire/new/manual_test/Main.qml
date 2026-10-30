import org.nightshell.Services.Pipewire

import QtQuick

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
    delegate: Text {
      required property string name
      text: name
    }
  }

  Item {
    anchors.fill: parent

    ListView {
      anchors.fill: parent
      model: sinkModel
    }
  }
}
