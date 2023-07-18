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

Rectangle{
    property alias showText: inputText.text
    property bool editable: true
    property bool disable: false
    property color backgroundColor: "transparent"
    property color backgroundPressedColor: "transparent"
    property color backgroundMouseEnterColor: "transparent"
    property alias isFocus: inputText.focus
    property bool isInput: true

    implicitHeight: 26
    implicitWidth: 240
    border{
        width: 1
        color: disable?Config.lineDisableColor:Config.borderLineColor
    }
    clip: true
    radius: 4
    color: Config.borderBackgroundColor

    FontLoader{
        id: fontLoader
        source: "../../resource/SourceCodePro-Medium.ttf"
    }

    TextEdit {
        property string inputTextRecode
        id: inputText
        clip: true
        anchors{
            verticalCenter: parent.verticalCenter
            left: parent.left
            leftMargin: 5
            right: parent.right
            rightMargin: 5
        }
        font{
            family: fontLoader.name
            pixelSize: 12
            letterSpacing: 7.2
        }
        color: disable?Config.textDisabledColor:Config.textColor
        selectionColor: Config.mouseCheckColor
        readOnly: !editable
        enabled: !disable
        selectByMouse: false
        Keys.enabled: true
        Keys.onPressed: {
            if(event.key===Qt.Key_Return || event.key===Qt.Key_Enter){
                focus=false
            }else if(event.key===Qt.Key_X || event.key===Qt.Key_0 || event.key===Qt.Key_1 || event.key===Qt.Key_Delete ||
                     event.key===Qt.Key_R || event.key===Qt.Key_F || event.key===Qt.Key_C || event.key===Qt.Key_Backspace){
                var str="";
                switch(event.key){
                case Qt.Key_0:
                    str="0"
                    break;
                case Qt.Key_1:
                    str="1"
                    break;
                case Qt.Key_R:
                    str="R"
                    break;
                case Qt.Key_F:
                    str="F"
                    break;
                case Qt.Key_C:
                    str="C"
                    break;
                default:
                    str="X"
                    break;
                }
                var temp=selectionStart;
                if(temp>=0){
                    isInput=true;
                    remove(temp,temp+1);
                    if(text.length<=15){
                        isInput=true;
                        insert(temp,str);
                    }
                    if(event.key===Qt.Key_Backspace && temp>0)
                        cursorPosition=temp;
                    else if(event.key===Qt.Key_Delete)
                        cursorPosition=temp;
                }
            }else if(event.key===Qt.Key_Left){
                if(selectionStart>0)
                    cursorPosition--;
            }else if(event.key===Qt.Key_Right){
                if(selectionStart<16)
                    cursorPosition++;
            }
            event.accepted = true;
        }
        onTextChanged: {
            if(text!==text.toUpperCase())
            {
                text=text.toUpperCase()
                return;
            }
            if(!isInput && text!==inputTextRecode){
                text=inputTextRecode;
            }else
            {
                isInput=false;
                inputTextRecode=text;
            }
        }

        onSelectedTextChanged: {
            if(selectedText==="")
                select(cursorPosition-1,cursorPosition);
        }

        onCursorPositionChanged: {
            select(cursorPosition-1,cursorPosition);
        }

        onSelectionStartChanged: {
            if(selectionStart>16)
                cursorPosition=16;
            else if(selectionStart<0)
                cursorPosition=0;

        }

        Component.onCompleted: {
            if(text=="")
                text="XXXXXXXXXXXXXXXX";
        }

        MouseArea{
            anchors.fill: parent
            cursorShape: editable ? Qt.IBeamCursor : Qt.ArrowCursor
            acceptedButtons: Qt.NoButton
        }
    }

    function reset(){
        isInput=true;
        inputText.text="XXXXXXXXXXXXXXXX";
    }
}
