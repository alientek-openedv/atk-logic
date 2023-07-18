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
    property color backgroundColor: "black"
    id: rootButton
    height: 13
    width: 13
    focusPolicy: Qt.NoFocus
    checkable: true
    Image {
        anchors.centerIn: parent
        source: "../../resource/icon/CheckWrite.png"
        visible: checked
    }
    background: Rectangle {
        color: backgroundColor
        radius: 4
    }
    MouseArea{
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
        onPressed: checked=true;
    }
}
