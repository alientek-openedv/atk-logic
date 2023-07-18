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
import "../style"
import "./content"

Rectangle {
    id: rootSideBarContent
    color: "transparent"
    clip: true
    border{
        width:1
        color: Config.lineColor
    }

    Connections{
        target: sSignal
        function onSideBarCanLoader()
        {
            sideBarLoader.sourceComponent=sideBarComponent;
        }
    }

    Loader{
        id: sideBarLoader
        width: parent.width
        height: parent.height
        anchors{
            top: parent.top
            left: parent.left
            leftMargin: 1
        }
        asynchronous: true  
    }

    Connections{
        target: sessionItem
        function onHeightChanged(){
            if(rootSideBarContent.width>sessionItem.width-200){
                let tmp=sessionItem.width-200;
                if(tmp<Config.sideBarContentWidth)
                    tmp=Config.sideBarContentWidth;
                rootSideBarContent.width=tmp;
            }
        }
    }

    MouseArea{
        width: 3
        height: parent.height
        acceptedButtons: Qt.LeftButton
        cursorShape: Qt.SplitHCursor
        hoverEnabled: true
        preventStealing: true
        onMouseYChanged: {
            if(pressed)
            {
                if(rootSideBarContent.width-mouseX<Config.sideBarContentWidth)
                    rootSideBarContent.width=Config.sideBarContentWidth;
                else if(rootSideBarContent.width-mouseX>sessionItem.width-200)
                    rootSideBarContent.width=sessionItem.width-200;
                else
                    rootSideBarContent.width-=mouseX;
            }
        }
        onPressedChanged: {
            if(pressed)
                Signal.setCursor(Qt.SplitHCursor);
            else
                Signal.setCursor(Qt.ArrowCursor);
        }
    }

    Component{
        id: sideBarComponent
        Rectangle {
            id: rootSideBarRectangle
            width: parent.width
            height: parent.height
            color: Config.background2Color
            ScrollView {
                property var contentType: Config.SidebarContentType.Set
                id: setContentScrollView
                height: parent.height
                width: parent.width
                clip: true
                visible: sConfig.sidebarContentType===setContentScrollView.contentType
                anchors{
                    top: parent.top
                    left: parent.left
                }
                Flickable{
                    id: setContentColumnLayout
                    flickableDirection: Flickable.AutoFlickDirection
                    boundsBehavior: Flickable.StopAtBounds
                    contentWidth: parent.width-26
                    contentHeight: setContent.height
                    anchors{
                        top: parent.top
                        left: parent.left
                        margins: 14
                    }
                    SetContent{
                        id: setContent
                        height: 820
                        width: parent.width
                    }
                }
                onHeightChanged: setScrollBarVisible(setContentScrollBar,setContentScrollView.contentType,height<setContentColumnLayout.contentHeight)
                onVisibleChanged: {
                    if(visible)
                        setScrollBarVisible(setContentScrollBar,setContentScrollView.contentType,height<contentHeight)
                    else
                        setScrollBarVisible(setContentScrollBar,setContentScrollView.contentType,false)
                }
                ScrollBar.vertical.policy: ScrollBar.AlwaysOff
                Component.onCompleted: ScrollBar.vertical=setContentScrollBar
            }
            QScrollBar{
                id: setContentScrollBar
                visible: false
                anchors{
                    top: parent.top
                    right: parent.right
                }
            }

            DecodeContent{
                id: decodeContent
                height: parent.height
                width: parent.width
                anchors{
                    top: parent.top
                    left: parent.left
                }
                clip: true
                visible: sConfig.sidebarContentType===Config.SidebarContentType.Decode
            }

            MeasureContent{
                id: measureContent
                height: parent.height
                width: parent.width
                anchors{
                    top: parent.top
                    left: parent.left
                }
                clip: true
                visible: sConfig.sidebarContentType===Config.SidebarContentType.Measure
            }

            SearchContent{
                id: searchContent
                height: parent.height
                width: parent.width
                anchors{
                    top: parent.top
                    left: parent.left
                }
                clip: true
                visible: sConfig.sidebarContentType===Config.SidebarContentType.Search
            }
        }
    }

    function setScrollBarVisible(scrollBar,type,visible){
        if(visible)
        {
            if(scrollBar.visible===false && sConfig.sidebarContentType===type)
                scrollBar.visible=true
        }else{
            if(scrollBar.visible)
                scrollBar.visible=false
        }
    }
}
