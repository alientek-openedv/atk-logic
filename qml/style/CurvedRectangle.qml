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

import QtQuick 2.13
import QtQuick.Controls 2.13
import "../config"

Rectangle {
    id: root
    property var cornersRadius : [0,0,0,0]
    color : Config.borderBackgroundColor
    width: parent.width
    height: parent.height

    Rectangle{
        id: leftTop
        width: parent.width/2
        height: parent.height/2
        color: root.color
        anchors{
            top: parent.top
            left: parent.left
        }
        visible: cornersRadius[0]>0
    }
    Rectangle{
        id: topRight
        width: parent.width/2
        height: parent.height/2
        color: root.color
        anchors{
            top: parent.top
            right: parent.right
        }
        visible: cornersRadius[1]>0
    }
    Rectangle{
        id: bottomRight
        width: parent.width/2
        height: parent.height/2
        color: root.color
        anchors{
            bottom: parent.bottom
            right: parent.right
        }
        visible: cornersRadius[2]>0
    }
    Rectangle{
        id: leftBottom
        width: parent.width/2
        height: parent.height/2
        color: root.color
        anchors{
            bottom: parent.bottom
            left: parent.left
        }
        visible: cornersRadius[3]>0
    }
}
