pragma Singleton

import org.nightshell.Services.Pipewire

import Quickshell
import QtQuick

Singleton {
  id: root

  readonly property PwNode defaultSink: PipewireGlobal.defaultSink
  readonly property bool defaultSinkMuted: defaultSink ? defaultSink.muted : false
  readonly property real defaultSinkVolume: defaultSink ? defaultSink.volume : 0

  readonly property string sinkIcon: {
    if (!defaultSink)
      return "volume_off";
    else if (defaultSinkMuted)
      return "volume_off";
    else if (defaultSinkVolume >= 0.75)
      return "volume_up";
    else if (defaultSinkVolume >= 0.4)
      return "volume_down";
    else
      return "volume_low";
  }
}
