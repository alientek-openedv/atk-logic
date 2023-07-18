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
import "../../config"
import "../../style"

Rectangle {
    id: measureMain
    color: Config.background2Color
    height: parent.height

    onHeightChanged: {
        if(measureTreeView.height<70 && measureTreeView.height>=0)
        {
            var temp=measureTreeView.height-70;
            vernierView.height+=temp;
        }
    }


    Connections{
        target: controller

        function onSendVernierTriggerPosition(position) {
            vernierCalcRow.calcVernier();
        }
    }

    Connections{
        target: sSignal

        function onShowMouseMeasurePopup(x_,channelID,period,freq,duty){
            periodText.text=": "+period;
            freqText.text=": "+freq;
            dutyText.text=": "+duty;
        }

        function onVernierDataChanged(vernierID){
            vernierCalcRow.calcVernier();
        }
    }

    Connections{
        target: sConfig
        function onIsLoadSettingChanged(){
            if(!sConfig.isLoadSetting){
                mouseMeasureQCheckBox.isCheck=sSettings.isMouseMeasure;
            }
        }

        function onIsRunChanged(){
            menuPopup.close();
        }
    }

    Column{
        id: topColumn
        spacing: 20
        anchors{
            top: parent.top
            topMargin: 14
            left: parent.left
            leftMargin: 14
            right: parent.right
            rightMargin: 14
        }

        Text {
            text: qsTr("标签测量")
            font.pixelSize: 18
            color: Config.mouseCheckColor
        }

        Item{
            width: parent.width
            height: mouseMeasureColumn.height+mouseMeasureTitle.contentHeight+mouseMeasureQCheckBox.height+40
            Text {
                id: mouseMeasureTitle
                text: qsTr("鼠标测量")
                font.pixelSize: 16
                color: Config.textColor
            }
            QCheckBox{
                id: mouseMeasureQCheckBox
                buttonText: qsTr("使能鼠标跟随测量")
                isCheck: sSettings.isMouseMeasure
                anchors{
                    top: mouseMeasureTitle.bottom
                    topMargin: 20
                }
                onIsCheckChanged: {
                    sSettings.isMouseMeasure=isCheck;
                    sSignal.setMouseMeasure(isCheck)
                }
            }

            Column{
                id: mouseMeasureColumn
                spacing: 10
                anchors{
                    top: mouseMeasureQCheckBox.bottom
                    topMargin: 20
                }
                Text {
                    text: qsTr("周期")
                    color: Config.textColor
                    font.pixelSize: 14
                }
                Text {
                    text: qsTr("频率")
                    color: Config.textColor
                    font.pixelSize: 14
                }
            }
            Column{
                id: mouseMeasureShowColumn
                spacing: mouseMeasureColumn.spacing
                anchors{
                    top: mouseMeasureColumn.anchors.top
                    topMargin: mouseMeasureColumn.anchors.topMargin
                    left: mouseMeasureColumn.right
                    leftMargin: 5
                }
                Text {
                    id: periodText
                    color: Config.textColor
                    font.pixelSize: 14
                }
                Text {
                    id: freqText
                    color: Config.textColor
                    font.pixelSize: 14
                }
            }

            Text {
                id: dutyT
                text: qsTr("占空比")
                color: Config.textColor
                font.pixelSize: 14
                anchors{
                    bottom: mouseMeasureColumn.bottom
                    topMargin: 28
                    left: mouseMeasureColumn.right
                    leftMargin: 96
                }
            }

            Text {
                id: dutyText
                color: Config.textColor
                font.pixelSize: 14
                anchors{
                    top: dutyT.top
                    left: dutyT.right
                    leftMargin: 5
                }
            }
        }

        Rectangle{
            height: 1
            width: parent.width
            color: Config.lineColor
        }

        Item{
            width: 240
            height: timeTitle.contentHeight
            Text {
                id: timeTitle
                text: qsTr("时间标签")
                font.pixelSize: 16
                color: Config.textColor
            }

            ImageButton{
                id: moreButton
                imageSource: "resource/icon/"+Config.tp+"/More.png"
                imageEnterSource: "resource/icon/"+Config.tp+"/MoreEnter.png"
                imageDisableSource: "resource/icon/"+Config.tp+"/MoreDisable.png"
                width: 10
                height: 16
                enabled: sConfig.loopState===-1 && !sConfig.isRun
                anchors{
                    right: parent.right
                    verticalCenter: parent.verticalCenter
                }
                onPressed: menuPopup.open();
            }
            QMenuPopup{
                id: menuPopup
                parent: moreButton
                showShortcutKey: true
                data: [{"showText":qsTr("添加标签"),"shortcutKey":Setting.vernierCreate,"seleteType":sConfig.isChannelMethodRun?-1:0,"buttonIndex":1},
                    {"showText":qsTr("删除全部"),"shortcutKey":"","seleteType":0,"buttonIndex":2}]
                onSelect: {
                    switch(index){
                    case 1:
                        sSignal.vernierCreate(0);
                        break;
                    case 2:
                        sSignal.vernierCreate(2);
                        vernierListModel.removeAll(false);
                        break;
                    }
                }
            }
        }
    }

    Connections{
        target: vernierListModel
        function onCountChanged(){
            vernierLeftComboBox.modelCount=vernierListModel.count();
        }
    }

    Row{
        id: vernierCalcRow
        anchors{
            top: topColumn.bottom
            topMargin: 10
            left: parent.left
            leftMargin: 10
        }
        height: vernierLeftComboBox.height
        width: parent.width
        spacing: 5
        QComboBox{
            id: vernierLeftComboBox
            width: 68
            model: vernierListModel
            showArrows: false
            onCurrentModelChildrenChanged: vernierCalcRow.calcVernier();
        }
        Text {
            text: "-"
            color: Config.textColor
            font.pixelSize: 12
            anchors.verticalCenter: parent.verticalCenter
        }
        QComboBox{
            id: vernierRightComboBox
            width: 68
            model: vernierListModel
            showArrows: false
            modelCount: vernierLeftComboBox.modelCount
            onCurrentModelChildrenChanged: vernierCalcRow.calcVernier()
        }
        Text {
            text: "="
            color: Config.textColor
            font.pixelSize: 12
            anchors.verticalCenter: parent.verticalCenter
        }
        Text {
            id: vernierCalcText
            color: Config.textColor
            font.pixelSize: 12
            anchors.verticalCenter: parent.verticalCenter
        }

        function calcVernier(){
            vernierCalcText.text=time22Unit(vernierListModel.get(vernierLeftComboBox.currentIndex).position-
                                            vernierListModel.get(vernierRightComboBox.currentIndex).position, 3)
        }
    }

    QVernierView{
        id: vernierView
        anchors{
            top: vernierCalcRow.bottom
            topMargin: 5
        }
        height: 110
        width: parent.width
        model: vernierListModel
    }
    Rectangle{
        id: vernierSplit
        height: 3
        width: parent.width
        anchors.top: vernierView.bottom
        color: "transparent"
        Rectangle{
            height: 1
            width: parent.width
            anchors.verticalCenter: parent.verticalCenter
            color: Config.lineColor
        }
        MouseArea{
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton
            cursorShape: Qt.SizeVerCursor
            hoverEnabled: true
            preventStealing: true
            onMouseYChanged: {
                if(pressed)
                {
                    if(vernierView.height+mouseY<56)
                        vernierView.height=56;
                    else if(measureTreeView.height-mouseY<70)
                    {
                        var temp=measureTreeView.height-70;
                        vernierView.height+=temp;
                    }
                    else
                        vernierView.height+=mouseY;
                }
            }
        }
    }

    Column{
        width: parent.width
        height: parent.height-topColumn.height-vernierView.height-vernierSplit.height-vernierCalcRow.height
        spacing: 10
        anchors{
            top: vernierSplit.bottom
            topMargin: 20
        }
        Item{
            id: measureTitleItem
            width: 245
            height: measureTitle.contentHeight
            anchors{
                left: parent.left
                leftMargin: 10
            }

            Text {
                id: measureTitle
                text: qsTr("参数测量")
                font.pixelSize: 16
                color: Config.textColor
            }
            ImageButton{
                id: measureChannelAdd
                anchors{
                    right: parent.right
                    verticalCenter: parent.verticalCenter
                }
                enabled: moreButton.enabled
                imageSource: "resource/icon/Add.png"
                imageEnterSource: "resource/icon/"+Config.tp+"/AddEnter.png"
                imageDisableSource: "resource/icon/"+Config.tp+"/AddDisable.png"
                width: 16
                height: 16
                onPressed: measureMenuPopup.visible=true
            }

            QMenuPopup{
                id: measureMenuPopup
                parent: measureChannelAdd
                showShortcutKey: true
                data: [{"showText":qsTr("添加通道标记"),"shortcutKey":Setting.parameterMeasure,"seleteType":sConfig.isChannelMethodRun?-1:0,"buttonIndex":0},
                    {"showText":qsTr("删除全部"),"shortcutKey":"","seleteType":0,"buttonIndex":1}]
                onSelect: {
                    switch(index){
                    case 0:
                        if(!sConfig.isRun && !sConfig.isChannelMethodRun)
                            sSignal.measureStart();
                        break;
                    case 1:
                        sSignal.measureEnd();
                        sSignal.measureRemoveAll();
                    }
                }
            }
        }
        QMeasureTreeView{
            id: measureTreeView
            height: parent.height-measureTitleItem.height-65
            width: parent.width
            model: measureTreeViewModel
        }

    }
}
