pragma ComponentBehavior: Bound

import org.nightshell.Configs

import QtQuick
import QtQuick.Layouts

Item {
  id: root

  property BaseStatus currentActive: null
  property string activeName: ""

  implicitWidth: mainLayout.width
  Layout.fillHeight: true

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

  RowLayout {
    id: mainLayout

    anchors {
      top: parent.top
      topMargin: 2
      bottom: parent.bottom
      bottomMargin: 2
      horizontalCenter: parent.horizontalCenter
    }

    WrappedStatus {
      id: languageStatus
      sourceComponent: Language {}
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
        onClicked: {
          root.toggleActive(notifStatus);
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
