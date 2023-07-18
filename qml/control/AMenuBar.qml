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

Rectangle {
    id: rootRectangle
    color: Config.background2Color

    Item{
        id: iconItem
        width: 34
        height: parent.height
        Image {
            anchors{
                horizontalCenter: parent.horizontalCenter
                horizontalCenterOffset: 3
                top: parent.top
                topMargin: 8
            }
            source: "../../resource/icon/icon.png"
        }
    }

    Row{
        id: rootRow
        spacing: 0
        height: parent.height
        anchors.left: iconItem.right
        AMenuTabButton{
            buttonText: qsTr("文件")
            buttonIndex: Config.MenuItemType.File
        }
        AMenuTabButton{
            buttonText: qsTr("设备")
            buttonIndex: Config.MenuItemType.Property
        }
        AMenuTabButton{
            buttonText: qsTr("功能")
            buttonIndex: Config.MenuItemType.Show
        }
        AMenuTabButton{
            buttonText: qsTr("关于我们")
            buttonIndex: Config.MenuItemType.About
        }
    }

    Item{
        width: parent.width-rootRow.width-stateButtonRow.width-iconItem.width
        height: parent.height
        anchors.left: rootRow.right
        Text {
            text: qsTr("ALIENTEK.LogicView ")+"V"+app_VERSION
            anchors.centerIn: parent
            color: Config.textColor
        }
    }

    Row{
        id: stateButtonRow
        spacing: 0
        height: parent.height
        anchors.right: parent.right

        ImageButton{
            id: helpButton
            width: 35
            height: parent.height
            imageSource: "resource/icon/"+Config.tp+"/Help.png"
            imageEnterSource: imageSource
            backgroundColor: rootRectangle.color
            backgroundPressedColor: Config.lineColor
            backgroundMouseEnterColor: Config.lineColor
            onClicked: {
                if(!root.openDataResources())
                    Qt.openUrlExternally("http://www.openedv.com/docs/tool/luojifenxiyi/DL16.html")
            }
        }
        ImageButton{
            id: minButton
            width: 35
            height: parent.height
            imageSource: "resource/icon/"+Config.tp+"/Min.png"
            imageEnterSource: imageSource
            backgroundColor: rootRectangle.color
            backgroundPressedColor: Config.lineColor
            backgroundMouseEnterColor: Config.lineColor
            onClicked: root.showMinimized_();
        }
        ImageButton{
            id: maxButton
            width: 35
            height: parent.height
            imageSource: "resource/icon/"+Config.tp+"/"+(root.windowState===2?"MaxIn.png":"Max.png")
            imageEnterSource: imageSource
            backgroundColor: rootRectangle.color
            backgroundPressedColor: Config.lineColor
            backgroundMouseEnterColor: Config.lineColor
            onClicked: {
                if(root.windowState===2)
                    root.showNormal();
                else
                    root.showMaximized();
            }
        }
        ImageButton{
            id: closeButton
            width: 35
            height: parent.height
            imageSource: "resource/icon/"+Config.tp+"/Close.png"
            imageEnterSource: imageSource
            backgroundColor: rootRectangle.color
            backgroundPressedColor: Config.red
            backgroundMouseEnterColor: Config.red
            onClicked: Signal.closeApp();
        }
    }

    Connections{
        target: Signal
        function onClickMenuTabButton(buttonIndex,isDoubleClick)
        {
            if(buttonIndex===Config.MenuItemType.About)
            {
                if(!Config.isFixed)
                    Signal.menuStateChanged(Config.MenuState.NoDisplay);
                aboutUs.visible=true;
            }else{
                Config.menuSelectButton=buttonIndex;
                for(var i in rootRow.children)
                {
                    if(rootRow.children[i].buttonIndex===buttonIndex)
                        if(rootRow.children[i].state==="selected"){
                            if(isDoubleClick)
                            {
                                if(Config.isFixed)
                                    Signal.menuStateChanged(Config.MenuState.NoDisplay);
                                Signal.menuStateChanged(Config.isFixed?Config.MenuState.NoFixed:Config.MenuState.Fixed);
                                if(Config.isFixed)
                                    Signal.menuStateChanged(Config.MenuState.Display);
                            }
                            else if(!Config.isFixed)
                                Signal.menuStateChanged(Config.MenuState.NoDisplay);
                            break;
                        }
                        else
                        {
                            rootRow.children[i].state="selected";
                            if(isDoubleClick)
                                Signal.menuStateChanged(Config.MenuState.Fixed);
                            Signal.menuStateChanged(Config.MenuState.Display);

                        }
                    else
                        rootRow.children[i].state="noSelected";
                }
            }
        }
        function onMenuStateChanged(state)
        {
            if(state===Config.MenuState.NoDisplay){
                for(var i in rootRow.children)
                    rootRow.children[i].state="noSelected";
            }
        }
    }
}
