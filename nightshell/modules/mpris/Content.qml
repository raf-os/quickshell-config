pragma ComponentBehavior: Bound

import qs.components
import qs.components.animations

import org.nightshell.DesktopEntries
import org.nightshell.Mpris
import org.nightshell.Configs
import org.nightshell.Components

import QtQuick
import QtQuick.Effects
import QtQuick.Layouts

MouseArea {
  id: root

  required property bool isActive
  required property vector2d anchoring
  readonly property MprisPlayer player: Mpris.model.values.length > 0 ? Mpris.model.values[0] : null
  readonly property bool isPlaying: player && player.playbackState === PlaybackState.Playing
  readonly property int padding: Styles.padding_md
  readonly property int borderOffset: Styles.padding_lg

  // readonly property vector2d maskSize: {
  //   root.isExpanded; bodyWrapper.implicitHeight;
  //
  // }

  property bool pinned: false
  property bool isExpanded: containsMouse || pinned

  hoverEnabled: true

  implicitWidth: rootLayout.desiredWidth + borderOffset
  implicitHeight: rootLayout.desiredHeight + borderOffset

  opacity: 0

  signal exitAnimationFinished

  states: [
    State {
      name: "active"
      when: root.isActive

      PropertyChanges {
        root.opacity: 1
      }
    }
  ]

  transitions: [
    Transition {
      from: ""
      to: "active"

      FlickerPopIn {
        property: "opacity"
        duration: 600
      }
    },
    Transition {
      from: "active"
      to: ""

      SequentialAnimation {
        NumberAnimation {
          property: "opacity"
          duration: 300
        }
        ScriptAction {
          script: root.exitAnimationFinished()
        }
      }
    }
  ]

  QtObject {
    id: pData
    property string identity
    property string desktopEntry
    property string trackTitle
    property string artUrl
    property string albumName
    property bool canReadPosition
  }

  DebouncedPropertyViewer {
    ref: pData
    target: root.player
  }

  Item {
    id: rootLayout

    anchors {
      left: parent.left
      top: root.anchoring.y === Wrapper.Top ? parent.top : undefined
      bottom: root.anchoring.y === Wrapper.Bottom ? parent.bottom : undefined
      margins: root.borderOffset
    }

    readonly property int desiredWidth: (root.isExpanded ? 420 : headerLayout.implicitWidth + pinIconWrapper.implicitWidth + root.padding) + root.padding * 2
    readonly property int desiredHeight: headerLayout.implicitHeight + (root.isExpanded ? bodyLayout.implicitHeight + Styles.spacing_sm : 0) + root.padding * 2

    implicitWidth: desiredWidth
    implicitHeight: desiredHeight

    clip: true

    Behavior on implicitWidth {
      NAnim {
        duration: 600
      }
    }

    Behavior on implicitHeight {
      NAnim {
        duration: 300
      }
    }

    Rectangle {
      anchors.fill: parent
      color: Colors.primaryContent
      opacity: 0.75
    }

    RowLayout {
      id: headerLayout

      opacity: root.isExpanded ? 1 : 0.5

      anchors {
        left: parent.left
        leftMargin: root.padding
        top: parent.top
        topMargin: root.padding
        right: pinIconWrapper.left
        rightMargin: root.padding
      }

      spacing: Styles.spacing_sm

      Item {
        id: desktopEntryIconWrapper
        readonly property DesktopEntry entry: pData.desktopEntry !== "" ? EntryManager.findEntry(pData.desktopEntry) : null
        Layout.fillHeight: true

        implicitWidth: 16

        Loader {
          active: !!desktopEntryIconWrapper.entry
          anchors.centerIn: parent
          sourceComponent: XDGIcon {
            size: 24
            scale: desktopEntryIconWrapper.height / 24
            icon: desktopEntryIconWrapper.entry?.icon ?? ""
            fallback: "folder-music"
          }
        }

        Loader {
          active: !(!!desktopEntryIconWrapper.entry)
          anchors.centerIn: parent
          sourceComponent: ShellIcon {
            size: 24
            scale: desktopEntryIconWrapper.height / 24
            icon: "music_note"
            colorize: true
            color: Colors.primary
          }
        }
      }

      StyledText {
        text: pData.identity
        color: Colors.primary

        font.pointSize: Styles.text_md
        font.weight: 600

        style: Text.Raised
      }

      StyledText {
        Layout.fillWidth: true

        elide: Text.ElideRight

        text: pData.trackTitle
        color: Colors.secondary

        opacity: root.isExpanded ? 0 : 1

        Behavior on opacity {
          NAnim {
            duration: 500
          }
        }
      }
    }

    SMouseArea {
      id: pinIconWrapper

      anchors {
        top: parent.top
        right: parent.right
        rightMargin: root.padding
        verticalCenter: headerLayout.verticalCenter
      }

      implicitHeight: headerLayout.implicitHeight
      implicitWidth: implicitHeight

      onClicked: root.pinned = !root.pinned

      RectangularShadow {
        anchors.centerIn: pinIcon
        implicitWidth: 8
        implicitHeight: 8
        color: pinIcon.color
        radius: height / 2
        blur: 18
        spread: -4
      }

      ShellIcon {
        id: pinIcon
        anchors.centerIn: parent
        icon: root.pinned ? "pin_on" : "pin_off"
        size: 24
        scale: Math.min(parent.height, parent.width) / size

        colorize: true
        color: root.pinned ? Colors.secondary : Colors.primary
      }
    }

    Item {
      id: expandableBody

      anchors {
        left: parent.left
        leftMargin: root.padding
        top: headerLayout.bottom
        bottom: parent.bottom
      }

      implicitWidth: 420

      clip: true

      Item {
        id: bodyWrapper

        readonly property int padding: Styles.padding_lg

        anchors {
          left: parent.left
          right: parent.right
          bottom: parent.bottom
        }

        implicitHeight: bodyLayout.height + padding

        opacity: root.isExpanded ? 1 : 0
        visible: opacity > 0

        Behavior on opacity {
          NAnim {
            duration: 400
          }
        }

        RowLayout {
          id: bodyLayout

          anchors {
            left: parent.left
            leftMargin: bodyWrapper.padding
            right: parent.right
            rightMargin: bodyWrapper.padding
            verticalCenter: parent.verticalCenter
          }

          spacing: Styles.spacing_md

          Item {
            id: artWrapper

            Layout.minimumWidth: 96
            Layout.minimumHeight: 96

            Layout.alignment: Qt.AlignTop

            Rectangle {
              anchors.fill: parent
              color: Colors.secondary

              visible: artImage.status !== Image.Ready
            }

            Image {
              id: artImage
              anchors.centerIn: parent

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
            spacing: Styles.spacing_md

            StyledText {
              id: titleText

              Layout.fillWidth: true

              text: pData.trackTitle
              color: Colors.secondary

              font.pointSize: Styles.text_lg
              font.weight: 600

              style: Text.Raised
              elide: Text.ElideRight
            }

            Item {
              id: albumNameWrapper

              Layout.fillWidth: true
              implicitHeight: albumNameFontMetrics.lineSpacing * 2

              FontMetrics {
                id: albumNameFontMetrics
                font: albumNameText.font
              }

              StyledText {
                id: albumNameText

                anchors.fill: parent
                visible: pData.albumName !== ""

                text: pData.albumName
                color: Colors.secondary
                style: Text.Raised
                elide: Text.ElideRight

                wrapMode: Text.Wrap
                maximumLineCount: 2
              }
            }

            Item {
              id: progressBarWrapper

              Layout.fillWidth: true
              implicitHeight: 16

              Rectangle {
                id: progressBg
                anchors.fill: parent
                color: root.player?.canReadPosition ? Colors.secondaryContent : Colors.primaryContent
              }

              RectangularShadow {
                anchors.fill: progressFg
                color: progressFg.color

                blur: 18
                spread: -1

                opacity: Math.min(1, (width / blur))
              }

              Rectangle {
                id: progressFg
                color: Colors.secondary
                implicitWidth: (root.player?.progress ?? 0) * parent.width
                implicitHeight: parent.implicitHeight
              }
            }

            RowLayout {
              id: controlsWrapper

              Layout.alignment: Qt.AlignHCenter

              CButton {
                enabled: root.player && root.player.canGoPrevious
                icon: "media_skip_next"
                flipH: true

                onClicked: {
                  root.player.goPrevious();
                }
              }

              CButton {
                enabled: root.player && (root.player.canPlay && root.player.canPause)
                icon: root.isPlaying ? "media_pause" : "media_play"

                onClicked: {
                  root.player.togglePlay();
                }
              }

              CButton {
                enabled: root.player && root.player.canGoNext
                icon: "media_skip_next"

                onClicked: {
                  root.player.goNext();
                }
              }

              Rectangle {
                implicitWidth: 2
                implicitHeight: 24
                color: Colors.secondary

                Layout.leftMargin: Styles.spacing_md
                Layout.rightMargin: Styles.spacing_md
              }

              CButton {
                enabled: root.player && root.player.canControl
                icon: "media_shuffle"
                color: enabled ? (root.player.shuffle ? Colors.accent : Colors.secondary) : Colors.primary

                onClicked: {
                  root.player.toggleShuffle();
                }
              }

              CButton {
                enabled: root.player && root.player.canControl
                interactable: false

                icon: (enabled && root.player.loopState === LoopState.Track) ? "media_repeat_one" : "media_repeat"
                color: enabled ? (root.player.loopState !== LoopState.None ? Colors.accent : Colors.secondaryContent) : Colors.primary
              }
            }
          }
        }
      }
    }
  }

  component CButton: ControlButton {
    color: enabled ? Colors.secondary : Colors.primary

    size: 24
  }
}
