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

Text{
    property string openUrl: ""
    property alias fontSize: rootText.font.pixelSize
    signal mouseEnter
    signal mouseExit
    id: rootText
    text: showText
    font{
        pixelSize: 12
        underline: true
    }
    color: mouseArea.containsMouse?Config.red:Config.mouseCheckColor
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        onPressed: {
            if(openUrl!="")
                Qt.openUrlExternally(openUrl)
        }
        onEntered: mouseEnter()
        onExited: mouseExit()
    }
}
