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

Rectangle {
    property int measureID

    id: rootItem
    height: bottomRectangle.height+showRectangle.height-8
    width: 400
    visible: (set.isTop||set.isSelect)&&set.isShow
    z: set.isSelect?2500:set.isTop?2000:2
    radius: 8
    color: Config.borderLineColor
    border{
        width: 1
        color: set.isSelect?Config.mouseCheckColor:Config.borderLineColor
    }

    onVisibleChanged: {
        topButton.focus=true;
    }

    onXChanged: {
        if(x>measureContainer.width-width && measureContainer.width-width>=0)
            x=measureContainer.width-width;
        else if(x<0||measureContainer.width-width<0)
            x=0;
    }

    onYChanged: yChanged();

    onHeightChanged: yChanged();

    function yChanged(){
        if(y>measureContainer.height-height && measureContainer.height-height>=0)
            y=measureContainer.height-height;
        else if(y<0||measureContainer.height-height<0)
            y=0;
        calcHeight();
    }

    function calcOffsetShow(x_){
        return !(x_<=-width||x_>=parent.width+width);
    }

    function calcHeight(){
        if(noteEdit.height>=measureContainer.height-rootItem.y-150){
            if(measureContainer.height-rootItem.y-150>=18)
                noteEdit.height=measureContainer.height-rootItem.y-150;
            else
                noteEdit.height=18;
        }
    }

    function setSelect(){
        if(!set.isSelect)
            controller.setMeasureSelect(set.data.measureID);
    }

    MouseArea{
        anchors.fill: parent
        preventStealing: true
        propagateComposedEvents: false
        onWheel: {
            wheel.accepted=true
        }
        onClicked: setSelect();
    }

    QtObject{
        id: set
        property var data
        property bool isTop: false
        property bool isSelect: false
        property bool isShow: true
        property int showLeftWidth: 100
        property int showRightWidth: 103
        onDataChanged: {
            isSelect=data.isSelect
            if(!isTop)
                isShow=calcOffsetShow(data.offset)
            nameText.text=data.name;
            timeText.text=": "+data.time;
            timeTextToolTip.showText=data.timeDetail;
            startText.text=": "+data.startText;
            startTextToolTip.showText=data.startTextDetail;
            endText.text=": "+data.endText;
            endTextToolTip.showText=data.endTextDetail;
            noteEdit.showText=data.note;
            if(data.isCalc){
                let calcStr=qsTr("计算中...");
                fMinText.text=": "+calcStr;
                fMinTextToolTip.showText=calcStr;
                risingText.text=": "+calcStr;
                fMaxText.text=": "+calcStr;
                fMaxTextToolTip.showText=calcStr;
                fallingText.text=": "+calcStr;
            }else{
                fMinText.text=": "+data.minFreq;
                fMinTextToolTip.showText=data.minFreqDetail;
                risingText.text=": "+data.rising;
                fMaxText.text=": "+data.maxFreq;
                fMaxTextToolTip.showText=data.maxFreqDetail;
                fallingText.text=": "+data.falling;
            }
            if(data.isAutoOffset && visible)
            {
                rootItem.x=data.offset;
                sSignal.getChannelY(data.channelID,0,0);
            }
            for(var i in colorRow.children){
                if(colorRow.children[i].backgroundColor.toString().toUpperCase()===data.dataColor.toString().toUpperCase())
                    colorRow.children[i].checked=true;
            }
        }
    }

    Connections{
        target: sSignal
        function onMeasureRefreshData(measureID_){
            if(measureID_===measureID)
                set.data=measureTreeViewModel.get(measureID)
        }
        function onMeasureRefreshShow(){
            measureTreeViewModel.refresh(measureID);
            set.data=measureTreeViewModel.get(measureID)
        }
        function onMeasureRefreshY(){
            if(set.data.isAutoOffset && visible)
                sSignal.getChannelY(set.data.channelID,0,0);
        }
        function onSendChannelY(channelID,type,y){
            if(typeof(channelID)==="number")
                if(channelID===set.data.channelID && set.data.isAutoOffset && type===0)
                    rootItem.y=y;
        }
    }

    Connections{
        target: measureContainer
        function onWidthChanged(){
            if(rootItem.x>measureContainer.width-rootItem.width)
                rootItem.x=measureContainer.width-rootItem.width;
        }

        function onHeightChanged(){
            if(rootItem.y>measureContainer.height-rootItem.height)
                rootItem.y=measureContainer.height-rootItem.height;
        }
    }

    Component.onCompleted: {
        sSignal.measureRefreshData(measureID);
    }
    Timer{
        id: showTimer
        interval: 50
        onTriggered: set.isTop=!set.isTop;
    }

    Timer{
        id: recoverTimer
        interval: 3000
        onTriggered: {
            copyButton.imageSource="resource/icon/"+Config.tp+"/Copy.png";
        }
    }

    Rectangle{
        id: showRectangle
        color: Config.measurePopupBackground
        border.color: color
        radius: 8
        width: parent.width-2
        height: 111
        z: 10
        anchors{
            left: parent.left
            leftMargin: 1
            top: parent.top
            topMargin: 1
        }
        Rectangle{
            width: parent.width
            height: 8
            anchors.bottom: parent.bottom
            color: showRectangle.color
        }
        MouseArea{
            anchors.fill: parent
            acceptedButtons: Qt.NoButton
            preventStealing: true
            hoverEnabled: true
        }
        MouseArea{
            property point clickPos: "0,0"
            id: showMouseArea
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton
            preventStealing: true
            onPositionChanged: {
                var delta = Qt.point(mouse.x-clickPos.x, mouse.y-clickPos.y)
                if(rootItem.x+delta.x<measureContainer.width-rootItem.width)
                    rootItem.x=Math.max(rootItem.x+delta.x,0);
                else
                    rootItem.x=measureContainer.width-rootItem.width;
                if(rootItem.y+delta.y<measureContainer.height-rootItem.height)
                    rootItem.y=Math.max(rootItem.y+delta.y,0);
                else
                    rootItem.y=measureContainer.height-rootItem.height;

                measureTreeViewModel.set(measureID, "isAutoOffset", false);
            }
            onPressed: clickPos=Qt.point(mouse.x,mouse.y)
            onClicked: setSelect();
        }
        Item{
            width: 40+nameText.implicitWidth
            height: 15
            anchors{
                right: parent.right
                rightMargin: 5
                top: parent.top
                topMargin: 8
            }
            Row{
                spacing: 5
                Text{
                    id: nameText
                    font.pixelSize: 12
                    color: Config.textColor
                    anchors.verticalCenter: parent.verticalCenter
                }
                ImageButton{
                    id: topButton
                    width: 15
                    height: 15
                    imageSource: "resource/icon/"+Config.tp+"/Top.png"
                    imageEnterSource: imageSource
                    onPressed: {
                        imageSource=!set.isTop?"resource/icon/"+Config.tp+"/TopEnter.png":"resource/icon/"+Config.tp+"/Top.png";
                        showTimer.start();
                    }
                }
                ImageButton{
                    id: copyButton
                    width: 15
                    height: 15
                    imageSource: "resource/icon/"+Config.tp+"/Copy.png"
                    imageEnterSource: imageSource
                    onPressed: {
                        imageSource="resource/icon/"+Config.tp+"/CopyEnter.png"
                        var str=timeTitleText.text+"\t\t"+timeText.text+"\n"
                        str+=startTitleText.text+"\t"+startText.text+"\t"+endTitleText.text+"\t"+endText.text+"\n"
                        str+=fMinTitleText.text+"\t"+fMinText.text+(startText.text.length===11&&fMinText.text.length!=11?"\t\t":"\t")+fMaxTitleText.text+"\t"+fMaxText.text+"\n"
                        str+=risingTitleText.text+"\t"+risingText.text+(startText.text.length===11&&risingText.text.length!=11?"\t\t":"\t")+fallingTitleText.text+"\t"+fallingText.text+"\n"
                        clipboard.setText(str);
                        recoverTimer.start();
                    }
                }
            }
        }
        Column{
            width: (parent.width-10)/2-20
            height: parent.height-20
            spacing: 6
            anchors{
                top: parent.top
                topMargin: 15
                left: parent.left
                leftMargin: 20
            }
            Item{
                width: parent.width
                height: timeText.contentHeight
                Text {
                    id: timeTitleText
                    text: "时间(ΔT)"
                    font.pixelSize: 11
                    width: set.showLeftWidth
                    color: Config.textColor
                }
                Text {
                    id: timeText
                    font.pixelSize: 11
                    color: Config.textColor
                    anchors{
                        left: parent.left
                        leftMargin: set.showLeftWidth
                    }
                    MouseArea{
                        id: timeTextMouseArea
                        anchors.fill: parent
                        hoverEnabled: true
                        preventStealing: true
                    }
                    QToolTip{
                        id: timeTextToolTip
                        parent: parent
                        isShow: timeTextMouseArea.containsMouse
                        direction: 2
                        z: 100
                        showTextPixelSize: 11
                    }
                }
            }
            Item{
                width: parent.width
                height: startText.contentHeight
                Text {
                    id: startTitleText
                    text: "起始位置(Start)"
                    font.pixelSize: 11
                    width: set.showLeftWidth
                    color: Config.textColor
                }
                Text {
                    id: startText
                    font.pixelSize: 11
                    color: Config.textColor
                    anchors{
                        left: parent.left
                        leftMargin: set.showLeftWidth
                    }
                    MouseArea{
                        id: startTextMouseArea
                        anchors.fill: parent
                        hoverEnabled: true
                        preventStealing: true
                    }
                    QToolTip{
                        id: startTextToolTip
                        parent: parent
                        isShow: startTextMouseArea.containsMouse
                        direction: 2
                        z: 100
                        showTextPixelSize: 11
                    }
                }
            }
            Item{
                width: parent.width
                height: startText.contentHeight
                Text {
                    id: fMinTitleText
                    text: "最小频率(fMin)"
                    font.pixelSize: 11
                    width: set.showLeftWidth
                    color: Config.textColor
                }
                Text {
                    id: fMinText
                    font.pixelSize: 11
                    color: Config.textColor
                    anchors{
                        left: parent.left
                        leftMargin: set.showLeftWidth
                    }
                    MouseArea{
                        id: fMinTextMouseArea
                        anchors.fill: parent
                        hoverEnabled: true
                        preventStealing: true
                    }
                    QToolTip{
                        id: fMinTextToolTip
                        parent: parent
                        isShow: fMinTextMouseArea.containsMouse
                        direction: 2
                        z: 100
                        showTextPixelSize: 11
                    }
                }
            }
            Item{
                width: parent.width
                height: startText.contentHeight
                Text {
                    id: risingTitleText
                    text: "上升沿个数(Rising)"
                    font.pixelSize: 11
                    width: set.showLeftWidth
                    color: Config.textColor
                }
                Text {
                    id: risingText
                    font.pixelSize: 11
                    color: Config.textColor
                    anchors{
                        left: parent.left
                        leftMargin: set.showLeftWidth
                    }
                }
            }
        }

        Column{
            width: (parent.width-10)/2-10
            height: parent.height-20
            spacing: 6
            anchors{
                top: parent.top
                topMargin: 15
                right: parent.right
                rightMargin: 10
            }
            Item{
                width: parent.width
                height: startText.contentHeight
            }
            Item{
                width: parent.width
                height: startText.contentHeight
                Text {
                    id: endTitleText
                    text: "结束位置(End)"
                    font.pixelSize: 11
                    width: set.showRightWidth
                    color: Config.textColor
                }
                Text {
                    id: endText
                    font.pixelSize: 11
                    color: Config.textColor
                    anchors{
                        left: parent.left
                        leftMargin: set.showLeftWidth
                    }
                    MouseArea{
                        id: endTextMouseArea
                        anchors.fill: parent
                        hoverEnabled: true
                        preventStealing: true
                    }
                    QToolTip{
                        id: endTextToolTip
                        parent: parent
                        isShow: endTextMouseArea.containsMouse
                        direction: 2
                        z: 100
                        showTextPixelSize: 11
                    }
                }
            }
            Item{
                width: parent.width
                height: startText.contentHeight
                Text {
                    id: fMaxTitleText
                    text: "最大频率(fMax)"
                    font.pixelSize: 11
                    width: set.showRightWidth
                    color: Config.textColor
                }
                Text {
                    id: fMaxText
                    font.pixelSize: 11
                    color: Config.textColor
                    anchors{
                        left: parent.left
                        leftMargin: set.showLeftWidth
                    }
                    MouseArea{
                        id: fMaxTextMouseArea
                        anchors.fill: parent
                        hoverEnabled: true
                        preventStealing: true
                    }
                    QToolTip{
                        id: fMaxTextToolTip
                        parent: parent
                        isShow: fMaxTextMouseArea.containsMouse
                        direction: 2
                        z: 100
                        showTextPixelSize: 11
                    }
                }
            }
            Item{
                width: parent.width
                height: startText.contentHeight
                Text {
                    id: fallingTitleText
                    text: "下降沿个数(Falling)"
                    font.pixelSize: 11
                    width: set.showRightWidth
                    color: Config.textColor
                }
                Text {
                    id: fallingText
                    font.pixelSize: 11
                    color: Config.textColor
                    anchors{
                        left: parent.left
                        leftMargin: set.showLeftWidth
                    }
                }
            }
        }
    }
    Rectangle{
        id: bottomRectangle
        width: parent.width-2
        height: textItem.height
        anchors{
            left: parent.left
            leftMargin: 1
            top: showRectangle.bottom
            topMargin: -10
        }
        radius: 8
        color: Config.measurePopupBackground2
        Item{
            id: textItem
            width: parent.width-30
            height: noteEdit.height+colorRow.height-parent.anchors.topMargin+20
            anchors{
                left: parent.left
                leftMargin: 20
                top: parent.top
                topMargin: 6-parent.anchors.topMargin
            }

            QTextEdit{
                id: noteEdit
                width: parent.width
                textColor: Config.subheadColor
                fontSize: 11
                onShowTextChanged: measureTreeViewModel.set(measureID, "note", showText);
                onTextFocusChanged: {
                    if(textFocus)
                        setSelect();
                }
            }

            Item {
                width: 7
                height: 7
                anchors{
                    right: noteEdit.right
                    rightMargin: 1
                    bottom: noteEdit.bottom
                }
                Image {
                    anchors.centerIn: parent
                    source: "../../resource/icon/Drag.png"
                }
                MouseArea{
                    property point clickPos: "0,0"
                    anchors.fill: parent
                    cursorShape: Qt.SizeFDiagCursor
                    acceptedButtons: Qt.LeftButton
                    preventStealing: true
                    onPositionChanged: {
                        var delta = Qt.point(mouse.x-clickPos.x, mouse.y-clickPos.y)
                        if(noteEdit.height+delta.y<=18)
                            noteEdit.height=18;
                        else if(noteEdit.height+delta.y<measureContainer.height-rootItem.y-150)
                            noteEdit.height=noteEdit.height+delta.y;
                    }
                    onPressed: clickPos=Qt.point(mouse.x,mouse.y)
                }
            }

            ImageButton{
                width: 9
                height: 11
                anchors{
                    top: noteEdit.bottom
                    topMargin: 6
                    right: parent.right
                }
                imageSource: "resource/icon/"+Config.tp+"/Delete.png"
                imageEnterSource: "resource/icon/DeleteEnter.png"
                onClicked: sSignal.measureRemove(measureID);
            }

            ButtonGroup{
                buttons: colorRow.children
                onCheckedButtonChanged: {
                    if(checkedButton.backgroundColor!==set.data.dataColor.toString().toUpperCase()){
                        measureTreeViewModel.set(measureID, "dataColor", checkedButton.backgroundColor);
                        sSignal.channelRefresh(set.data.channelID);
                        set.data=measureTreeViewModel.get(measureID)
                    }
                }
            }
            Row{
                id: colorRow
                spacing: 6
                anchors{
                    top: noteEdit.bottom
                    topMargin: 6
                }
                ColorCheckButton{
                    backgroundColor: "#CC0E51"
                }
                ColorCheckButton{
                    backgroundColor: "#DD1D0A"
                }
                ColorCheckButton{
                    backgroundColor: "#FF5309"
                }
                ColorCheckButton{
                    backgroundColor: "#FB9904"
                }
                ColorCheckButton{
                    backgroundColor: "#FABC05"
                }
                ColorCheckButton{
                    backgroundColor: "#0547FF"
                }
                ColorCheckButton{
                    backgroundColor: "#0392CE"
                }
                ColorCheckButton{
                    backgroundColor: "#66B132"
                }
            }
        }
    }
}
