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

import QtQuick 2.11
import QtQuick.Controls 2.5
import "../config"

Popup {
    property string showText
    property string hotkey
    property bool isShow: false
    property int direction: 1
    property bool autoSetIsShow: false
    property int showTextPixelSize: 14
    property alias verticalCenterOffset: show.anchors.verticalCenterOffset
    id: control
    x: (direction===1||direction===3)?direction===1?-contentWidth-5:parent.width+5:parent.width/2-contentWidth/2
    y: (direction===1||direction===3)?parent.height/2-contentHeight/2:direction===2?-contentHeight-5:parent.height+5
    width: contentWidth
    height: contentHeight
    contentWidth: root.implicitWidth
    contentHeight: root.implicitHeight
    onIsShowChanged: {
        if(isShow)
            open();
        else
            close();
    }

    onOpenedChanged: {
        if(autoSetIsShow&&opened!==isShow)
            isShow=opened;
    }

    background: Item {
        id: root
        implicitHeight: 28+((direction===0||direction===2)?canvasTop.height:0)
        implicitWidth: show.contentWidth+(hotkeyText.contentWidth==0?0:hotkeyText.contentWidth)+20+((direction===1||direction===3)?canvasLeft.width:0)
        Rectangle{
            id: mainRectangle
            color: Config.backgroundColor
            radius: 4
            z: 1
            anchors{
                top: parent.top
                topMargin: direction===0?5:0
                right: parent.right
                rightMargin: direction===1?5:0
                bottom: parent.bottom
                bottomMargin: direction===2?5:0
                left: parent.left
                leftMargin: direction===3?5:0
            }

            Text {
                id: show
                text: showText
                font.pixelSize: showTextPixelSize
                color: Config.textColor
                anchors{
                    left: parent.left
                    leftMargin: 10
                    verticalCenter: parent.verticalCenter
                    verticalCenterOffset: -1
                }
            }
            Text {
                id: hotkeyText
                text: hotkey==""?"":"（"+hotkey+"）"
                font.pixelSize: 12
                color: Config.subheadColor
                anchors{
                    right: parent.right
                    rightMargin: 10
                    verticalCenter: parent.verticalCenter
                }
            }
            border{
                width: 1
                color: Config.lineColor
            }
        }
        Canvas{
            property int drawWidth: 12
            id: canvasTop
            width: parent.width
            z: 10
            height: 6
            visible: direction===0
            anchors{
                left: parent.left
                bottom: mainRectangle.top
                bottomMargin: -1
            }
            onPaint: {
                var ctx = getContext("2d");
                ctx.lineWidth = 0
                ctx.strokeStyle = mainRectangle.border.color
                ctx.fillStyle = mainRectangle.color
                ctx.beginPath();
                ctx.moveTo(canvasTop.width/2-drawWidth/2+1, canvasTop.height)
                ctx.lineTo(canvasTop.width/2, 1);
                ctx.lineTo(canvasTop.width/2+drawWidth/2-1, canvasTop.height);
                ctx.closePath();
                ctx.fill()

                ctx.beginPath();
                ctx.lineWidth = 1
                ctx.moveTo(canvasTop.width/2-drawWidth/2, canvasTop.height)
                ctx.lineTo(canvasTop.width/2, 0);
                ctx.lineTo(canvasTop.width/2+drawWidth/2, canvasTop.height);
                ctx.stroke();
            }
        }
        Canvas{
            property int drawHeight: 12
            id: canvasRight
            width: 6
            z: 10
            height: parent.height
            visible: direction===1
            anchors{
                top: parent.top
                left: mainRectangle.right
                leftMargin: -1
            }
            onPaint: {
                var ctx = getContext("2d");
                ctx.lineWidth = 0
                ctx.strokeStyle = mainRectangle.border.color
                ctx.fillStyle = mainRectangle.color
                ctx.beginPath();
                ctx.moveTo(0, canvasRight.height/2-drawHeight/2+1)
                ctx.lineTo(canvasRight.width-1, canvasRight.height/2);
                ctx.lineTo(0, canvasRight.height/2+drawHeight/2-1);
                ctx.closePath();
                ctx.fill()

                ctx.beginPath();
                ctx.lineWidth = 1
                ctx.moveTo(0, canvasRight.height/2-drawHeight/2)
                ctx.lineTo(canvasRight.width, canvasRight.height/2);
                ctx.lineTo(0, canvasRight.height/2+drawHeight/2);
                ctx.stroke();
            }
        }
        Canvas{
            property int drawWidth: 12
            id: canvasBottom
            width: parent.width
            z: 10
            height: 6
            visible: direction===2
            anchors{
                left: parent.left
                top: mainRectangle.bottom
                topMargin: -1
            }
            onPaint: {
                var ctx = getContext("2d");
                ctx.lineWidth = 0
                ctx.strokeStyle = mainRectangle.border.color
                ctx.fillStyle = mainRectangle.color
                ctx.beginPath();
                ctx.moveTo(canvasBottom.width/2-drawWidth/2+1, 0)
                ctx.lineTo(canvasBottom.width/2, canvasBottom.height-1);
                ctx.lineTo(canvasBottom.width/2+drawWidth/2-1, 0);
                ctx.closePath();
                ctx.fill()

                ctx.beginPath();
                ctx.lineWidth = 1
                ctx.moveTo(canvasBottom.width/2-drawWidth/2, 0)
                ctx.lineTo(canvasBottom.width/2, canvasBottom.height);
                ctx.lineTo(canvasBottom.width/2+drawWidth/2, 0);
                ctx.stroke();
            }
        }
        Canvas{
            property int drawHeight: 12
            id: canvasLeft
            width: 6
            z: 10
            height: parent.height
            visible: direction===3
            anchors{
                top: parent.top
                right: mainRectangle.left
                rightMargin: -1
            }
            onPaint: {
                var ctx = getContext("2d");
                ctx.lineWidth = 0
                ctx.strokeStyle = mainRectangle.border.color
                ctx.fillStyle = mainRectangle.color
                ctx.beginPath();
                ctx.moveTo(canvasLeft.width, canvasLeft.height/2-drawHeight/2+1)
                ctx.lineTo(1, canvasLeft.height/2);
                ctx.lineTo(canvasLeft.width, canvasLeft.height/2+drawHeight/2-1);
                ctx.closePath();
                ctx.fill()

                ctx.beginPath();
                ctx.lineWidth = 1
                ctx.moveTo(canvasLeft.width, canvasLeft.height/2-drawHeight/2)
                ctx.lineTo(0, canvasLeft.height/2);
                ctx.lineTo(canvasLeft.width, canvasLeft.height/2+drawHeight/2);
                ctx.stroke();
            }
        }
    }
}
