pragma Singleton

import Quickshell
import QtQuick

Singleton {
    readonly property int hours: clock.hours
    readonly property int minutes: clock.minutes

    readonly property string timeStr: format("hh:mm")
    readonly property string dateStr: format("dd/MM/yy")

    function format(fmt: string): string {
        return Qt.formatDateTime(clock.date, fmt);
    }

    SystemClock {
        id: clock
        precision: SystemClock.Seconds
    }
}
