import QtQuick 2.13
import QtQuick.Controls 2.13
import "../config"

Rectangle {
    id: root
    property var cornersRadius : [0,0,0,0]
    color : Config.borderBackgroundColor
    width: parent.width
    height: parent.height

    Rectangle{
        id: leftTop
        width: parent.width/2
        height: parent.height/2
        color: root.color
        anchors{
            top: parent.top
            left: parent.left
        }
        visible: cornersRadius[0]>0
    }
    Rectangle{
        id: topRight
        width: parent.width/2
        height: parent.height/2
        color: root.color
        anchors{
            top: parent.top
            right: parent.right
        }
        visible: cornersRadius[1]>0
    }
    Rectangle{
        id: bottomRight
        width: parent.width/2
        height: parent.height/2
        color: root.color
        anchors{
            bottom: parent.bottom
            right: parent.right
        }
        visible: cornersRadius[2]>0
    }
    Rectangle{
        id: leftBottom
        width: parent.width/2
        height: parent.height/2
        color: root.color
        anchors{
            bottom: parent.bottom
            left: parent.left
        }
        visible: cornersRadius[3]>0
    }
}
