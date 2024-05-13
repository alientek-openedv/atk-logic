import QtQuick 2.15
import QtQuick.Controls 2.5
import "../config"

Text{
    property string openUrl: ""
    property alias fontSize: rootText.font.pixelSize
    signal mouseEnter
    signal mouseExit
    id: rootText
    text: showText
    font{
        pixelSize: 12
        underline: true
    }
    color: mouseArea.containsMouse?Config.red:Config.mouseCheckColor
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        onPressed: {
            if(openUrl!="")
                Qt.openUrlExternally(openUrl)
        }
        onEntered: mouseEnter()
        onExited: mouseExit()
    }
}
