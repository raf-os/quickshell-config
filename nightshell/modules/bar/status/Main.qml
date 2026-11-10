pragma ComponentBehavior: Bound

import qs.components

import org.nightshell.Components
import org.nightshell.Configs

import QtQuick
import QtQuick.Layouts

Item {
  id: root

  property BaseStatus currentActive: null
  property string activeName: ""

  readonly property int hPadding: Styles.padding_md
  readonly property int vPadding: Styles.padding_sm

  implicitWidth: mainLayout.width + hPadding * 2 + Math.max(0, ((height - vPadding * 2) / 3))
  Layout.fillHeight: true

  Behavior on implicitWidth {
    NAnim {
      duration: 300
    }
  }

  function toggleActive(item: BaseStatus) {
    if (item.name === "")
      return;

    if (currentActive === item && activeName === item.name) {
      currentActive = null;
    } else {
      currentActive = item;
      activeName = item.name;
    }
  }

  function requestCloseByName(name: string): void {
    if (!currentActive)
      return;
    if (currentActive.name !== name)
      return;

    currentActive = null;
  }

  function clearActive() {
    root.currentActive = null;
  }

  ChamferRect {
    id: bgRect
    anchors.fill: parent

    color: Colors.primaryContent
    borderWidth: 1
    borderColor: Colors.primary

    bottomLeftChamfer: Math.round(height / 3)
  }

  Item {
    id: wrapper
    anchors.fill: parent
    clip: true

    RowLayout {
      id: mainLayout

      anchors {
        top: parent.top
        topMargin: root.vPadding
        bottom: parent.bottom
        bottomMargin: root.vPadding
        right: parent.right
        rightMargin: root.hPadding
      }

      WrappedStatus {
        id: languageStatus
        sourceComponent: Language {
          id: languageStatusComp
          isActive: root.currentActive === languageStatusComp
          onClicked: {
            root.toggleActive(languageStatusComp);
          }
        }
      }

      WrappedStatus {
        id: networkStatus
        sourceComponent: Network {}
      }

      WrappedStatus {
        id: gamemodeStatus
        active: Config.modules.enabledModules.gamemode
        sourceComponent: Gamemode {}
      }

      WrappedStatus {
        id: audioStatus
        sourceComponent: Audio {
          id: audioStatusComp
          isActive: root.currentActive === audioStatusComp
          onClicked: {
            root.toggleActive(audioStatusComp);
          }
        }
      }

      WrappedStatus {
        id: notificationStatus
        active: Config.modules.enabledModules.notifications
        sourceComponent: Notifications {
          id: notifStatus
          isActive: root.currentActive === notifStatus
          onClicked: {
            root.toggleActive(notifStatus);
          }
        }
      }
    }
  }

  component WrappedStatus: Loader {
    active: true
    asynchronous: true
    Layout.fillHeight: true
  }
}
