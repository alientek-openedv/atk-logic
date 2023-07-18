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

Item {
    property bool isCheck: false
    property string buttonText
    property bool disable: false
    property color backgroundColor: Config.background2Color
    property alias fontPixelSize: showText.font.pixelSize
    property alias textVerticalCenterOffset: showText.anchors.verticalCenterOffset
    property bool autoCheck: true
    signal clicked_();

    implicitHeight: 14
    implicitWidth: 60
    Button {
        id: checkButton
        clip: true
        width: 11
        height: 11
        enabled: !disable
        focusPolicy: Qt.NoFocus
        anchors{
            top: parent.top
            topMargin: 4
        }
        Image{
            visible: isCheck
            source: "../../resource/image/check.png"
            anchors{
                bottom: parent.bottom
                horizontalCenter: parent.horizontalCenter
            }
        }
        background: Rectangle{
            id: backgroundRectangle
            color: disable ? Config.lineDisableColor : isCheck ? Config.mouseCheckColor : backgroundColor
            radius: 2
            border{
                width: isCheck ? 0:1
                color: disable ? Config.lineDisableColor : Config.iceBlue
            }
            Behavior on color{
                ColorAnimation {
                    duration: 100
                }
            }
        }
        onPressed: {
            if(autoCheck)
                isCheck=!isCheck
            else
                clicked_();
        }
    }
    Text {
        id: showText
        text: buttonText
        color: disable ? Config.textDisabledColor : Config.textColor
        height: parent.height
        width: contentWidth
        anchors{
            left: checkButton.right
            leftMargin: 5
            verticalCenter: parent.verticalCenter
        }
        MouseArea{
            anchors.fill: parent
            enabled: !disable
            onPressed: {
                if(autoCheck)
                    isCheck=!isCheck
                else
                    clicked_();
            }
        }
    }
}
