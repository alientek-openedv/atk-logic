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
        fillMode: Image.Pad
    }

    MouseArea{
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
        acceptedButtons: Qt.NoButton
    }
}
