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
        asynchronous: true  //异步加载会话
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
            //设置
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

            //解码
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

            //触发
//            ScrollView {
//                property var contentType: Config.SidebarContentType.Trigger
//                id: triggerContentScrollView
//                implicitHeight: parent.height
//                implicitWidth: parent.width
//                clip: true
//                visible: sConfig.sidebarContentType===triggerContentScrollView.contentType
//                anchors{
//                    top: parent.top
//                    left: parent.left
//                }
//                Flickable{
//                    id: triggerContentColumnLayout
//                    flickableDirection: Flickable.AutoFlickDirection
//                    boundsBehavior: Flickable.StopAtBounds
//                    contentWidth: parent.width-28
//                    contentHeight: triggerContent.height
//                    anchors{
//                        top: parent.top
//                        left: parent.left
//                        margins: 14
//                    }
//                    TriggerContent{
//                        id: triggerContent
//                        height: 300
//                        width: parent.width
//                    }
//                }
//                onContentHeightChanged: setScrollBarVisible(triggerContentScrollBar,triggerContentScrollView.contentType,height<contentHeight)
//                onHeightChanged: setScrollBarVisible(triggerContentScrollBar,triggerContentScrollView.contentType,height<triggerContentColumnLayout.height)
//                onVisibleChanged: {
//                    if(visible)
//                        setScrollBarVisible(triggerContentScrollBar,triggerContentScrollView.contentType,height<contentHeight)
//                    else
//                        setScrollBarVisible(triggerContentScrollBar,triggerContentScrollView.contentType,false)
//                }
//                ScrollBar.vertical.policy: ScrollBar.AlwaysOff
//                Component.onCompleted: ScrollBar.vertical=triggerContentScrollBar
//            }
//            QScrollBar{
//                id: triggerContentScrollBar
//                visible: false
//                anchors{
//                    top: parent.top
//                    right: parent.right
//                }
//            }

            //测量
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

            //搜索
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
