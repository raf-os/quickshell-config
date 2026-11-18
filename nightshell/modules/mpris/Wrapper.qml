import qs.components

import org.nightshell.Mpris
import org.nightshell.Configs
import org.nightshell.Components

import QtQuick
import QtQuick.Effects
import QtQuick.Layouts

Item {
  id: root

  readonly property bool isActive: Mpris.model.values.length > 0

  enum HPosition {
    Left = 0,
    Right = 1
  }

  enum VPosition {
    Top = 0,
    Right = 1
  }

  readonly property vector2d anchorPos: {
    switch (Config.modules.mpris.position) {
    case "TopLeft":
      return Qt.vector2d(Wrapper.Left, Wrapper.Top);
    case "BottomLeft":
      return Qt.vector2d(Wrapper.Left, Wrapper.Bottom);
    case "TopRight":
      return Qt.vector2d(Wrapper.Right, Wrapper.Top);
    case "BottomRight":
      return Qt.vector2d(Wrapper.Right, Wrapper.Bottom);
    default:
      return Qt.vector2d(Wrapper.Left, Wrapper.Top);
    }
  }

  anchors {
    left: root.anchorPos.x === Wrapper.Left ? parent.left : undefined
    right: root.anchorPos.x === Wrapper.Right ? parent.right : undefined
    top: root.anchorPos.y === Wrapper.Top ? parent.top : undefined
    bottom: root.anchorPos.y === Wrapper.Bottom ? parent.bottom : undefined
    margins: Styles.padding_lg
  }

  Loader {
    active: root.isActive
    sourceComponent: Item {
      id: lditem
      readonly property MprisPlayer player: Mpris.model.values.length > 0 ? Mpris.model.values[0] : null

      QtObject {
        id: pData
        property string identity
        property string trackTitle
        property string artUrl
        property string albumName
        property bool canReadPosition
      }

      DebouncedPropertyViewer {
        ref: pData
        target: lditem.player
      }

      ColumnLayout {
        width: 320

        StyledText {
          text: lditem.player.identity
          color: Colors.primary

          font.pointSize: Styles.text_md
          font.weight: 600

          style: Text.Raised
        }

        RowLayout {
          Layout.fillWidth: true
          Layout.fillHeight: true

          spacing: Styles.spacing_md

          Item {
            id: artWrapper

            Layout.minimumWidth: 96
            Layout.minimumHeight: 96

            Rectangle {
              anchors.fill: parent
              color: Colors.secondary

              visible: artImage.status !== Image.Ready
            }

            Image {
              id: artImage
              asynchronous: true
              source: pData.artUrl
              width: 96
              height: 96

              layer.enabled: true
              layer.effect: MultiEffect {
                colorization: 1
                colorizationColor: Colors.secondary
              }
            }
          }

          ColumnLayout {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignTop

            StyledText {
              text: pData.trackTitle
              color: Colors.secondary

              Layout.fillWidth: true

              font.pointSize: Styles.text_lg
              font.weight: 600

              style: Text.Raised

              elide: Text.ElideRight
            }

            StyledText {
              text: pData.albumName
              visible: pData.albumName !== ""

              Layout.fillWidth: true

              color: Colors.secondary

              style: Text.Raised

              elide: Text.ElideRight
              wrapMode: Text.Wrap
              maximumLineCount: 2
            }

            Item {
              Layout.fillWidth: true
              implicitHeight: 16

              Rectangle {
                anchors.fill: parent
                color: lditem.player?.canReadPosition ? Colors.secondaryContent : Colors.primaryContent
              }

              RectangularShadow {
                anchors.fill: fgBar
                color: Colors.secondary
                blur: 18
                spread: -1
                opacity: Math.min(1, width / blur)
              }

              Rectangle {
                id: fgBar
                color: Colors.secondary
                implicitWidth: lditem.player.progress * parent.width
                anchors {
                  left: parent.left
                  top: parent.top
                  bottom: parent.bottom
                }
              }
            }
          }
        }
      }
    }
  }
}
