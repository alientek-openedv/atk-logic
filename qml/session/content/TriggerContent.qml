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
import QtQuick.Layouts 1.3
import "../../config"
import "../../style"

Rectangle {
    color: Config.background2Color

    Text {
        id: titleText
        text: qsTr("条件触发")
        font.pixelSize: 18
        color: Config.mouseCheckColor
        anchors{
            top: parent.top
            left: parent.left
        }
    }

    Item{
        anchors{
            top: titleText.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            topMargin: 5
        }
        ButtonGroup{
            buttons: tabBarRow.children
        }
        Connections{
            target: sSettings
            function onIsSimpleTriggerChanged()
            {
                height=(sSettings.isSimpleTrigger?300:610)
            }

            Component.onCompleted: sSettings.isSimpleTriggerChanged();
        }
        RowLayout {
            id: tabBarRow
            anchors{
                left: parent.left
                right: parent.right
                top: parent.top
            }
            height: 38
            spacing: 0
            QTabButton{
                showText: qsTr("简单触发")
                Layout.fillWidth: true
                Layout.fillHeight: true
                checked: sSettings.isSimpleTrigger
                enabled: sConfig.loopState===-1 && !sConfig.isRun
                onCheckedChanged: {
                    if(checked)
                        sSettings.isSimpleTrigger=true
                }
            }
            QTabButton{
                showText: qsTr("高级触发")
                Layout.fillWidth: true
                Layout.fillHeight: true
                checked: !sSettings.isSimpleTrigger
                enabled: sConfig.loopState===-1 && !sConfig.isRun
                onCheckedChanged: {
                    if(checked)
                    {
                        sSettings.isSimpleTrigger=false
                        sSettings.isInstantly=false
                    }
                }
            }
        }
        Item{
            id: triggerPositionItem
            anchors{
                top: tabBarRow.bottom
                topMargin: 20
                left: parent.left
                right: parent.right
            }
            height: 50

            QTextInput{
                id: triggerPositionInput
                width: 26
                height: 16
                disable: sConfig.loopState!==-1 || sConfig.isRun
                onIsFocusChanged: {
                    if(!isFocus)
                    {
                        if(parseInt(showText)<1)
                            triggerPositionSlider.value=1;
                        else if(parseInt(showText)>90)
                            triggerPositionSlider.value=90;
                        else
                            triggerPositionSlider.value=parseInt(showText);
                        showText=triggerPositionSlider.value;
                    }
                }
            }
            Text {
                text: "%"
                font.pixelSize: 12
                color: sConfig.loopState===-1 && !sConfig.isRun?Config.textColor:Config.textDisabledColor
                anchors{
                    verticalCenter: triggerPositionInput.verticalCenter
                    left: triggerPositionInput.right
                    leftMargin: 5
                }
            }

            Row{
                id: triggerPositionRow
                spacing: 10
                anchors{
                    left: parent.left
                    right: parent.right
                    bottom: parent.bottom
                    bottomMargin: 10
                }
                height: 28
                Text {
                    width: 52
                    text: qsTr("触发位置")
                    font.pixelSize: 14
                    color: Config.textColor
                    anchors.verticalCenter: parent.verticalCenter
                }
                QSlider{
                    id: triggerPositionSlider
                    width: 182
                    anchors{
                        bottom: parent.bottom
                        bottomMargin: 7
                    }
                    from: 1
                    to: 90
                    postfixText: "%"
                    isShowText: false
                    enabled: sConfig.loopState===-1 && !sConfig.isRun
                    value: sSettings.settingData["triggerPosition"]
                    onHandleXChanged: {
                        if(handleX>135)
                            triggerPositionInput.x=135+triggerPositionSlider.x
                        else
                            triggerPositionInput.x=handleX+triggerPositionSlider.x
                    }
                    onValueChanged: {
                        sSettings.settingData["triggerPosition"]=value
                        triggerPositionInput.showText=value
                    }
                    Component.onCompleted: triggerPositionInput.x=triggerPositionSlider.x+5
                }
            }
        }
    }
}
