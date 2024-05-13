import QtQuick 2.15
import QtQuick.Controls 2.5
import "../config"
import "../style"

Popup {
    property alias isAlpha: colorPicker.isAlpha
    property alias currentColor: colorPicker.currentColor
    id: mainPopup
    width: colorPicker.width+15
    height: colorPicker.height+15
    contentWidth: width
    contentHeight: height
    padding: 1
    clip: true;
    background: Item{}
    Rectangle{
        width: parent.width
        height: parent.height
        radius: 4
        color: Config.background2Color
        border{
            width: 1
            color: Config.borderLineColor
        }
        ColorPicker{
            id: colorPicker
            anchors.centerIn: parent
        }
    }
    function setColor(color){
        colorPicker.setColor(color)
    }
}
