pragma ComponentBehavior: Bound

import qs.services
import qs.components

import org.nightshell.Services.Pipewire
import org.nightshell.Configs

import QtQuick
import QtQuick.Layouts

BasePopout {
  id: root

  readonly property int padding: Styles.padding_md

  implicitWidth: 320
  implicitHeight: rootLayout.height + padding * 2

  PipewireModel {
    id: pipewire
    trackedTypes: PipewireModel.NodeSinkSource
  }

  DelegateModel {
    id: sinksModel
    rootIndex: pipewire.modelIndexFor(PipewireModel.Type.NodeSink)
    model: pipewire
    delegate: NodeController {}
  }

  ColumnLayout {
    id: rootLayout

    anchors {
      left: parent.left
      right: parent.right
      top: parent.top
      margins: root.padding
    }

    DefaultNodeSettings {
      id: defaultSinkComponent
      node: PipewireGlobal.defaultSink
      iconOn: "volume_up"
      iconOff: "volume_off"
    }

    DefaultNodeSettings {
      id: defaultSourceComponent
      node: PipewireGlobal.defaultSource
      iconOn: "mic_on"
      iconOff: "mic_off"
    }

    ListView {
      id: nodeList

      Layout.fillWidth: true

      implicitHeight: 256

      orientation: ListView.Horizontal
      model: sinksModel
    }
  }

  component NodeController: Item {
    id: ncRoot

    required property PwNode modelData

    implicitWidth: 32
    implicitHeight: ListView.view ? ListView.view.height : 0

    ColumnLayout {
      id: nodeLayout

      anchors.fill: parent

      MouseArea {
        id: appIconWrapper

        implicitWidth: 24
        implicitHeight: 24
      }

      SSlider {
        Layout.fillHeight: true
        mirror: true

        from: 0
        to: 1
        value: ncRoot.modelData.volume
      }
    }
  }

  component DefaultNodeSettings: Loader {
    id: defaultNodeSettingsComponent
    required property PwNode node
    required property string iconOn
    required property string iconOff

    active: !!node
    visible: active
    Layout.fillWidth: true

    sourceComponent: ColumnLayout {
      id: nodeHorizontalSlider

      StyledText {
        Layout.fillWidth: true
        text: AudioService.defaultSink?.nickname ?? "Unknown node"
        elide: Text.ElideRight
        color: Colors.primary
        font.pointSize: Styles.text_xs
      }

      RowLayout {
        Layout.fillWidth: true
        spacing: Styles.spacing_sm

        Item {
          id: volumeBg

          implicitWidth: volumeMetrics.width + Styles.padding_xs * 2
          implicitHeight: volumeMetrics.height + Styles.padding_xs * 2
          Layout.fillHeight: true

          TextMetrics {
            id: volumeMetrics
            font.family: Config.appearance.fontFamily.sans
            font.weight: 600
            font.pointSize: Styles.text_sm
            text: "100%"
          }

          Text {
            id: volumeAmount
            anchors.fill: parent
            text: `${Math.round(defaultNodeSettingsComponent.node.volume * 100)}%`
            font: volumeMetrics.font
            color: Colors.primary

            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
          }
        }

        SMouseArea {
          Layout.fillHeight: true
          implicitWidth: Styles.text_xl

          onClicked: {
            defaultNodeSettingsComponent.node.setMuted(!defaultNodeSettingsComponent.node.muted);
          }

          ShellIcon {
            anchors.centerIn: parent
            size: Styles.text_xl
            icon: defaultNodeSettingsComponent.node.muted ? defaultNodeSettingsComponent.iconOff : defaultNodeSettingsComponent.iconOn

            colorize: true
            glow: true

            color: defaultNodeSettingsComponent.node.muted ? Colors.primary : Colors.secondary
          }
        }

        SSlider {
          Layout.fillWidth: true
          from: 0
          to: 1
          value: defaultNodeSettingsComponent.node.volume
          opacity: defaultNodeSettingsComponent.node.muted ? 0.5 : 1
          onMoved: {
            if (defaultNodeSettingsComponent.node) {
              if (defaultNodeSettingsComponent.node.muted) {
                defaultNodeSettingsComponent.node.setMuted(false);
              }
              defaultNodeSettingsComponent.node.setVolume(Math.round(value * 100) / 100);
            }
          }
        }
      }
    }
  }
}
