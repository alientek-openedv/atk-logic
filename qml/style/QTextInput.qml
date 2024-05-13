import QtQuick 2.15
import QtQuick.Controls 2.5
import "../config"

Rectangle{
    property alias showText: inputText.text
    property bool editable: true
    property bool disable: false
    property bool keysIntercept: false
    property color backgroundColor: Config.borderBackgroundColor
    property color backgroundBorderColor: Config.borderLineColor
    property alias isFocus: inputText.activeFocus
    property alias fontSize: inputText.font.pixelSize
    property alias textColor: inputText.color
    property alias textInput: inputText
    property alias containsMouse: mouseAreaRoot.containsMouse
    property bool showBorder: true
    property string noteText: ""

    signal keyPressed(var event)
    signal keyReleased(var event)

    implicitHeight: 30
    implicitWidth: 50
    border{
        width: showBorder?1:0
        color: disable?Config.lineDisableColor:backgroundBorderColor
    }
    clip: true
    radius: 4
    color: backgroundColor

    TextInput {
        id: inputText
        clip: true
        anchors{
            verticalCenter: parent.verticalCenter
            left: parent.left
            leftMargin: 5
            right: parent.right
            rightMargin: 5
        }
        font.pixelSize: 12
        readOnly: !editable
        text: noteText
        enabled: !disable
        selectByMouse: true
        selectionColor: Config.mouseCheckColor
        color: disable?Config.textDisabledColor:Config.textColor
        Keys.enabled: true
        Keys.onPressed: {
            if(keysIntercept)
                event.accepted=true;
            if(event.key===Qt.Key_Return || event.key===Qt.Key_Enter){
                focus=false
                return;
            }
            keyPressed(event);
        }
        Keys.onReleased: {
            if(event.key===Qt.Key_Return || event.key===Qt.Key_Enter){
                focus=false
                return;
            }
            keyReleased(event);
        }

        Text{
            anchors.fill: parent
            visible: !inputText.activeFocus&&inputText.text===""
            text: noteText
            color: inputText.color
        }
        MouseArea{
            id: mouseAreaRoot
            anchors.fill: parent
            cursorShape: editable ? Qt.IBeamCursor : Qt.ArrowCursor
            acceptedButtons: Qt.NoButton
            hoverEnabled: true
        }
    }
}

