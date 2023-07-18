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

Button{
    property string showText
    id: rootButton
    height: 38
    checkable: true
    focusPolicy: Qt.NoFocus
    contentItem: Item {
        height: rootButton.height
        width: rootButton.width
        Text {
            id: rootText
            text: showText
            font.pixelSize: 16
            anchors.centerIn: parent
            color: enabled?Config.textColor:Config.textDisabledColor
        }
    }
    background: Rectangle {
        color: "transparent"
        Rectangle{
            height: 3
            anchors{
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }
            color: Config.mouseCheckColor
            visible: checked
            z: 100
        }
        Rectangle{
            height: 1
            anchors{
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }
            color: enabled?Config.lineColor:Config.textDisabledColor
            z: 1
        }
    }
}
