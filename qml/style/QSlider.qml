/**
 ****************************************************************************************************
 * @author      正点原子团队(ALIENTEK)
 * @date        2023-07-18
 * @license     Copyright (c) 2023-2035, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:zhengdianyuanzi.tmall.com
 *
 ****************************************************************************************************
 */

import QtQuick 2.15
import QtQuick.Controls 2.5
import "../config"

Slider {
    property string postfixText: ""
    property int decimalDigits: 0
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
            width: parent.width-10
            height: 1
            anchors.centerIn: parent
            color: Config.iceBlue
            Repeater{
                model: 11
                Rectangle{
                    width: 1
                    height: 3
                    x: (parent.width-handleRectangle.width)/(to-from)*index*((to-from)*0.1)+handleRectangle.width/2
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
