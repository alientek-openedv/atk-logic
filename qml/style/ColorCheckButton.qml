import QtQuick 2.15
import QtQuick.Controls 2.5
import "../config"

Button{
    property color backgroundColor: "black"
    id: rootButton
    height: 13
    width: 13
    focusPolicy: Qt.NoFocus
    checkable: true
    Image {
        anchors.centerIn: parent
        width: 13
        height: 10
        fillMode: Image.PreserveAspectFit
        source: "../../resource/icon/CheckWrite.png"
        visible: checked
    }
    background: Rectangle {
        color: backgroundColor
        radius: 4
    }
    MouseArea{
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
        onPressed: checked=true;
    }
}
