import QtQuick 2.15
import QtQuick.Controls 2.5
import "../config"

Item {
    property bool isCheck: false
    property string buttonText
    property bool disable: false
    property color backgroundColor: Config.background2Color
    property alias fontPixelSize: showText.font.pixelSize
    property alias textVerticalCenterOffset: showText.anchors.verticalCenterOffset
    property alias containsMouse: mouseAreaRoot.containsMouse
    property bool autoCheck: true
    signal clicked_();

    implicitHeight: 14
    implicitWidth: checkButton.width+showText.implicitWidth+5

    MouseArea{
        id: mouseAreaRoot
        anchors.fill: parent
        hoverEnabled: true
        enabled: !disable
    }

    Button {
        id: checkButton
        clip: true
        width: 11
        height: 11
        enabled: !disable
        focusPolicy: Qt.NoFocus
        anchors{
            top: parent.top
            topMargin: 1
        }
        Image{
            visible: isCheck
            width: 13
            height: 10
            source: "../../resource/image/check.png"
            fillMode: Image.PreserveAspectFit
            anchors{
                bottom: parent.bottom
                horizontalCenter: parent.horizontalCenter
            }
        }
        background: Rectangle{
            id: backgroundRectangle
            color: disable ? Config.lineDisableColor : isCheck ? Config.mouseCheckColor : backgroundColor
            radius: 2
            border{
                width: isCheck ? 0:1
                color: disable ? Config.lineDisableColor : Config.iceBlue
            }
            Behavior on color{
                ColorAnimation {
                    duration: 100
                }
            }
        }
        onPressed: {
            if(autoCheck)
                isCheck=!isCheck
            else
                clicked_();
        }
    }
    Text {
        id: showText
        text: buttonText
        color: disable ? Config.textDisabledColor : Config.textColor
        height: parent.height
        width: contentWidth
        anchors{
            left: checkButton.right
            leftMargin: 5
            verticalCenter: parent.verticalCenter
        }
        MouseArea{
            anchors.fill: parent
            enabled: !disable
            onPressed: {
                if(autoCheck)
                    isCheck=!isCheck
                else
                    clicked_();
            }
        }
    }
}
