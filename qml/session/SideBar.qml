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
    property var startSet
    property int startSetType
    color: Config.background2Color
    clip: true
    border{
        width:1
        color: Config.lineColor
    }

    Timer{
        id: startTimer
        interval: 10
        onTriggered: {
            sSettings.save();
            sSignal.measureRemoveAll();
            vernierListModel.remove(0);
            progressBar.currentValue=0;
            progressBar.type=0;
            progressBar.visible=controller.start(startSet, startSetType);
        }
    }

    Connections{
        target: Signal
        function onHotKey(type)
        {
            if(Config.tabSelectButton===sessionID_){
                switch(type){
                case Config.ShortcutKey.StartCollecting:
                    if(sessionType_===Config.SessionType.Demo)
                        sSignal.openTestFileSelectPopup();
                    else if(isConnect===1 && sConfig.loopState===-1 && !sConfig.isRun && startButton.enable)
                        sSignal.start(-1);
                    break;
                case Config.ShortcutKey.StopCollecting:
                    if(sessionType_!==Config.SessionType.Demo && isConnect===1)
                    {
                        if((sConfig.isRun||sConfig.loopState===1) && startButton.enable)
                            sSignal.stop(0);
                    }
                    break;
                case Config.ShortcutKey.ParameterMeasure:
                    if(!sConfig.isRun && !sConfig.isChannelMethodRun && !sConfig.isErrorShow)
                        sSignal.measureStart();
                    break;
                case Config.ShortcutKey.DeviceConfig:
                    showSidebarContent(Config.SidebarContentType.Set, true);
                    break;
                case Config.ShortcutKey.ProtocolDecode:
                    showSidebarContent(Config.SidebarContentType.Decode, true);
                    break;
                case Config.ShortcutKey.LabelMeasurement:
                    showSidebarContent(Config.SidebarContentType.Measure, true);
                    break;
                case Config.ShortcutKey.DataSearch:
                    showSidebarContent(Config.SidebarContentType.Search, true);
                    break;
                }
            }
        }

        function onShowError(str){
            sSignal.stop(0);
        }
    }

    Connections{
        target: sSignal
        function onStart(state)
        {
            if(state>=0){
                sSettings.isInstantly=false;
                sSettings.isOne=false;
                switch(state){
                case 0:
                    sSettings.isOne=true;
                    break;
                case 2:
                    sSettings.isInstantly=true;
                }

            }
            sSignal.refreshMethodsRadioGroup();
            start();
        }

        function onSetSidebarContentType(sidebarContentType){
            showSidebarContent(sidebarContentType,true);
        }

        function onSessionSelect(){
            Signal.refreshMenuTabIcon(sConfig.sidebarContentType, sessionType_)
        }

        function onShowCollectorSchedule(schedule, visible_){
            if(visible_)
            {
                startButton.type=2;
                startButton.currentValue=schedule;
            }else
                startButton.type=0;
        }
    }

    Connections{
        target: sConfig
        function onLoopStateChanged()
        {
            if(sConfig.loopState===0)
            {
                startButton.type=0;
                startSend();
            }
            else if(sConfig.loopState===1)
                startButton.type=1;
            else if(sConfig.loopState===-1)
                startButton.type=0;
        }
    }

    Rectangle {
        color: Config.background2Color
        width: parent.width
        height: parent.height
        anchors{
            top: parent.top
            left: parent.left
            leftMargin: 1
        }

        StopProgressButton{
            id: startButton
            enable: !sConfig.isErrorShow && progressBar.type!==7 && !sConfig.isGlitchRemovaling
            anchors{
                left: parent.left
                leftMargin: 8
                top: parent.top
                topMargin: 20
            }
        }

        ImageButton{
            id: setButton
            anchors{
                horizontalCenter: parent.horizontalCenter
                top: startButton.bottom
                topMargin: 20
            }
            width: 30
            height: 30
            imageSource: sConfig.sidebarContentType===Config.SidebarContentType.Set?"resource/icon/SetActivate.png":"resource/icon/Set.png"
            imageEnterSource: sConfig.sidebarContentType===Config.SidebarContentType.Set?"resource/icon/SetActivate.png":"resource/icon/Set.png"
            onPressed: showSidebarContent(Config.SidebarContentType.Set,false)
            QToolTip{
                parent: parent
                isShow: parent.containsMouse
                showText: qsTr("设备配置")
                hotkey: Setting.deviceConfig
            }
        }

        Rectangle{
            id: splitRectangle
            height: 1
            anchors{
                left: parent.left
                top: setButton.bottom
                right: parent.right
                margins: 10
                topMargin: 20
            }
            color: Config.lineColor
        }

        Column{
            spacing: 20
            anchors{
                top: splitRectangle.bottom
                margins: 10
                topMargin: 20
            }
            width: parent.width
            ImageButton{
                id: decodeButton
                anchors.horizontalCenter: parent.horizontalCenter
                width: 30
                height: 30
                imageSource: sConfig.sidebarContentType===Config.SidebarContentType.Decode?"resource/icon/DecodeActivate.png":"resource/icon/Decode.png"
                imageEnterSource: sConfig.sidebarContentType===Config.SidebarContentType.Decode?"resource/icon/DecodeActivate.png":"resource/icon/Decode.png"
                onPressed: showSidebarContent(Config.SidebarContentType.Decode,false)
                QToolTip{
                    parent: parent
                    isShow: parent.containsMouse
                    showText: qsTr("协议解码")
                    hotkey: Setting.protocolDecode
                }
            }
            ImageButton{
                id: measureButton
                anchors.horizontalCenter: parent.horizontalCenter
                width: 30
                height: 30
                imageSource: sConfig.sidebarContentType===Config.SidebarContentType.Measure?"resource/icon/MeasureActivate.png":"resource/icon/Measure.png"
                imageEnterSource: sConfig.sidebarContentType===Config.SidebarContentType.Measure?"resource/icon/MeasureActivate.png":"resource/icon/Measure.png"
                onPressed: showSidebarContent(Config.SidebarContentType.Measure,false)
                QToolTip{
                    parent: parent
                    isShow: parent.containsMouse
                    showText: qsTr("标签测量")
                    hotkey: Setting.labelMeasurement
                }
            }
            ImageButton{
                id: searchButton
                anchors.horizontalCenter: parent.horizontalCenter
                width: 30
                height: 30
                imageSource: sConfig.sidebarContentType===Config.SidebarContentType.Search?"resource/icon/SearchActivate.png":"resource/icon/Search.png"
                imageEnterSource: sConfig.sidebarContentType===Config.SidebarContentType.Search?"resource/icon/SearchActivate.png":"resource/icon/Search.png"
                onPressed: showSidebarContent(Config.SidebarContentType.Search,false)
                QToolTip{
                    parent: parent
                    isShow: parent.containsMouse
                    showText: qsTr("数据搜索")
                    hotkey: Setting.dataSearch
                }
            }
        }
    }

    Item {
        width: parent.width
        height: 15
        visible: Setting.showFPS
        anchors{
            bottom: parent.bottom
            bottomMargin: 8
        }
        Text {
            text: "fps:"+fps.fps
            font.pixelSize: 12
            color: Config.textColor
            anchors{
                horizontalCenter: parent.horizontalCenter
            }
        }
    }

    function showSidebarContent(type, isForcedDisplay)
    {
        if(sConfig.sidebarContentType===type && !isForcedDisplay)
        {
            sConfig.sidebarContentType=Config.SidebarContentType.Null;
            sConfig.isSidebarContentShow=!sConfig.isSidebarContentShow;
        }else{
            sConfig.sidebarContentType=type;
            sConfig.isSidebarContentShow=true;
        }
        Signal.refreshMenuTabIcon(sConfig.sidebarContentType, sessionType_);
    }

    function start(){
        if(sConfig.isRun || sessionType_===Config.SessionType.Demo || (sessionType_!==Config.SessionType.Demo && isConnect!==1))
            return;
        switch(sessionType_){
        case Config.SessionType.Demo:
            break;
        case Config.SessionType.Device:
            var temp={};
            temp["settingData"]=sSettings.settingData;
            temp["settingData"]["RLE"]=sConfig.isRLE;
            temp["channelsSet"]=sSettings.channelsSet;
            if(sSettings.isSimpleTrigger){
                temp["isInstantly"]=sSettings.isInstantly;
                startSetType=0;
            }else{
                startSetType=1;
            }
            startSet=temp;
            startButton.loopNumRecode=temp["settingData"]["intervalTime"];
            if(sSettings.isOne)
                startSend();
            else
            {
                if(sSettings.isSimpleTrigger&&sSettings.isInstantly)
                    startSend();
                else{
                    sConfig.loopState=0;
                    return;
                }
            }
            break;
        case Config.SessionType.File:
            break;
        }
    }

    function startSend(){
        sConfig.isRun=true;
        progressBar.currentValue=0;
        progressBar.type=7;
        progressBar.visible=true;
        sConfig.isStopDecode=true;
        controller.stopAllDecode();
        decodeTableModel.stopAll(false);
        searchTableModel.stopAll();
        sSignal.measureEnd();
        sSignal.vernierCreate(2);
        sSignal.closeVernierPopup();
        startTimer.start();
    }
}
