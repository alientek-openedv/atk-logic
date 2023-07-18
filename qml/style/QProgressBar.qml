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

Item{
    property int value: 0
    property color loadingBackground: Config.iceBlue
    property color loadingTextColor: Config.textColor
    property bool isSlender: false
    id: progressItem
    implicitWidth: 120
    implicitHeight: isSlender?8:14
    clip: true
    Rectangle{
        id: showRectangle
        anchors.fill: parent
        border{
            width: 1
            color: Config.lineColor
        }
        color: "transparent"
        radius: 7
        Rectangle{
            anchors{
                top: parent.top
                left: parent.left
                right: parent.right
                bottom: parent.bottom
                margins: 1
            }
            color: "#F4F4F4"
            radius: 5
        }
    }
    Rectangle{
        anchors{
            top: parent.top
            left: parent.left
            bottom: parent.bottom
        }
        width: progressItem.width/100*value
        color: loadingBackground
        radius: width<showRectangle.radius?width:showRectangle.radius
        Behavior on width{
            NumberAnimation {
                duration: 100
            }
        }
    }
    Text {
        visible: !isSlender
        text: value+"%"
        anchors.centerIn: parent
        color: loadingTextColor
        font.pixelSize: 10
    }
}
