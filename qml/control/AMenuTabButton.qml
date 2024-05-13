import QtQuick 2.15
import QtQuick.Controls 2.5
import "../config"

Item {
    required property var buttonText
    required property var buttonIndex
    property int buttonClickCount:0

    height: parent.height
    width: mainRectangle.width+mainRectangle.anchors.leftMargin*2
    clip: true
    state: "noSelected"
    states: [
        State {
            name: "selected"
            PropertyChanges {
                target: mainRectangle
                color:Config.pageColor
            }
        },
        State {
            name: "noSelected"
            PropertyChanges {
                target: mainRectangle
                color:"transparent"
            }
        }
    ]

    Rectangle{
        id: leftRectangle
        width: mainRectangle.width
        height: mainRectangle.height
        anchors{
            right: mainRectangle.left
            top: mainRectangle.top
        }
        radius: mainRectangle.radius
        color: Config.background2Color
        z:10
    }
    Rectangle{
        anchors{
            right: leftRectangle.right
            bottom: leftRectangle.bottom
        }
        width: leftRectangle.width/2
        height: leftRectangle.height/2
        color: mainRectangle.color
        z:1
    }
    Rectangle{
        id: rightRectangle
        width: mainRectangle.width
        height: mainRectangle.height
        anchors{
            left: mainRectangle.right
            top: mainRectangle.top
        }
        radius: mainRectangle.radius
        color: Config.background2Color
        z:10
    }
    Rectangle{
        anchors{
            left: rightRectangle.left
            bottom: rightRectangle.bottom
        }
        width: rightRectangle.width/2
        height: rightRectangle.height/2
        color: mainRectangle.color
        z:1
    }

    Rectangle{
        id: mainRectangle
        width: nshowText.contentWidth+32
        anchors{
            left: parent.left
            leftMargin: 6
            top: parent.top
            topMargin: 5
            bottom: parent.bottom
        }
        radius: 6
        color: Config.pageColor
        z:10
        Text {
            anchors{
                left: parent.left
                leftMargin: 16
                verticalCenter: parent.verticalCenter
            }
            font.pixelSize: 12
            text: buttonText
            color: Config.textColor
        }
        Text{
            id:nshowText
            visible: false
            font.pixelSize: 12
            text: buttonText
        }
    }
    Rectangle{
        anchors{
            left: mainRectangle.left
            right: mainRectangle.right
            bottom: mainRectangle.bottom
        }
        height: mainRectangle.height/2
        color: mainRectangle.color
    }

    Timer{
        id:clickTimer
        interval: 100
        onTriggered: {
            Signal.clickMenuTabButton(buttonIndex,buttonClickCount>1);
            buttonClickCount=0;
        }
    }

    MouseArea{
        anchors.fill: parent
        onPressed: {
            if(!clickTimer.running)
            {
                buttonClickCount++;
                clickTimer.start();
            }else{
                clickTimer.stop();
                buttonClickCount=0;
                Signal.clickMenuTabButton(buttonIndex,true);
            }
        }
        onDoubleClicked: buttonClickCount++;
    }
}
