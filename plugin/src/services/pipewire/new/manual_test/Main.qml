import org.nightshell.Services.Pipewire

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
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

  ColumnLayout {
    id: mainLayout

    anchors.fill: parent

    Label {
      Layout.fillWidth: true
      text: "Defaults"
    }

    RowLayout {
      id: defaultsLayout
      Layout.fillWidth: true

      Label {
        readonly property PwNode defaultSink: PipewireGlobal.defaultSink
        text: defaultSink.description
      }
    }

    RowLayout {
      id: nodesLayout

      Layout.fillWidth: true

      spacing: 4

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
  }

  Component {
    id: nodeComponent

    Item {
      id: nodeItem

      required property PwNode modelData
      required property int nodeId
      required property string name
      required property string description
      required property string nickname

      implicitWidth: ListView.view ? ListView.view.width : 0
      implicitHeight: nodeMainLayout.height + 16

      Frame {
        anchors.fill: parent
      }

      ColumnLayout {
        id: nodeMainLayout

        spacing: 8

        anchors {
          left: parent.left
          leftMargin: 8
          right: parent.right
          rightMargin: 8
          verticalCenter: parent.verticalCenter
        }

        Label {
          id: nodeName

          Layout.fillWidth: true
          elide: Text.ElideRight

          MouseArea {
            id: tooltipArea
            anchors.fill: parent
            hoverEnabled: true
          }

          ToolTip.delay: 500
          ToolTip.visible: tooltipArea.containsMouse
          ToolTip.text: nodeName.text

          text: {
            if (nodeItem.description !== "")
              return nodeItem.description;
            if (nodeItem.nickname !== "")
              return nodeItem.nickname;
            return nodeItem.name;
          }
        }

        CheckBox {
          checked: nodeItem.modelData.muted
          text: "Muted"

          onToggled: {
            nodeItem.modelData.setMuted(checked);
          }
        }

        RowLayout {
          Layout.fillWidth: true

          Label {
            text: Math.round(nodeItem.modelData.volume * 100)
          }
          Slider {
            id: volumeSlider
            from: 0
            to: 1

            value: nodeItem.modelData.volume

            onMoved: {
              nodeItem.modelData.setVolume(value);
            }

            Layout.fillWidth: true
          }
        }
      }
    }
  }
}
