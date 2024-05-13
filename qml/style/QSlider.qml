import QtQuick 2.15
import QtQuick.Controls 2.5
import "../config"

Slider {
    property string postfixText: ""
    property int decimalDigits: 0
    property int splice: 10
    property bool isShowText: true
    property alias handleX: handleItem.x
    property bool wheelChanged: Setting.componentWheelChanged

    id: rootSlider
    value: 0
    snapMode: Slider.SnapAlways

    onMoved: align()

    background: Item {
        anchors.fill: parent
        Rectangle {
            width: parent.width-handleRectangle.width*2
            height: 1
            anchors.centerIn: parent
            color: Config.iceBlue
            Repeater{
                model: splice + 1
                Rectangle{
                    width: 1
                    height: 3
                    x: (parent.width-1)/(to-from)*index*((to-from)/splice)
                    y: -height
                    color: Config.iceBlue
                }
            }
        }
    }

    handle: Item{
        id: handleItem
        x: rootSlider.visualPosition * (rootSlider.width - width*2) + width/2
        y: rootSlider.topPadding + rootSlider.availableHeight / 2 - height / 2
        height: 8
        width: 8
        Rectangle {
            id: handleRectangle
            implicitWidth: 8
            implicitHeight: 8
            radius: 10
            color: enabled ? Config.mouseCheckColor : Config.textDisabledColor
        }
        Text {
            id: showText
            visible: isShowText
            text: rootSlider.value.toFixed(decimalDigits)+postfixText
            font.pixelSize: 12
            color: handleRectangle.color
            anchors{
                top: handleRectangle.bottom
                horizontalCenter: parent.horizontalCenter
                horizontalCenterOffset: 1
                topMargin: 2
            }
        }
    }

    MouseArea{
        anchors.fill: parent
        acceptedButtons: Qt.NoButton
        onWheel: {
            if(wheelChanged)
                wheelChangedSelect(wheel.angleDelta.y<0)
        }
    }

    function wheelChangedSelect(isUp){
        var office=1.0;
        if(decimalDigits>0)
            office=1.0/decimalDigits;
        if(isUp){
            if(value+office<=to)
                value+=office;
        }else{
            if(value-office>=from)
                value-=office;
        }
    }

    function align(){
        value = parseFloat(value.toFixed(decimalDigits))
    }
}
