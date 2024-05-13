import QtQuick 2.15
import QtQuick.Controls 2.5
import "../config"

Button {
    property string buttonID
    width: 13
    height: 20
    checkable: true
    focusPolicy: Qt.NoFocus
    background: Item{}
    Image{
        id: imageRoot
        width: parent.width
        height: parent.height
        anchors{
            verticalCenter: parent.verticalCenter
            horizontalCenter: parent.horizontalCenter
        }
        source: "../../resource/icon/"+(checked?"VernierButtonChecked":"VernierButton")+buttonID+".png"
        fillMode: Image.PreserveAspectFit
    }

    MouseArea{
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
        acceptedButtons: Qt.NoButton
    }
}
