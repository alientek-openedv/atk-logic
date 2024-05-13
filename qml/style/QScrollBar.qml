import QtQuick 2.15
import QtQuick.Controls 2.5
import "../config"

ScrollBar {
    property int dely: 300
    id: vbar
    hoverEnabled: true
    active: hovered || pressed
    orientation: Qt.Vertical
    z: 105
    height: parent.height
    visible: visualSize!=1
    policy: ScrollBar.AsNeeded
    snapMode: ScrollBar.SnapOnRelease
    contentItem: Rectangle {
        implicitWidth: orientation===Qt.Vertical?6:100
        implicitHeight: orientation===Qt.Vertical?100:6
        clip: true
        color: Config.lineColor
    }
    Behavior on position{
        NumberAnimation{
            duration: dely
            easing.type: Easing.OutCubic
        }
    }
}


