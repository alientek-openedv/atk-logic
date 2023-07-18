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
import "../config"

Flickable {
    property string note: qsTr("注释")
    property color noteColor: Config.textDisabledColor
    property color textColor: Config.textColor
    property alias fontSize: noteTextEdit.font.pixelSize
    property alias showText: noteTextEdit.text
    property alias readOnly: noteTextEdit.readOnly
    property alias textFormat: noteTextEdit.textFormat
    property alias textFocus: noteTextEdit.activeFocus

    id: flick
    width: parent.width
    height: 18
    contentWidth: width
    contentHeight: noteTextEdit.height
    flickableDirection: Flickable.AutoFlickDirection
    boundsBehavior: Flickable.StopAtBounds
    clip: true
    function ensureVisible(r)
    {
        if (contentX >= r.x)
            contentX = r.x;
        else if (contentX+width <= r.x+r.width)
            contentX = r.x+r.width-width;
        if (contentY >= r.y)
            contentY = r.y;
        else if (contentY+height <= r.y+r.height)
            contentY = r.y+r.height-height;
    }

    TextEdit{
        id: noteTextEdit
        font.pixelSize: 12
        width: parent.width
        height: paintedHeight
        color: textColor
        textFormat: TextEdit.PlainText
        wrapMode: TextEdit.WordWrap
        onCursorRectangleChanged: flick.ensureVisible(cursorRectangle)
        Text{
            anchors.fill: parent
            visible: !noteTextEdit.activeFocus&&noteTextEdit.text===""
            font.pixelSize: noteTextEdit.font.pixelSize
            text: note
            color: noteColor
        }
    }
}
