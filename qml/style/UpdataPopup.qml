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
import QtQuick.Controls 2.15
import "../config"

Rectangle {
    property var json
    property string sText
    property string url
    property bool isUpdate
    property int isHardwareUpdate
    id: rootControl
    color: "#00000000"
    onVisibleChanged: {
        if(visible)
        {
            if(isHardwareUpdate===2){
                titleText.text=qsTr("设备连接失败");
                showEdit.showText=qsTr("设备连接失败，可强制升级固件。");
                subRow.visible=false;
            }else{
                var firstVersion="",showText='<style type="text/css">body{line-height:1.2}</style><body>';
                for(var i=0;i<json["count"];i++){
                    if(firstVersion==="")
                        firstVersion="V"+json["version"][i];
                    showText+='<p><font color="#808080">v'+json["version"][i]+"&nbsp;&nbsp;"+json["date"][i]+'</font><br/>';
                    showText+='<U>'+json["text"][i].toString().replace(new RegExp("\r\n", "g"),"<br/>")+'</U>';
                    if(i+1<json["count"])
                        showText+="<br/>";
                }
                showText+="</body>";
                showEdit.showText=showText;
                let str=qsTr("最新版本");
                if(isHardwareUpdate==1)
                {
                    titleText.text=qsTr("固件版本")+" "+firstVersion+str;
                    subRow.visible=true;
                    if(typeof(url)!=="undefined")
                        updateButton.url=url.split('\n');
                }
                else
                {
                    titleText.text="LogicView "+firstVersion+str;
                    subRow.visible=false;
                }
            }
        }
    }

    Connections{
        target: Signal
        function onEscKeyPress()
        {
            rootControl.visible=false
        }
    }

    MouseArea{
        anchors.fill: parent
        preventStealing: true
        propagateComposedEvents: false
        hoverEnabled: true
        acceptedButtons: Qt.AllButtons
        onWheel: wheel.acctped=true
    }

    Image {
        width: 556
        height: 303
        source: "../../resource/image/UpdatePopupDropShadow.png"
        anchors.centerIn: parent
        Rectangle{
            width: 540
            height: 287
            radius: 4
            color: Config.backgroundColor
            anchors{
                top: parent.top
                topMargin: 6
                horizontalCenter: parent.horizontalCenter
            }
            ImageButton{
                width: 10
                height: 10
                imageSource: "resource/icon/MessageClose.png"
                imageEnterSource: "resource/icon/MessageClose.png"
                anchors{
                    right: parent.right
                    rightMargin: 15
                    top: parent.top
                    topMargin: 15
                }
                onPressed: rootControl.visible=false
            }
            Item{
                anchors{
                    top: parent.top
                    topMargin: 30
                    left: parent.left
                    leftMargin: 20
                    right: parent.right
                    rightMargin: 20
                    bottom: parent.bottom
                    bottomMargin: 30
                }
                Text {
                    id: titleText
                    font.pixelSize: 30
                    color: Config.textColor
                    width: contentWidth
                    height: 37
                }
                Row{
                    id: subRow
                    anchors{
                        left: titleText.left
                        top: titleText.bottom
                    }
                    width: parent.width
                    height: subText.height
                    Text {
                        id: subText
                        font.pixelSize: 12
                        color: Config.subheadColor
                        width: contentWidth
                        height: 13
                        text: sText
                    }
                }
                TextButton{
                    property var url
                    id: updateButton
                    visible: isUpdate
                    textColor: "white"
                    textEnterColor: textColor
                    backgroundColor: showEdit.textColor
                    backgroundEnterColor: backgroundColor
                    backgroundPressedColor: backgroundColor
                    width: noShowText.contentWidth+10
                    height: 24
                    text: qsTr("更新")
                    textFontSize: 14
                    anchors{
                        left: titleText.right
                        leftMargin: 10
                        bottom: titleText.bottom
                    }
                    onPressed: {
                        if(isHardwareUpdate!==0){
                            if(isHardwareUpdate===1)
                                root.startUpdate(updateButton.url[0],updateButton.url[1]);
                            else
                                root.enterBootloader();
                        }else
                            Qt.openUrlExternally(rootControl.url)
                    }

                    Text {
                        id: noShowText
                        font.pixelSize: updateButton.textFontSize
                        text: updateButton.text
                        visible: false
                    }
                }
                Text {
                    text: qsTr("已是最新版本")
                    font{
                        pixelSize: 14
                        underline: true
                    }
                    color: showEdit.textColor
                    visible: !updateButton.visible
                    anchors{
                        left: titleText.right
                        leftMargin: 10
                        bottom: titleText.bottom
                    }
                }
                QTextEdit{
                    id: showEdit
                    width: parent.width
                    height: parent.height-titleText.height-subRow.height-10
                    textColor: Config.mouseCheckColor
                    textFormat: TextEdit.RichText
                    ScrollBar.vertical: vbar
                    anchors{
                        top: subRow.bottom
                        topMargin: 10
                    }
                    fontSize: 16
                    note: ""
                    readOnly: true
                }
                QScrollBar {
                    id: vbar
                    height: showEdit.height
                    anchors{
                        top: showEdit.top
                        right: parent.right
                        rightMargin: -20
                    }
                }
            }
        }
    }
}
