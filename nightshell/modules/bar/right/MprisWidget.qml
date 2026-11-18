pragma ComponentBehavior: Bound

import qs.components
import qs.components.animations

import org.nightshell.Components
import org.nightshell.DesktopEntries
import org.nightshell.Configs
import org.nightshell.Mpris

import QtQuick
import QtQuick.Layouts

Loader {
  id: root

  Layout.fillHeight: true

  active: Config.modules.enabledModules.mpris
  sourceComponent: Item {
    id: mprisWidget

    readonly property MprisPlayer current: Mpris.model.values.length > 0 ? Mpris.model.values[0] : null
    readonly property bool isPlaying: current ? current.playbackState === PlaybackState.Playing : false
    readonly property int padding: Styles.padding_sm

    readonly property real desiredWidth: !!current ? mainLayout.width + padding * 2 + Math.floor(height / 6) : 0
    property bool enterAnimDone: false

    implicitWidth: desiredWidth
    clip: true

    visible: implicitWidth > 0

    onCurrentChanged: {
      if (mprisWidget.current === null)
        mprisWidget.enterAnimDone = false;
      else {
        player.desktopEntry = current.desktopEntry;
      }
    }

    Behavior on implicitWidth {
      enabled: !mprisWidget.enterAnimDone || mprisWidget.current === null
      NAnim {
        duration: 300
      }
    }

    states: [
      State {
        name: "active"
        when: mprisWidget.current !== null

        PropertyChanges {
          mainLayout.opacity: 1
        }
      },
      State {
        name: ""
        PropertyChanges {
          mainLayout.opacity: 0
        }
      }
    ]

    transitions: [
      Transition {
        from: ""
        to: "active"
        FlickerPopIn {
          target: mainLayout
          duration: 600
        }

        SequentialAnimation {
          PauseAnimation {
            duration: 300
          }
          ScriptAction {
            script: mprisWidget.enterAnimDone = true
          }
        }
      },
      Transition {
        from: "active"
        to: ""

        NAnim {
          target: mainLayout
          duration: 300
        }
      }
    ]

    QtObject {
      id: player
      property string trackTitle
      property string desktopEntry
      property bool isPlaying: false
      property bool canPlay: false
      property bool canPause: false
      property bool canGoNext: false
      property bool canGoPrevious: false
    }

    DebouncedPropertyViewer {
      ref: player
      target: mprisWidget.current
    }

    ChamferRect {
      id: bgRect
      anchors.fill: parent
      color: Colors.primaryContent
      borderWidth: 1
      borderColor: Colors.primary
      bottomLeftChamfer: Math.floor(height / 3)
    }

    RowLayout {
      id: mainLayout

      anchors {
        top: parent.top
        bottom: parent.bottom
        right: parent.right
        rightMargin: mprisWidget.padding
      }

      Item {
        id: iconWrapper

        Layout.fillHeight: true
        Layout.minimumWidth: 16

        implicitWidth: 16

        XDGIcon {
          anchors.centerIn: parent
          size: 24
          scale: Math.min(parent.height, parent.width) / size
          icon: player.desktopEntry !== "" ? EntryManager.findEntry(player.desktopEntry).icon : "application-octet-stream"
          fallback: "application-octet-stream"
        }
      }

      STextMetrics {
        id: titleMetrics
        text: player.trackTitle

        readonly property bool isElided: width > titleWrapper.width + titleWrapper.padding * 2

        font.pointSize: Styles.text_sm

        elide: Qt.ElideRight
        // elideWidth: titleWrapper.width
      }

      Item {
        id: titleWrapper

        readonly property int padding: Styles.padding_sm
        property int dynamicAnimDuration: 5000

        Layout.fillWidth: true
        Layout.fillHeight: true

        Layout.maximumWidth: 96
        Layout.preferredWidth: titleMetrics.width + padding * 2

        clip: true

        StyledText {
          id: titleText

          anchors {
            top: parent.top
            bottom: parent.bottom
          }

          text: titleMetrics.text
          font: titleMetrics.font

          onTextChanged: {
            if (titleMetrics.isElided) {
              scrollAnimation.restart();
              titleWrapper.dynamicAnimDuration = titleMetrics.width * 50; // ~ 50 pixels per second
            } else {
              titleText.x = 0;
              scrollAnimation.complete();
            }
          }

          leftPadding: titleWrapper.padding

          verticalAlignment: Text.AlignVCenter

          SequentialAnimation on x {
            id: scrollAnimation

            loops: Animation.Infinite
            running: titleMetrics.isElided

            PropertyAction {
              target: titleText
              property: "x"
              value: 0
            }
            PauseAnimation {
              duration: 2000
            }
            NumberAnimation {
              target: titleText
              property: "x"
              to: -(titleMetrics.width + titleWrapper.padding + Styles.spacing_lg)
              duration: titleWrapper.dynamicAnimDuration
            }
            PropertyAction {
              target: titleText
              property: "x"
              value: 0
            }
            PauseAnimation {
              duration: 2000
            }
          }
        }

        Loader {
          active: titleMetrics.isElided
          anchors {
            top: parent.top
            bottom: parent.bottom
            left: titleText.right
            leftMargin: Styles.spacing_lg + titleText.leftPadding
          }
          sourceComponent: StyledText {
            text: titleMetrics.text
            font: titleMetrics.font
            color: titleText.color
            verticalAlignment: titleText.verticalAlignment
          }
        }
      }

      Loader {
        Layout.fillWidth: true
        Layout.fillHeight: true
        active: mprisWidget.current && mprisWidget.current.canControl
        sourceComponent: RowLayout {
          ControlIcon {
            enabled: player.canGoPrevious
            icon: "media_skip_next"
            flipH: true

            onClicked: {
              mprisWidget.current.goPrevious();
            }
          }

          ControlIcon {
            enabled: mprisWidget.isPlaying ? player.canPause : player.canPlay
            icon: mprisWidget.isPlaying ? "media_pause" : "media_play"

            onClicked: {
              mprisWidget.current.togglePlay();
            }
          }

          ControlIcon {
            enabled: player.canGoNext
            icon: "media_skip_next"

            onClicked: {
              mprisWidget.current.goNext();
            }
          }
        }
      }
    }
  }

  component ControlIcon: SMouseArea {
    id: cItem

    required property string icon
    property bool flipH: false

    Layout.minimumWidth: 16
    Layout.fillHeight: true

    ShellIcon {
      id: cIcon
      anchors.centerIn: parent
      icon: cItem.icon
      size: 24
      scale: Math.min(parent.width, parent.height) / size

      colorize: true
      color: cItem.enabled ? Colors.secondary : Colors.primary
      glow: true

      transform: Scale {
        xScale: cItem.flipH ? -1 : 1
        origin.x: cIcon.width / 2
      }
    }
  }
}
