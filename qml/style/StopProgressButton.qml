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

import QtQuick 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import "../config"

Item{
    width: 44
    height: 44

    property int type: 0
    property int loopNumRecode
    property alias maximumValue: set.maximumValue
    property real currentValue: 0
    property bool enable: true

    onCurrentValueChanged: {
        txtProgress.text=Math.trunc(currentValue).toString();
        canvas.requestPaint();
    }

    onTypeChanged: {
        set.secondaryColor=set.defaultSecondaryColor
        set.primaryColor=set.defaultPrimaryColor
        switch(type){
        case 0:
            loopTimer.stop();
            break;
        case 1:
            maximumValue=loopNumRecode;
            currentValue=loopNumRecode;
            set.containsMouse_=false;
            loopTimer.loopNum=loopNumRecode;
            loopTimer.start();
            break;
        case 2:
            maximumValue=100;
            set.primaryColor=set.type2PrimaryColor
            set.secondaryColor=set.type2SecondaryColor
            break;
        }
        let recode=currentValue;
        currentValue=-100;
        currentValue=recode;
    }

    QtObject{
        id: set
        property real centerWidth: width / 2
        property real centerHeight: height / 2
        property real radius: Math.min(canvas.width-12, canvas.height-12) / 2
        property real angle: (type===1?maximumValue-currentValue-minimumValue:currentValue-minimumValue) / (maximumValue - minimumValue) * 2 * Math.PI
        property real angleOffset: -Math.PI / 2

        property color defaultPrimaryColor: "white"
        property color defaultSecondaryColor: Config.mouseCheckColor
        property color type2PrimaryColor: "white"
        property color type2SecondaryColor: "#26ffffff"

        property color primaryColor: defaultPrimaryColor
        property color secondaryColor: defaultSecondaryColor

        property real minimumValue: 0
        property real maximumValue: 100

        property bool containsMouse_: false
    }

    Connections{
        target: sSignal
        function onStop(state){
            if(state===0){
                if(imageButton.visible && imageButton.enabled){
                    if(sConfig.isRun)
                        sSignal.stop(1);
                }
                else if(mouseItem.visible && mouseArea.enabled)
                    mouseArea.onClicked(null);
            }
        }
    }

    ImageButton{
        id: imageButton
        anchors.fill: parent
        visible: type===0 || !enable
        imageSource: sessionType_===Config.SessionType.Demo?"resource/icon/StartFile.png":sConfig.isRun?"resource/icon/Stop.png":
                                                                                                         sSettings.isInstantly?"resource/icon/FastStart.png":sSettings.isOne?"resource/icon/Start.png":"resource/icon/LoopStart.png"
        imageEnterSource: imageSource
        imageDisableSource: "resource/icon/StartDisable.png"
        enabled: (sessionType_===Config.SessionType.Demo || (sessionType_!==Config.SessionType.Demo && isConnect===1))&&enable
        onClicked: {
            if(sessionType_===Config.SessionType.Demo)
                sSignal.openTestFileSelectPopup();
            else if(sConfig.isRun)
                sSignal.stop(1);
            else
                start();
        }
        QToolTip{
            parent: parent
            isShow: parent.containsMouse
            showText: sessionType_===Config.SessionType.Demo?qsTr("打开Demo文件"):sConfig.isRun?qsTr("停止"):sSettings.isInstantly?qsTr("立即采集"):sSettings.isOne?qsTr("单次采集"):qsTr("循环采集")
            hotkey: sConfig.isRun?Setting.stopCollecting:Setting.startCollecting
        }
    }

    Item {
        id: mouseItem
        anchors.fill: parent
        visible: type!=0 && enable

        Timer{
            property double loopNum:10
            id: loopTimer
            interval: 100
            repeat: true
            onTriggered: {
                loopNum-=0.1;
                currentValue=loopNum;
                if(loopNum<=0)
                {
                    running=false;
                    sConfig.loopState=0;
                }
            }
        }

        MouseArea {
            id: mouseArea
            anchors.fill: parent
            hoverEnabled: true
            enabled: enable
            onClicked: {
                if(type===1){
                    sConfig.loopState=-1;
                    sSignal.collectComplete();
                }
                else
                    sSignal.stop(1);
            }

            onEntered: {
                set.containsMouse_=true;
                canvas.requestPaint()
            }

            onExited: {
                set.containsMouse_=false;
                canvas.requestPaint()
            }

            cursorShape: Qt.PointingHandCursor
        }

        Image {
            source: "../../resource/icon/StopBackground.png"
            anchors.fill: parent
        }

        Canvas {
            id: canvas
            antialiasing: true
            width: parent.width
            height: parent.height
            onPaint: {
                var ctx = getContext("2d");
                ctx.save();
                ctx.clearRect(0, 0, canvas.width, canvas.height);
                ctx.fillStyle = set.primaryColor;
                ctx.lineWidth = 2;

                if(set.containsMouse_)
                    ctx.fillRect(width/2-5, height/2-5, 10, 10);
                ctx.beginPath();
                if(type===2)
                    ctx.strokeStyle = set.secondaryColor;
                else
                    ctx.strokeStyle = set.primaryColor;
                ctx.arc(set.centerWidth,
                        set.centerHeight,
                        set.radius,
                        set.angleOffset + set.angle,
                        set.angleOffset + 2*Math.PI);
                ctx.stroke();
                ctx.beginPath();
                if(type===2)
                    ctx.strokeStyle = set.primaryColor;
                else
                    ctx.strokeStyle = set.secondaryColor;
                ctx.arc(set.centerWidth,
                        set.centerHeight,
                        set.radius,
                        set.angleOffset,
                        set.angleOffset + set.angle);
                ctx.stroke();
                ctx.restore();
            }

            Text {
                id: txtProgress
                anchors.centerIn: parent
                visible: !set.containsMouse_
                font.pixelSize: 16
                color: set.primaryColor
            }
        }
    }
}



