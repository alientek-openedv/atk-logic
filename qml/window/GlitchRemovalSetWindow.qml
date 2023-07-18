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
import "../config"
import "../style"

Window {
    signal dataSend(var dataJson)
    property var screen
    property var dataJson
    id: window
    visible: true
    modality: Qt.WindowModal
    flags: Qt.Window | Qt.FramelessWindowHint
    color: "#00000000"
    width: dropShadow.width
    height: dropShadow.height

    Connections{
        target: Signal
        function onEscKeyPress()
        {
            window.close();
        }
    }

    Component.onCompleted: Config.isSetModel=true;

    Image {
        id: dropShadow
        width: 442
        height: 473
        source: "../../resource/image/GlitchRemovalSetWindowDropShadow.png"
    }

    Rectangle{
        width: 426
        height: 458
        color: Config.backgroundColor
        radius: 10
        anchors.centerIn: parent

        Item{
            id: titleItem
            property string periodText: qsTr("采样周期")
            width: parent.width
            height: 38
            Text {
                text: qsTr("毛刺过滤设置")
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

        Row{
            id: contentColumn
            width: parent.width-40
            spacing: 30
            anchors{
                top: titleSplit.bottom
                topMargin: 10
                bottom: contentSplit.top
                bottomMargin: 10
                left: parent.left
                leftMargin: 20
            }

            Column{
                spacing: 10
                Repeater{
                    model: 8
                    Row{
                        spacing: 5
                        QCheckBox{
                            id: checkLeftBox
                            width: 52
                            buttonText: "Ch"+index
                            fontPixelSize: 14
                            anchors.verticalCenter: parent.verticalCenter
                            textVerticalCenterOffset: -2
                            isCheck: dataJson[index]["enable"]
                            onIsCheckChanged: dataJson[index]["enable"]=isCheck;
                        }
                        Text {
                            text: "≤"
                            color: checkLeftBox.isCheck?Config.textColor:Config.textDisabledColor
                            anchors.verticalCenter: parent.verticalCenter
                            font.pixelSize: 14
                        }
                        QSpinBox{
                            width: 45
                            height: 30
                            from: 1
                            to: 999
                            disable: !checkLeftBox.isCheck
                            value: dataJson[index]["num"]
                            onValueChanged: dataJson[index]["num"]=parseInt(value);
                        }
                        Text {
                            text: titleItem.periodText
                            color: checkLeftBox.isCheck?Config.textColor:Config.textDisabledColor
                            anchors.verticalCenter: parent.verticalCenter
                            font.pixelSize: 14
                        }
                    }
                }
            }
            Column{
                spacing: 10
                Repeater{
                    model: 8
                    Row{
                        spacing: 5
                        QCheckBox{
                            id: checkRightBox
                            width: 52
                            buttonText: "Ch"+(index+8)
                            fontPixelSize: 14
                            anchors.verticalCenter: parent.verticalCenter
                            textVerticalCenterOffset: -2
                            isCheck: dataJson[index+8]["enable"]
                            onIsCheckChanged: dataJson[index+8]["enable"]=isCheck;
                        }
                        Text {
                            text: "≤"
                            color: checkRightBox.isCheck?Config.textColor:Config.textDisabledColor
                            anchors.verticalCenter: parent.verticalCenter
                            font.pixelSize: 14
                        }
                        QSpinBox{
                            width: 45
                            height: 30
                            from: 1
                            to: 999
                            disable: !checkRightBox.isCheck
                            value: dataJson[index+8]["num"]
                            onValueChanged: dataJson[index+8]["num"]=parseInt(value);
                        }
                        Text {
                            text: titleItem.periodText
                            color: checkRightBox.isCheck?Config.textColor:Config.textDisabledColor
                            anchors.verticalCenter: parent.verticalCenter
                            font.pixelSize: 14
                        }
                    }
                }
            }
        }

        Row{
            spacing: 0
            anchors{
                left: contentColumn.left
                bottom: contentSplit.top
                bottomMargin: 15
            }
            Text {
                text: "*"
                color: Config.textColor
                font.pixelSize: 13
                anchors{
                    top: parent.top
                    topMargin: -2
                }
            }
            Text {
                text: qsTr("勾选某一通道后，小于设定宽度的脉冲将被滤除")
                color: Config.textColor
                font.pixelSize: 13
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
                        sendDecodeData();
                        window.close();
                    }
                }
            }
        }
    }

    function sendDecodeData(){
        dataSend(dataJson)
    }
}
