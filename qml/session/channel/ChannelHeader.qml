﻿/**
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
import atk.qml.Controls 1.0
import "../../config"
import "../../style"

Item {
    property bool disable: false
    id: main
    Connections{
        target: sSignal
        function onChannelRefresh(channelID)
        {
            if(channelID<0&&!sConfig.isExit)
                drawChannelHeader.update();
        }

        function onVernierCreate(state){
            if(state===0 && !sConfig.isChannelMethodRun)
            {
                drawChannelHeader.createVernier();
                sConfig.isVernierStart=true;
            }
            else if (state===2)
            {
                drawChannelHeader.isCreating=false;
                sConfig.isVernierStart=false;
            }
        }
    }
    Connections{
        target: Config
        function onTpChanged()
        {
            drawChannelHeader.setTheme(Config.tp);
        }
    }
    Rectangle{
        width: parent.width
        height: parent.height
        color: Config.backgroundColor
        Column
        {
            height: parent.height
            width: parent.width
            spacing: 0
            Row{
                height: parent.height-1
                width: parent.width
                spacing: 0
                z: 5
                Item{
                    id: channelIcon
                    height: Config.headerHeight
                    width: (sConfig.isExpandSidebar?Config.headerWidth:Config.closeHeaderWidth)+5
                    Behavior on width {
                        NumberAnimation {
                            duration: 100
                        }
                    }
                    Item{
                        height: parent.height
                        width: parent.width-30
                        Image {
                            anchors{
                                verticalCenter: parent.verticalCenter
                                horizontalCenter: parent.horizontalCenter
                                horizontalCenterOffset: 5
                            }
                            source: sConfig.isExpandSidebar?"qrc:resource/image/ChannelIcon.png":"qrc:resource/image/ChannelSingleIcon.png"
                        }
                    }
                    ImageButton{
                        height: parent.height
                        width: height
                        anchors{
                            right: parent.right
                        }
                        imageSource: "resource/icon/"+Config.tp+"/Expand.png"
                        imageEnterSource: "resource/icon/"+Config.tp+"/ExpandEnter.png"
                        imageDisableSource: "resource/icon/"+Config.tp+"/ExpandEnter.png"
                        rotation: sConfig.isExpandSidebar?0:180
                        onPressed: sConfig.isExpandSidebar=!sConfig.isExpandSidebar
                    }
                }
                DrawChannelHeader{
                    id: drawChannelHeader
                    height: Config.headerHeight
                    width: parent.width-channelIcon.width
                    isExit: sConfig.isExit
                    MouseArea{
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        acceptedButtons: Qt.NoButton
                        propagateComposedEvents: true
                    }
                    onVernierAppend: {
                        vernierListModel.append();
                        sSignal.channelRefresh(-1);
                    }
                    onShowVernierPopup: sSignal.showVernierPopup(vernierID, mouse_x);
                    onCloseVernierPopup: sSignal.closeVernierPopup();
                    onVernierDataChanged: {
                        sSignal.vernierDataChanged(vernierID);
                        vernierListModel.refresh(vernierID);
                    }
                    onDeleteVernier: vernierListModel.remove(vernierID);
                    onVernierCreate: sSignal.vernierCreate(1);
                    onVernierCreateComplete: sSignal.vernierCreate(2);
                    onVernierMoveState: {
                        if(isMove)
                            sConfig.isVernierMove=true;
                        else{
                            sConfig.isVernierMove=false;
                            sSignal.vernierMoveComplete(id);
                        }
                    }
                    Component.onCompleted: {
                        drawChannelHeader.init(sessionID_)
                        setTheme(Config.tp);
                    }
                    MouseArea{
                        visible: main.disable
                        anchors.fill: drawChannelHeader
                        acceptedButtons: Qt.AllButtons
                        preventStealing: true
                        propagateComposedEvents: false
                    }
                }
            }
            Rectangle{
                width: parent.width
                height: 1
                color: Config.lineColor
                z: 1
            }
        }
    }
}
