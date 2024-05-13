import QtQuick 2.15
import "../config"

Flickable {
    property string note: qsTr("注释")
    property color noteColor: Config.textDisabledColor
    property color textColor: Config.textColor
    property bool canSelect: false
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
        selectByMouse: canSelect
        selectionColor: Config.mouseCheckColor
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
