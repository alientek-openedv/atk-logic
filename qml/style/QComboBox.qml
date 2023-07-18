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

ComboBox {
    property alias backgroundRectangle: backRect
    property var currentTextValue
    property var currentModelChildren
    property int popupMaxHeight: 30*8
    property bool showArrows: true
    property bool wheelChanged: Setting.componentWheelChanged
    property bool disable: false
    property var modelCount: model.count
    property bool isError: false

    onDisableChanged: {
        if(disable)
            popup.close();
    }

    onModelCountChanged: {
        if(currentIndex>=modelCount)
            currentIndex=0;
        refreshText();
    }

    id: rootComboBox
    implicitHeight: 30
    focusPolicy: Qt.NoFocus
    clip: true
    delegate: ItemDelegate
    {
        clip: true
        height: 30
        width: rootComboBox.width
        contentItem: Text {
            text: showText?showText:""
            elide: Text.ElideRight
            color: rootComboBox.highlightedIndex === index ? "white" : Config.textColor
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: 12
        }
        background: Rectangle{
            anchors.fill: parent
            color: rootComboBox.highlightedIndex === index ? Config.mouseCheckColor : "transparent"
        }
        highlighted: rootComboBox.highlightedIndex === index
    }

    indicator: Image{
        visible: showArrows
        anchors{
            verticalCenter: parent.verticalCenter
            right: parent.right
            rightMargin: 6
        }
        source: disable?"qrc:resource/icon/"+Config.tp+"/PullDownDisable.png":"qrc:resource/icon/"+Config.tp+"/PullDown.png"
    }

    contentItem: Text {
        leftPadding: 6
        rightPadding: rootComboBox.indicator.width + rootComboBox.spacing
        text: currentTextValue
        horizontalAlignment: showArrows?Text.AlignLeft:Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
        color: disable ? Config.textDisabledColor:Config.textColor
        font.pixelSize: 12
    }

    onCurrentIndexChanged: refreshText();

    onPressedChanged: {
        if(disable)
            pressed=false
    }

    popup: Popup {
        id: popup
        y: rootComboBox.height;
        width: rootComboBox.width;
        height: contentItem.implicitHeight>popupMaxHeight?popupMaxHeight:contentItem.implicitHeight;
        clip: true
        contentItem: ListView {
            clip: true;
            implicitHeight: contentHeight;
            onImplicitHeightChanged: showAnimation.to=implicitHeight>popupMaxHeight?popupMaxHeight:implicitHeight;
            flickableDirection: Flickable.AutoFlickDirection
            boundsBehavior: Flickable.StopAtBounds
            model: rootComboBox.delegateModel;
            currentIndex: rootComboBox.highlightedIndex;
            highlightMoveDuration: 0;
            anchors.fill: parent
            snapMode: ListView.NoSnap
            ScrollIndicator.vertical: ScrollIndicator {}
        }

        background: Rectangle {
            anchors.fill: parent
            color: Config.borderBackgroundColor;
            border{
                width: 1
                color: Config.borderLineColor
            }
            radius: 4;
            clip: true
        }

        onOpenedChanged: {
            if(opened)
                showAnimation.start();
        }


        PropertyAnimation {
            id: showAnimation
            target: popup;
            properties: "height";
            from: 0
            to: contentItem.implicitHeight>popupMaxHeight?popupMaxHeight:contentItem.implicitHeight;
            duration: 100
        }
    }

    background: Rectangle {
        id: backRect
        anchors.fill: parent
        color: Config.borderBackgroundColor
        border{
            width: 1
            color: isError?Config.red:rootMouseArea.containsMouse && wheelChanged && !disable?Config.mouseCheckColor:disable?Config.lineDisableColor:Config.borderLineColor
        }
        radius: 4
    }

    MouseArea{
        id: rootMouseArea
        anchors.fill: parent
        acceptedButtons: Qt.NoButton
        hoverEnabled: true
        onWheel: {
            if(wheelChanged)
                wheelChangedSelect(wheel.angleDelta.y>0)
        }
        enabled: !rootComboBox.disable
    }

    function wheelChangedSelect(isUp){
        if(isUp){
            if(currentIndex>0)
                currentIndex--;
        }else{
            if(currentIndex+1<modelCount)
                currentIndex++;
        }
    }

    function refreshText(){
        currentModelChildren=rootComboBox.model.get(rootComboBox.currentIndex);
        if(currentModelChildren)
            currentTextValue=currentModelChildren.showText;
        else
            currentTextValue="";
    }
}


