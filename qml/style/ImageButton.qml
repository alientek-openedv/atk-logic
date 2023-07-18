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

Button {
    property var imageSource
    property var imageEnterSource
    property var imagePressedSource
    property var imageDisableSource
    property color backgroundColor: "transparent"
    property color backgroundPressedColor: "transparent"
    property color backgroundMouseEnterColor: "transparent"
    property bool autoChecked: true
    property bool isChecked: false
    property string showText
    property color textColor: Config.textColor
    property alias fontSize: textControl.font.pixelSize
    property alias fillMode: imageRoot.fillMode
    property alias underline: textControl.font.underline
    property alias imageHeight: imageRoot.height
    property alias imageWidth: imageRoot.width
    property alias verticalCenterOffset: imageRoot.anchors.verticalCenterOffset
    property alias horizontalCenterOffset: imageRoot.anchors.horizontalCenterOffset
    property alias backgroundRectangle: backRect
    property alias containsMouse: mouseAreaRoot.containsMouse
    property alias cursorShape: mouseAreaRoot.cursorShape
    signal rightClicked()

    clip: true
    width: (showText!=""?textControl.width<implicitWidth?implicitWidth:textControl.width:implicitWidth)+padding*2
    padding: 0
    id: rootContent

    focusPolicy: Qt.NoFocus

    Item{
        height: parent.height-padding*2
        width: parent.width-padding*2
        anchors{
            top: parent.top
            left: parent.left
            margins: padding
        }
        Item{
            id: imageItem
            width: parent.width
            height: showText!=""?parent.height-textControl.height:parent.height
            Image{
                id: imageRoot
                width: parent.width
                height: parent.height
                anchors{
                    verticalCenter: parent.verticalCenter
                    horizontalCenter: parent.horizontalCenter
                }
                source: "qrc:"+imageSource
                fillMode: Image.Pad
                onStatusChanged: {
                    if ((imageRoot.status == Image.Error) && source !== imageSource )
                    {
                        source = "qrc:"+imageSource
                        imageEnterSource = imageSource
                    }
                }
            }
        }
        Text{
            id: textControl
            height: contentHeight
            width: contentWidth<implicitWidth?implicitWidth:contentWidth
            color: rootContent.enabled?textColor:Config.textDisabledColor
            visible: showText!=""
            text: showText
            font{
                underline: false
                pixelSize: 12
            }
            anchors{
                top: imageItem.bottom
                horizontalCenter: parent.horizontalCenter
            }
        }
    }

    background: Rectangle{
        id: backRect
        color: backgroundColor
        Behavior on color{
            ColorAnimation {
                duration: 50
            }
        }
    }

    MouseArea{
        property var isMouseEnter
        id: mouseAreaRoot
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        acceptedButtons: Qt.RightButton
        enabled: parent.enabled
        onEntered: mouseEnter(true)
        onExited:  mouseEnter(false)
        onClicked: {
            if(mouse.button == Qt.RightButton)
                rightClicked()
            else if(mouse.button == Qt.LeftButton){
                if(autoChecked)
                    isChecked=!isChecked;
            }
        }
    }

    function mouseEnter(flag){ 
        if(typeof(flag)!=="undefined")
        {
            if(checkable)
            {
                backRect.color = isChecked ? backgroundPressedColor : flag ? backgroundMouseEnterColor : backgroundColor
                imageRoot.source = "qrc:" + (enabled ? isChecked ? imagePressedSource : flag ? imageEnterSource : imageSource : imageDisableSource)
            }
            else
            {
                backRect.color = flag ? backgroundMouseEnterColor : backgroundColor
                imageRoot.source = "qrc:" + (enabled ? flag ? imageEnterSource : imageSource : imageDisableSource)
            }
            mouseAreaRoot.isMouseEnter=flag
        }else if(checkable && isChecked)
            backRect.color = backgroundMouseEnterColor;
        imageRoot.update();
        backRect.update();
    }

    onDownChanged: {
        var color=down ? backgroundPressedColor : backgroundColor;
        if(!checkable)
        {
            backRect.color=color;
            if(!down)
                mouseEnter(mouseAreaRoot.isMouseEnter);
        }else
            mouseEnter(mouseAreaRoot.isMouseEnter);
    }

    onEnabledChanged: mouseEnter(false);

    onIsCheckedChanged: mouseEnter(mouseAreaRoot.isMouseEnter)

    onBackgroundColorChanged: mouseEnter(mouseAreaRoot.isMouseEnter)

    onBackgroundMouseEnterColorChanged: mouseEnter(mouseAreaRoot.isMouseEnter)

    onImageSourceChanged: mouseEnter(mouseAreaRoot.isMouseEnter)

    onImageEnterSourceChanged: mouseEnter(mouseAreaRoot.isMouseEnter)

    onImageDisableSourceChanged: mouseEnter(mouseAreaRoot.isMouseEnter)

    Component.onCompleted: {
        if(!imageEnterSource)
        {
            if(imageSource.indexOf(".")!==-1)
            {
                var split=imageSource.split(".",2);
                imageEnterSource=split[0]+"Enter.png"
                imagePressedSource=imageEnterSource;
            }
        }
        mouseEnter(false);
    }
}
