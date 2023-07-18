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
    id: mainPopup
    y: height>maxBottom?maxBottom-height:0
    x: -contentWidth
    width: contentWidth
    height: contentHeight
    contentWidth: maxWidth+30;
    contentHeight: mainRectangle.height+2
    padding: 1
    clip: true;
    onOpened: {
        searchInput.text="";
        searchInput.focus=true;
        searchInput.forceActiveFocus()
        decodeSearchListModel.refresh()
        row_listview.positionViewAtBeginning();
    }

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
            color: Config.lineColor
        }

        Column{
            id: showColumn
            width: parent.width-2
            height: decode_header.height+row_listview.height+11
            anchors{
                top: parent.top
                topMargin: 5
                left: parent.left
                leftMargin: 1
            }

            spacing: 5
            Item{
                id: decode_header
                width: parent.width
                height: rowHeight
                Row{
                    width: parent.width-20
                    height: parent.height
                    spacing: 5
                    anchors{
                        left: parent.left
                        leftMargin: 10
                    }
                    Image {
                        width: 13
                        height: 15
                        anchors.verticalCenter: parent.verticalCenter
                        source: "../../resource/icon/DecodeSearch.png"
                    }
                    TextInput {
                        id: searchInput
                        width: parent.width-18
                        height: contentHeight
                        anchors.verticalCenter: parent.verticalCenter
                        font.pixelSize: 14
                        clip: true
                        verticalAlignment: Qt.AlignVCenter
                        color: Config.textColor
                        MouseArea{
                            anchors.fill: parent
                            acceptedButtons: Qt.NoButton
                            cursorShape: Qt.IBeamCursor
                        }
                        onTextChanged: {
                            if(text==="")
                                decodeSearchListModel.exitSearch();
                            else
                                decodeSearchListModel.search(text);
                            decodeSearchListModel.refresh();
                            row_listview.positionViewAtBeginning();
                        }
                    }
                }
            }
            Rectangle{
                height: 1
                width: parent.width
                color: Config.lineColor
            }
            ListView {
                id: row_listview
                model: decodeSearchListModel
                clip: true
                width: parent.width
                height: contentHeight>346?346:contentHeight
                snapMode: ListView.NoSnap
                flickableDirection: Flickable.AutoFlickDirection
                boundsBehavior: Flickable.StopAtBounds
                ScrollIndicator.vertical: ScrollIndicator { }
                delegate: Rectangle{
                    width: row_listview.width
                    height: rowHeight
                    color: rowItemMouseArea.containsMouse?Config.mouseCheckColor:"transparent"
                    clip: true
                    MouseArea{
                        id: rowItemMouseArea
                        anchors.fill: parent
                        hoverEnabled: true
                        onPressed: {
                            showDecodeWindow(name);
                            close();
                        }
                        onEntered: sSignal.decodeDocPopupShow(true, note);
                        onExited: sSignal.decodeDocPopupShow(false, "");
                    }
                    Text{
                        id: rowShowTextElement
                        text: name
                        font.pixelSize: 14
                        width: parent.width-20
                        anchors{
                            verticalCenter: parent.verticalCenter
                            left: parent.left
                            leftMargin: 10
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
}
