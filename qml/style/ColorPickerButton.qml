import QtQuick 2.15
import QtQuick.Controls 2.5
import "../config"
import "../style"

Button {
    property alias isAlpha: popup.isAlpha
    property alias currentColor: popup.currentColor
    property alias popupX: popup.x
    property alias popupY: popup.y

    signal popupClose();
    width: 25
    height: 25
    background: Rectangle{
        id: backRect
        color: popup.currentColor
        radius: 4
        border{
            width: 1
            color: Config.borderLineColor
        }
        MouseArea{
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            acceptedButtons: Qt.NoButton
        }
    }

    ColorPickerPopup{
        id: popup
        y: parent.y+parent.height;
        onClosed: popupClose();
    }
    onClicked: {
        popup.open();
        popup.setColor();
    }
    function setColor(color){
        popup.setColor(color)
    }
}
