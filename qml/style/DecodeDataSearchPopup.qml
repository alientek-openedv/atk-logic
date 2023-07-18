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

import QtQuick 2.13
import QtQuick.Controls 2.5
import "../config"
import "../style"

Popup{
    property int maxWidth: 160
    property int rowHeight: 30
    property int maxBottom
    signal selectDecode(var decodeID);
    signal selectRow(var rowName,var isSelect,var isRefresh);
    id: mainPopup
    y: height>maxBottom?maxBottom-height:0
    x: -contentWidth
    width: contentWidth
    height: contentHeight
    contentWidth: maxWidth+30;
    contentHeight: mainRectangle.height+2
    padding: 1
    clip: true;
    onOpened: refreshShow();
    background: Item{}
    Rectangle{
        id: mainRectangle
        width: parent.width
        height: showColumn.height+10
        radius: 4
        clip: true;
        color: Config.backgroundColor
        border{
            width: 1
            color: Config.lineDisableColor
        }

        Column{
            id: showColumn
            width: parent.width-2
            height: decode_header.height+decode_listview.height+row_header.height+row_listview.height+11
            anchors{
                top: parent.top
                topMargin: 5
                left: parent.left
                leftMargin: 1
            }

            spacing: 0
            Item{
                id: decode_header
                width: parent.width
                height: rowHeight
                Text {
                    text: qsTr("协议：")
                    color: Config.textColor
                    anchors{
                        verticalCenter: parent.verticalCenter
                        left: parent.left
                        leftMargin: 24
                    }
                }
            }
            ListView {
                id: decode_listview
                model: ListModel{
                    id: decode_model
                }
                clip: true
                width: parent.width
                height: contentHeight
                snapMode: ListView.NoSnap
                flickableDirection: Flickable.AutoFlickDirection
                boundsBehavior: Flickable.StopAtBounds
                ScrollIndicator.vertical: ScrollIndicator { }
                delegate: Rectangle{
                    width: decode_listview.width
                    height: rowHeight
                    color: itemMouseArea.containsMouse?Config.mouseCheckColor:"transparent"
                    MouseArea{
                        id: itemMouseArea
                        anchors.fill: parent
                        hoverEnabled: true
                        onPressed: {
                            if(!isSelect)
                                selectDecode(decodeID);
                        }
                    }
                    Item{
                        id: checkedItem
                        width: 24
                        height: parent.height
                        Rectangle{
                            width: 12
                            height: width
                            color: Config.channelDataColor[colorIndex%16]
                            radius: 4
                            anchors.centerIn: parent
                            Image {
                                visible: isSelect
                                source: "../../resource/icon/CheckedWhite.png"
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.horizontalCenter: parent.horizontalCenter
                            }
                        }
                    }
                    Text{
                        id: showTextElement
                        text: name
                        font.pixelSize: 12
                        anchors{
                            verticalCenter: parent.verticalCenter
                            left: checkedItem.right
                        }
                        color: itemMouseArea.containsMouse?"white":Config.textColor
                        Component.onCompleted: {
                            var temp=showTextElement.contentWidth;
                            if(temp>maxWidth)
                                maxWidth=temp;
                        }
                    }
                }
            }

            Item{
                width: parent.width
                height: 11
                Rectangle{
                    height: 1
                    width: parent.width
                    color: Config.lineColor
                    anchors.verticalCenter: parent.verticalCenter
                }
            }

            Item{
                id: row_header
                width: parent.width
                height: rowHeight
                Text {
                    text: qsTr("显示行：")
                    color: Config.textColor
                    anchors{
                        verticalCenter: parent.verticalCenter
                        left: parent.left
                        leftMargin: 24
                    }
                }
                Item{
                    height: parent.height
                    width: 85
                    anchors.right: parent.right
                    TextButton{
                        id: clearRowButton
                        text: "clear"
                        anchors{
                            verticalCenter: parent.verticalCenter
                            right: parent.right
                            rightMargin: 8
                        }
                        width: 40
                        height: 16
                        textFontSize: 11
                        textColor: Config.iceBlue
                        textEnterColor: Config.subheadColor
                        backgroundRectangle{
                            border{
                                width: 1
                                color: Config.iceBlue
                            }
                            radius: 12
                        }
                        onPressed: setAllRowState(false);
                    }
                    TextButton{
                        id: allRowButton
                        text: "all"
                        anchors{
                            verticalCenter: parent.verticalCenter
                            right: clearRowButton.left
                            rightMargin: 5
                        }
                        width: 30
                        height: 16
                        textFontSize: 11
                        textColor: Config.iceBlue
                        textEnterColor: Config.subheadColor
                        backgroundRectangle{
                            border{
                                width: 1
                                color: Config.iceBlue
                            }
                            radius: 8
                        }
                        onPressed: setAllRowState(true);
                    }
                }
            }
            ListView {
                id: row_listview
                model: ListModel{
                    id: row_model
                }
                clip: true
                width: parent.width
                height: contentHeight
                snapMode: ListView.NoSnap
                flickableDirection: Flickable.AutoFlickDirection
                boundsBehavior: Flickable.StopAtBounds
                ScrollIndicator.vertical: ScrollIndicator { }
                delegate: Rectangle{
                    width: row_listview.width
                    height: rowHeight
                    color: rowItemMouseArea.containsMouse?Config.mouseCheckColor:"transparent"
                    MouseArea{
                        id: rowItemMouseArea
                        anchors.fill: parent
                        hoverEnabled: true
                        onPressed: {
                            isSelect=!isSelect;
                            selectRow(showText, isSelect, true);
                        }
                    }
                    Item{
                        id: rowCheckedItem
                        width: 24
                        height: parent.height
                        Image {
                            visible: isSelect
                            source: "../../resource/icon/"+(rowItemMouseArea.containsMouse?"CheckedWhite.png":"Checked.png")
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.horizontalCenter: parent.horizontalCenter
                        }
                    }
                    Text{
                        id: rowShowTextElement
                        text: showText
                        font.pixelSize: 12
                        anchors{
                            verticalCenter: parent.verticalCenter
                            left: rowCheckedItem.right
                        }
                        color: rowItemMouseArea.containsMouse?"white":Config.textColor
                        Component.onCompleted: {
                            var temp=rowShowTextElement.contentWidth;
                            if(temp>maxWidth&&temp<=300)
                                maxWidth=temp;
                        }
                    }
                }
            }
        }
    }
    function refreshShow(){
        decode_model.clear();
        row_model.clear();
        for(var i in decodeShowRowList)
        {
            decode_model.append(decodeShowRowList[i]);
            if(decodeShowRowList[i]["isSelect"])
            {
                for(var j in decodeShowRowList[i]["rowList"])
                    row_model.append(decodeShowRowList[i]["rowList"][j]);
            }
        }
    }

    function setAllRowState(isSelect){
        decodeTableModel.stopAll(false)
        for(var i=0;i<row_model.count;i++){
            var item= row_model.get(i);
            selectRow(item.showText, isSelect, i+1===row_model.count);
        }
        refreshShow();
    }
}
