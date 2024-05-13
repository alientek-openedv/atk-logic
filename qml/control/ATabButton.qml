import QtQuick 2.15
import QtQuick.Controls 2.5
import "../config"
import "../style"

Rectangle{
    id: mainRectangle
    width: (nshowText.contentWidth>tabButtonMaxWidth-20?tabButtonMaxWidth-20:nshowText.contentWidth)+20+
           (closeTabItem.visible?closeTabItem.width+10:0)+(connectLight.visible?connectLight.width+10:0)
    color:select?Config.pageColor:"transparent"
    radius: 6
    clip: true

    signal rightClick(var index, var x, var y);

    Rectangle{
        width: parent.radius
        height: parent.radius
        anchors{
            left: parent.left
            top: parent.top
        }
        color: parent.color
    }

    Rectangle{
        width: parent.radius
        height: parent.radius
        anchors{
            right: parent.right
            top: parent.top
        }
        color: parent.color
    }

    Rectangle{
        visible: showFirst
        width: 1
        height: 21
        color: Config.lineColor
        anchors{
            left: parent.left
            top: parent.top
            topMargin: 7
        }
        clip: true
    }

    Rectangle{
        visible: showBack
        width: 1
        height: 21
        color: Config.lineColor
        anchors{
            right: parent.right
            top: parent.top
            topMargin: 7
        }
        clip: true
    }

    MouseArea{
        anchors.fill: parent
        preventStealing: true
        acceptedButtons: Qt.AllButtons
        onPressed: {
            Signal.tabSelected(sessionID)
            refreshSplit();
            if(mouse.button==Qt.RightButton)
                rightClick(index, mainRectangle.x+mouse.x, mouse.y);
        }
    }

    Row{
        id: showRow
        spacing: 10
        height: parent.height-2
        width: parent.width-20
        anchors{
            left: parent.left
            leftMargin: 10
            top: parent.top
        }

        Rectangle{
            id: connectLight
            width: 8
            height: width
            radius: width/2
            anchors.verticalCenter: parent.verticalCenter
            color: isConnect===1?Config.green:Config.subheadColor
            visible: isConnect!==0
        }

        Text {
            id:showText
            elide: Text.ElideRight
            font.pixelSize: 14
            width: showRow.width-(closeTabItem.visible?closeTabItem.width+10:0)-(connectLight.visible?connectLight.width+10:0)
            anchors.verticalCenter: parent.verticalCenter
            text: buttonText
            color: select?Config.textColor:Config.subheadColor
            onTextChanged: nshowText.text=text
        }

        ImageButton{
            id: closeTabItem
            visible: type===Config.SessionType.File
            width: 8
            height: 8
            imageWidth: 8
            imageHeight: 8
            imageSource: "resource/icon/"+Config.tp+"/Close_small.png"
            imageEnterSource: imageSource
            anchors.verticalCenter: parent.verticalCenter
            onClicked: Signal.sessionCloseClicked(sessionID,0);
        }
    }

    XAnimator{
        target: scheduleRectangle;
        id: scheduleMove
        from: -scheduleRectangle.width
        to: mainRectangle.width+scheduleRectangle.width
        duration: 1000
        loops: Animation.Infinite
        alwaysRunToEnd: true
        running: isRunScheduleMove
    }

    Rectangle{
        id: scheduleRectangle
        height: 1
        width: parent.width*0.2
        visible: isRunScheduleMove
        anchors{
            bottom: parent.bottom
            bottomMargin: 0
        }
        color: Config.green
    }

    Text{
        id:nshowText
        visible: false
        elide: Text.ElideRight
        font.pixelSize: 14
        text: buttonText
    }
}
