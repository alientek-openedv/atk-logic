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

import QtQuick 2.11
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import "../config"
import "../style"

Window {
    signal colorPickerDataSend(var dataJson)
    property var screen
    property var dataJson
    id: window
    visible: true
    modality: Qt.WindowModal
    flags: Qt.Window | Qt.FramelessWindowHint
    color: "#00000000"
    width: dropShadow.width
    height: dropShadow.height

    Component.onCompleted: Config.isSetModel=true;

    Connections{
        target: Signal
        function onEscKeyPress()
        {
            window.close();
        }
    }

    Image {
        id: dropShadow
        width: 370
        height: 457
        source: "../../resource/image/ColorPickerDropShadow.png"
    }

    Rectangle{
        width: 354
        height: 441
        color: Config.backgroundColor
        radius: 10
        anchors.centerIn: parent

        Item{
            id: titleItem
            width: parent.width
            height: 38
            Text {
                text: qsTr("通道颜色设置")
                font.pixelSize: 18
                anchors{
                    left: parent.left
                    leftMargin: 20
                    verticalCenter: parent.verticalCenter
                }
                color: Config.textColor
            }
            ImageButton{
                z: 100
                width: 10
                height: 10
                imageSource: "resource/icon/MessageClose.png"
                imageEnterSource: "resource/icon/MessageClose.png"
                anchors{
                    right: parent.right
                    rightMargin: 10
                    verticalCenter: parent.verticalCenter
                }
                onPressed: window.close()
            }
            MouseArea{
                anchors.fill: parent
                acceptedButtons: Qt.LeftButton 
                property point clickPos: "0,0"
                onPressed: clickPos=Qt.point(mouse.x,mouse.y)
                onPositionChanged: {
                    var delta = Qt.point(mouse.x-clickPos.x, mouse.y-clickPos.y)
                    if(window.x+delta.x<Screen.virtualX)
                        window.setX(Screen.virtualX);
                    else if(window.x+window.width+delta.x>Screen.virtualX+screen.width)
                        window.setX(Screen.virtualX+screen.width-window.width);
                    else
                        window.setX(window.x+delta.x)

                    if(window.y+delta.y<Screen.virtualY)
                        window.setY(Screen.virtualY);
                    else if(window.y+window.height+delta.y>Screen.virtualY+screen.height)
                        window.setY(Screen.virtualY+screen.height-window.height);
                    else
                        window.setY(window.y+delta.y)
                }
            }
        }

        Rectangle{
            id: titleSplit
            height: 1
            width: parent.width
            anchors.top: titleItem.bottom
            color: Config.lineColor
        }

        Column{
            property int select: -1

            signal colorChanged(var index)

            id: contentColumn
            width: parent.width-40
            spacing: 10-colorPicker.cursorWidth/2

            anchors{
                top: titleSplit.bottom
                topMargin: 10
                bottom: contentSplit.top
                bottomMargin: 10
            }

            RowLayout{
                spacing: 10
                anchors{
                    left: parent.left
                    leftMargin: 20
                }
                Flow{
                    id: flow
                    spacing: 5
                    width: 240
                    Repeater{
                        model: dataJson.length
                        Item{
                            property color showColor: dataJson[index]["color"]
                            id: rootItem
                            width: 26-(index===0||index===8?3:0)
                            height: 20

                            Connections{
                                target: contentColumn
                                function onColorChanged(index_)
                                {
                                    if(index_===index && rootItem.showColor!==dataJson[index]["color"])
                                        rootItem.showColor=dataJson[index]["color"];
                                }
                            }
                            TextButton{
                                text: index
                                checkable: true
                                width: 23
                                height: 20
                                anchors.right: parent.right
                                enabled: sConfig.loopState===-1 && !sConfig.isRun
                                textColor: rootItem.showColor
                                textEnterColor: "white"
                                backgroundPressedColor: textColor
                                backgroundDisableColor: Config.lineDisableColor
                                backgroundRectangle{
                                    border{
                                        width: 1
                                        color: enabled?rootItem.showColor:Config.lineDisableColor
                                    }
                                    radius: 4
                                }
                                isCheckedEnable: true
                                isChecked: contentColumn.select===index
                                onIsCheckedChanged: {
                                    if(isChecked)
                                    {
                                        if(colorPicker.currentColor!==rootItem.showColor)
                                            colorPicker.setColor(rootItem.showColor);
                                    }
                                }
                                onClicked: contentColumn.select=index;
                            }
                        }
                    }
                }

                Column{
                    spacing: 5
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    TextButton{
                        textColor: Config.textColor
                        textEnterColor: textColor
                        backgroundColor: Config.setKeysInputBackground
                        backgroundEnterColor: backgroundColor
                        backgroundPressedColor: backgroundColor
                        text: qsTr("默认值")
                        backgroundRectangle.radius: 0
                        height: 20
                        width: 64
                        onPressed: colorPicker.setColor(Config.channelDataColor[contentColumn.select])
                    }
                    TextButton{
                        textColor: Config.textColor
                        textEnterColor: textColor
                        backgroundColor: Config.setKeysInputBackground
                        backgroundEnterColor: backgroundColor
                        backgroundPressedColor: backgroundColor
                        text: qsTr("全部重置")
                        backgroundRectangle.radius: 0
                        height: 20
                        width: 64
                        onPressed: {
                            for(let i=0;i<dataJson.length;i++){
                                if(contentColumn.select===i)
                                    colorPicker.setColor(Config.channelDataColor[i])
                                else{
                                    dataJson[i]["color"]=Config.channelDataColor[i];
                                    contentColumn.colorChanged(i);
                                }
                            }
                        }
                    }
                }
            }

            ColorPicker{
                id: colorPicker
                anchors{
                    left: parent.left
                    leftMargin: 20-cursorWidth/2
                }
                onCurrentColorChanged: {
                    if(contentColumn.select>=0){
                        dataJson[contentColumn.select]["color"]=currentColor;
                        contentColumn.colorChanged(contentColumn.select);
                    }
                }
                Component.onCompleted: contentColumn.select=0;
            }
        }

        Rectangle{
            id: contentSplit
            height: 1
            width: parent.width
            anchors{
                bottom: buttonItem.top
                bottomMargin: 5
            }
            color: Config.lineColor
        }

        Item{
            id: buttonItem
            width: parent.width
            height: 40
            anchors{
                bottom: parent.bottom
                bottomMargin: 5
            }
            Row{
                spacing: 10
                anchors{
                    right: parent.right
                    rightMargin: 20
                    verticalCenter: parent.verticalCenter
                }
                TextButton{
                    text: qsTr("取消")
                    width: 46
                    height: 30
                    onClicked: window.close();
                }

                TextButton{
                    text: qsTr("确定")
                    width: 46
                    height: 30
                    backgroundColor: Config.mouseCheckColor
                    backgroundEnterColor: backgroundColor
                    backgroundPressedColor: backgroundColor
                    textColor: "white"
                    textEnterColor: textColor
                    onPressed: {
                        colorPicker.allEnter()
                        sendData();
                        window.close();
                    }
                }
            }
        }
    }

    function sendData(){
        let j=0;
        for(let i in flow.children)
        {
            if(j<16){
                dataJson[j]["color"]=flow.children[i].showColor;
                j++;
            }
        }
        colorPickerDataSend(dataJson)
    }
}
