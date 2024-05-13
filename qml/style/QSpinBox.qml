import QtQuick 2.15
import QtQuick.Controls 2.5
import "../config"

Rectangle{
    property real from: 0
    property real to: 0
    property alias value: inputText.text
    property int decimalDigits: 0
    property bool editable: true
    property bool disable: false
    property bool wheelChanged: Setting.componentWheelChanged
    property string postfixText: ""
    property color backgroundColor: Config.borderBackgroundColor
    property color backgroundPressedColor: Config.mouseEnterColor
    property color backgroundMouseEnterColor: Config.mouseCheckColor
    property alias backgroundMouseArea: rootMouseArea
    property alias containsMouse: rootMouseArea.containsMouse
    property alias inputActiveFocus: inputText.activeFocus

    implicitHeight: 30
    implicitWidth: 50
    border{
        width: 1
        color: rootMouseArea.containsMouse&&wheelChanged&&!disable?Config.mouseCheckColor:disable?Config.lineDisableColor:Config.borderLineColor
    }
    clip: true
    radius: 4
    color: backgroundColor

    Component.onCompleted: calculate()

    onBackgroundColorChanged: mouseEnter();

    onBackgroundMouseEnterColorChanged: mouseEnter();

    onDisableChanged: mouseEnter();

    onToChanged: calculate();

    onFromChanged: calculate();

    onActiveFocusChanged: calculate();

    MouseArea{
        id: rootMouseArea
        anchors.fill: parent
        hoverEnabled: true
        acceptedButtons: Qt.NoButton
    }

    TextInput {
        id: inputText
        clip: true
        anchors{
            verticalCenter: parent.verticalCenter
            left: parent.left
            leftMargin: 5
            right: showText.left
            rightMargin: 2
        }
        font.pixelSize: 12
        readOnly: !editable
        enabled: !disable
        selectByMouse: true
        selectionColor: Config.mouseCheckColor
        color: disable?Config.textDisabledColor:Config.textColor
        validator: DoubleValidator{
            bottom: from
            top: to
            decimals: decimalDigits
            notation: DoubleValidator.StandardNotation
        }
        MouseArea{
            anchors.fill: parent
            cursorShape: editable ? Qt.IBeamCursor : Qt.ArrowCursor
            acceptedButtons: Qt.NoButton
            enabled: !disable
            onWheel: {
                if(wheelChanged)
                {
                    if(wheel.angleDelta.y>0)
                        decrease();
                    else
                        increase();
                }
            }
        }
        onActiveFocusChanged: {
            if(!activeFocus)
                calculate();
        }
        Keys.onPressed: {
            if(event.key===Qt.Key_Return || event.key===Qt.Key_Enter){
                focus=false
            }
        }
    }

    Column{
        spacing: 1
        anchors{
            verticalCenter: parent.verticalCenter
            right: parent.right
            rightMargin: 4
        }
        Button {
            clip: true
            Image{
                width: 9
                height: 6
                fillMode: Image.PreserveAspectFit
                source: disable?"../../resource/icon/"+Config.tp+"/PullDownDisable.png":"../../resource/icon/"+Config.tp+"/PullDown.png"
                rotation: 180
                anchors{
                    bottom: parent.bottom
                    horizontalCenter: parent.horizontalCenter
                }
            }
            width: 11
            height: 8
            enabled: !disable
            background: Rectangle{
                id: upBackgroundRectangle
                color: backgroundColor
                Behavior on color{
                    ColorAnimation {
                        duration: 100
                    }
                }
            }
            MouseArea{
                id: buttonUpMouseArea
                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor
                acceptedButtons: Qt.NoButton
                onEntered: upBackgroundRectangle.color = backgroundMouseEnterColor
                onExited:  upBackgroundRectangle.color = backgroundColor
            }
            onDownChanged: upBackgroundRectangle.color = down?backgroundPressedColor:buttonUpMouseArea.containsMouse?backgroundMouseEnterColor:backgroundColor
            onPressed: decrease()
        }
        Button {
            clip: true
            Image{
                width: 9
                height: 6
                fillMode: Image.PreserveAspectFit
                source: disable?"../../resource/icon/"+Config.tp+"/PullDownDisable.png":"../../resource/icon/"+Config.tp+"/PullDown.png"
                anchors{
                    top: parent.top
                    horizontalCenter: parent.horizontalCenter
                }
            }
            width: 11
            height: 8
            enabled: !disable
            background: Rectangle{
                id: downBackgroundRectangle
                color: backgroundColor
                Behavior on color{
                    ColorAnimation {
                        duration: 100
                    }
                }
            }
            MouseArea{
                id: buttonDownMouseArea
                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor
                acceptedButtons: Qt.NoButton
                onEntered: downBackgroundRectangle.color = backgroundMouseEnterColor
                onExited:  downBackgroundRectangle.color = backgroundColor
            }
            onDownChanged: downBackgroundRectangle.color = down?backgroundPressedColor:buttonDownMouseArea.containsMouse?backgroundMouseEnterColor:backgroundColor
            onPressed: increase()
        }
    }

    Text {
        id: showText
        text: postfixText
        width: noShowText.contentWidth
        anchors{
            verticalCenter: parent.verticalCenter
            right: parent.right
            rightMargin: 17
        }
        color: inputText.color
    }

    Text {
        id: noShowText
        text: postfixText
        visible: false
    }

    function mouseEnter(){
        upBackgroundRectangle.color = buttonUpMouseArea.containsMouse?backgroundMouseEnterColor:backgroundColor
        downBackgroundRectangle.color = buttonDownMouseArea.containsMouse?backgroundMouseEnterColor:backgroundColor
    }

    function decrease(){
        var office=1/Math.pow(10,parseInt(decimalDigits));
        var temp=(parseFloat(value)+office).toFixed(decimalDigits);
        if(temp>to)
            value=to.toFixed(decimalDigits);
        else
            value=temp;
    }

    function increase(){
        var office=1/Math.pow(10,parseInt(decimalDigits));
        var temp=(parseFloat(value)-office).toFixed(decimalDigits);
        if(temp<from)
            value=from.toFixed(decimalDigits);
        else
            value=temp;
    }

    function calculate(){
        if(value>to)
            value=to.toFixed(decimalDigits);
        else if(value<from)
            value=from.toFixed(decimalDigits);
        else
            value=parseFloat(value).toFixed(decimalDigits);
        if(value==="NaN")
            value=((to-Math.abs(from))/2).toFixed(decimalDigits);
    }
}

