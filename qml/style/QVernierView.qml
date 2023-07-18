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
import QtQuick.Controls 2.5
import "../config"

ListView {
    property int rowsHeight: 28
    property int changedExtendIndex
    property int selectIndex: -1

    id: listView
    headerPositioning: ListView.OverlayHeader
    flickableDirection: Flickable.AutoFlickDirection
    boundsBehavior: Flickable.StopAtBounds
    ScrollBar.vertical: vbar
    ScrollBar.horizontal: hbar
    delegate: listDelegate
    contentHeight: height
    contentWidth: width
    height: parent.height
    width: parent.width
    clip: true
    currentIndex: -1
    ScrollBar {
        id: hbar
        policy: ScrollBar.AlwaysOff
    }

    QScrollBar {
        id: vbar
        anchors{
            top: parent.top
            right: parent.right
        }
    }
    move: Transition {
        NumberAnimation { properties: "x,y"; duration: 100 }
    }
    Component{
        id: listDelegate
        Rectangle{
            property var datacolor_: dataColor
            onDatacolor_Changed: canvas.requestPaint()
            width: listView.contentWidth
            height: visible?rowsHeight:0
            visible: visible_
            color: currentIndex===index?Config.mouseEnterColor:"transparent"
            Column{
                spacing: 0
                width: parent.width
                height: parent.height
                Rectangle{
                    property bool isContainsMouse: rowMouseArea.containsMouse||followButton.containsMouse||moreButton.containsMouse
                    id: parentRectangle
                    width: parent.width
                    height: rowsHeight
                    color: (parentRectangle.isContainsMouse&&listView.selectIndex===index)?Config.mouseEnterColor:"transparent"
                    onIsContainsMouseChanged: {
                        if(isContainsMouse)
                            listView.selectIndex=index;
                        else
                            listView.selectIndex=-1;
                    }
                    ImageButton{
                        id: followButton
                        visible: parentRectangle.isContainsMouse&&listView.selectIndex===index
                        imageSource: "resource/icon/DecodeFollow.png"
                        imageEnterSource: "resource/icon/DecodeFollow.png"
                        width: 16
                        height: 16
                        z: 5
                        anchors{
                            verticalCenter: parent.verticalCenter
                            right: moreButton.left
                            rightMargin: 2
                        }
                        onPressed: controller.showViewScope(position,position,!Setting.jumpZoom);
                    }
                    ImageButton{
                        id: moreButton
                        visible: parentRectangle.isContainsMouse
                        imageSource: "resource/icon/"+Config.tp+"/More.png"
                        imageEnterSource: "resource/icon/"+Config.tp+"/MoreEnter.png"
                        width: 10
                        height: 16
                        z: 5
                        anchors{
                            verticalCenter: parent.verticalCenter
                            right: parent.right
                            rightMargin: vbar.visible?13:6
                        }
                        onPressed: menuPopup.visible=true
                    }
                    Connections{
                        target: sConfig
                        function onIsRunChanged(){
                            menuPopup.close();
                        }
                    }
                    QMenuPopup{
                        id: menuPopup
                        parent: moreButton
                        showShortcutKey: false
                        data: [{"showText":qsTr("跳到该处"),"shortcutKey":"","seleteType":0,"buttonIndex":0},
                            {"showText":"-","shortcutKey":"","seleteType":0,"buttonIndex":-1},
                            {"showText":qsTr("删除"),"shortcutKey":"","seleteType":0,"buttonIndex":1}]
                        onSelect: {
                            switch(index){
                            case 0:
                                controller.showViewScope(position,position,!Setting.jumpZoom);
                                break;
                            case 1:
                                vernierListModel.remove(vernierID)
                                break;
                            }
                        }
                    }
                    MouseArea{
                        id: rowMouseArea
                        anchors.fill: parent
                        hoverEnabled: true
                        onDoubleClicked: controller.showViewScope(position,position,!Setting.jumpZoom);
                    }
                    Row{
                        spacing: 0
                        anchors.verticalCenter: parent.verticalCenter
                        Item{
                            width: 16
                            height: 16
                        }
                        Item{
                            width: 10
                            height: parent.height
                            Canvas{
                                property int drawHeight: 10
                                id: canvas
                                width: 6
                                height: drawHeight
                                z: 10
                                anchors.verticalCenter: parent.verticalCenter
                                onPaint: {
                                    var ctx = getContext("2d");
                                    ctx.lineWidth = 0
                                    ctx.strokeStyle = dataColor
                                    ctx.fillStyle = dataColor
                                    ctx.beginPath();
                                    ctx.moveTo(0, 0)
                                    ctx.lineTo(canvas.width, 0);
                                    ctx.lineTo(canvas.width, canvas.height-3);
                                    ctx.lineTo(canvas.width/2, canvas.height);
                                    ctx.lineTo(0, canvas.height-3);
                                    ctx.closePath();
                                    ctx.fill()
                                    ctx.stroke();
                                }
                            }
                        }
                        Text {
                            text: name+" = "+positionStr
                            color: Config.textColor
                        }
                    }
                }
            }
        }
    }
}
