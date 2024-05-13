import QtQuick 2.13
import QtQuick.Controls 2.5
import "../config"
import "../style"

Popup{
    id: mainPopup
    width: 350
    height: showText.contentHeight+20
    contentWidth: width
    contentHeight: height
    padding: 1
    clip: true;
    background: Item{}
    Connections{
        target: sSignal

        function onDecodeDocPopupShow(isShow, note)
        {
            if(isShow)
                mainPopup.open();
            else
                mainPopup.close();
            showText.text=note;
        }
    }
    Rectangle{
        id: mainRectangle
        width: parent.width
        height: parent.height
        radius: 4
        clip: true
        color: Config.background2Color
        border{
            width: 1
            color: Config.lineColor
        }

        Text {
            id: showText
            wrapMode: Text.WordWrap
            width: parent.width-20
            height: contentHeight
            font.pixelSize: 12
            color: Config.textColor
            anchors{
                left: parent.left
                top: parent.top
                margins: 10
            }
        }
    }
}
