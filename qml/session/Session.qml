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
import atk.qml.Controls 1.0
import atk.qml.Model 1.0
import "../config"
import "../style"
import "./channel"

Item {
    property string sessionName_
    property string sessionID_
    property var sessionType_
    property var sessionChannelCount_
    property var sessionPort_
    property var sessionLevel_
    property var sessionUSB
    property bool demoShowData: false
    property int isConnect: 0
    property string stateText
    property string filePath_
    property alias sessionSignal: sSignal
    property alias sessionController: controller
    property alias messageBox: errorDialog

    id: sessionItem

    function resetCompleted(){
        if(sessionType_===Config.SessionType.Device)
            Config.isHardwarePageConnect=true;
    }

    Component.onCompleted: {
        for(var i=0;i<sessionChannelCount_;i++)
            channelModel.append({"showText": "D"+i.toString(), "cost": i})
        sSettings.init();
        if(sessionType_!==Config.SessionType.Device)
            sSettings.lockSave();
        resetCompleted();
    }

    Component.onDestruction: {
        if(isConnect!=2 && sessionType_===Config.SessionType.Device)
            sSettings.save();
        if(sConfig.setPath!=="")
            controller.deleteSetDirectory(sConfig.setPath)
    }

    Timer{
        id: refreshZoomTimer
        interval: 100
        onTriggered: refreshZoom(1);
    }

    SSettings{
        id: sSettings
        category: sessionName_
        Component.onCompleted: {
            sConfig.isLoadSetting=true;
            load();
            init_();
            sSignal.sideBarCanLoader();
            sessionLoader.sourceComponent=sessionComponent
            loadSettingEndTimer.start();
        }

        function init_(){
            var isAddFavoritesList=true;
            var temp={};
            var i=0;
            if(channelsSet.length == 0){
                isFirst=true;
                for(i=0;i<sessionChannelCount_;i++){
                    temp={};
                    temp.id=i;
                    temp.enable=true;
                    temp.triggerType=Config.ChannelTriggerButton.Null;
                    channelsSet.push(temp);
                }
            }else
                isFirst=false;
            isSimpleTrigger=true;
            if(isFirst){
                settingData["selectTimeIndex"]=8;
                settingData["selectHzIndex"]=9;
                settingData["thresholdLevel"]=3.3;
                settingData["intervalTime"]=0;
                settingData["selectThresholdLevelIndex"]=2;
                settingData["isBuffer"]=false;
                settingData["model"]=0;
                settingData["triggerPosition"]=1;
                settingData["RLE"]=false;
                isSimpleTrigger=true;
                pwmData.push({"hz":"1","unit":2,"duty":50});
                pwmData.push({"hz":"1","unit":2,"duty":50});
                channelsDataColor=[...Config.channelDataColor];
            }else{
                if(settingData["selectTimeIndex"]>30||settingData["selectTimeIndex"]<0)
                    settingData["selectTimeIndex"]=8;
                if(settingData["selectHzIndex"]>13||settingData["selectHzIndex"]<0)
                    settingData["selectHzIndex"]=8;
                if(settingData["thresholdLevel"]>5||settingData["thresholdLevel"]<-5)
                    settingData["thresholdLevel"]=3.3;
                if(settingData["intervalTime"]>10||settingData["intervalTime"]<0)
                    settingData["intervalTime"]=0;
                if(settingData["triggerPosition"]>90||settingData["triggerPosition"]<1)
                    settingData["triggerPosition"]=1;
                if(settingData["selectThresholdLevelIndex"]>7||settingData["selectThresholdLevelIndex"]<0)
                    settingData["selectThresholdLevelIndex"]=2;
                var list=[1,2,5,8]
                if(!list.concat(channelHeightMultiple))
                    channelHeightMultiple=1;
                if(favoritesList.length<=3 && favoritesList.length>=0)
                    isAddFavoritesList=false;
                for(let j in favoritesList)
                {
                    if(favoritesList[j]==="I2C")
                    {
                        favoritesList[j]="I²C"
                        break;
                    }
                }
                {
                    let isReset=false;
                    if(channelsDataColor.length!==sessionChannelCount_)
                        isReset=true;
                    else{
                        for(i=0;i<channelsDataColor.length;i++){
                            let r = new RegExp("^#[0-9a-fA-F]{6}$");
                            if(channelsDataColor[i].match(r)===null)
                            {
                                isReset=true;
                                break;
                            }
                        }
                    }
                    if(isReset)
                        channelsDataColor=[...Config.channelDataColor];
                }
            }
            if(isAddFavoritesList){
                favoritesList=[];
                favoritesList.push("UART");
                favoritesList.push("I²C");
                favoritesList.push("SPI");
            }
        }
    }
    SConfig{
        id: sConfig
        type: sessionType_
        name: sessionName_
        Component.onCompleted: {
            var i=0;
            availableChannels=[];
            for(i=0;i<sessionChannelCount_;i++){
                availableChannels.push(i);
            }
        }

        onIsRunChanged: refreshMenuPopupRunState();

        onLoopStateChanged: refreshMenuPopupRunState();

        onFilePathChanged: refreshMenuPopupRunState();
    }

    SSignal{
        id: sSignal
        onStop: {
            if(state===1)
                progressBar.closeButtonClick();
        }

        onCloseNotDataChannels: sConfig.isCloseNotDataChannels=true;

        onRefreshZoom: sessionItem.refreshZoom(state);

        onSessionSelect: {
            controller.onSessionSelect();
            refreshMenuPopupRunState();
        }

        onCollectComplete: {
            if(controller.isGlitchRemoval()){
                sConfig.isGlitchRemovaling=true;
                controller.restartGlitchRemoval();
            }else{
                sConfig.isStopDecode=false;
                controller.restartAllDecode();
            }
            sConfig.isRun=false;
        }

        onExit: {
            sConfig.isExit=true;
            controller.threadStop(true);
            decodeTableModel.stopAll(false);
            searchTableModel.stopAll();
            controller.stopAllDecode();
            Signal.sessionCloseClicked(sessionID_,1);
        }
    }

    SessionError{
        id: sessionError
        objectName: "sessionError"+sessionID_
        onError_msgChanged: {
            if(error_msg!="")
            {
                console.log("qml::sessionError:"+error_msg);
                errorDialog.titleText=qsTr("错误");
                errorDialog.showText=error_msg;
                errorDialog.type=0;
                errorDialog.visible=true;
            }
            error_msg="";
        }
        Component.onCompleted: controller.errorInit();
    }

    Timer{
        id: refreshDisplayRangeTimer
        interval: 10
        onTriggered: {
            if(sConfig.isExit)
                return;
            if(Setting.collectZoom)
                controller.onRefreshZoom(1);
            else
                controller.resetDisplayRange();
        }
    }

    Timer{
        property var showStart
        id: refreshChannelTimer
        interval: 25
        onTriggered: {
            if(sConfig.isExit)
                return;
            controller.setShowStart(showStart);
        }
    }

    Timer{
        id: loadSettingEndTimer
        interval: 500
        onTriggered: sConfig.isLoadSetting=false;
    }

    SessionController{
        property string pathRecode
        property string categoryRecode
        id: controller
        sessionName: sessionName_
        sessionType: sessionType_
        sessionPort: parseInt(sessionPort_)
        filePath: filePath_
        channelCount: sessionChannelCount_

        onIsInitChanged: {
            sessionID_=sessionID;
        }

        onSaveSessionSettings: {
            pathRecode=sSettings.fileName;
            sSettings.fileName=path+"/set.ini"
            let islock=sSettings.islock();
            if(islock)
                sSettings.unlockSave();
            sSettings.save();
            if(islock)
                sSettings.lockSave();
            sSettings.fileName=pathRecode;
        }

        onLoadSessionSettings: {
            pathRecode=sSettings.fileName;
            categoryRecode=sSettings.category;
            sConfig.setPath=path;
            sSettings.fileName=path+"/set.ini"
            sSettings.category=name;
            sConfig.isLoadSetting=true;
            sSettings.load();
            sSettings.init_();
            sSettings.fileName=pathRecode;
            sSettings.category=categoryRecode;
            sSettings.save();
            loadSettingEndTimer.start();
        }

        onSendDeviceRecvSchedule: {
            if(sConfig.isExit)
                return;
            Signal.runScheduleMove(sessionID_,schedule!==100);
            if(type!==1&&type!==2){
                var isSet=progressBar.type!==type;
                if(isSet)
                    progressBar.currentValue=0;
            }
            progressBar.type=type;
            progressBar.currentValue=schedule;
            if(type===3){
                if(schedule===100){
                    if(!sConfig.pwm0Start&&!sConfig.pwm1Start)
                        controller.fpgaDormancy();
                    sSignal.setLiveFollowing(false,false);
                    if(!sConfig.isBuffer)
                        sSignal.showCollectorSchedule(0,false);
                    if(sConfig.loopState===0)
                        sConfig.loopState=1;
                    if(sConfig.loopState===-1)
                        sSignal.collectComplete();
                    else
                        sConfig.isRun=false;
                    refreshDisplayRangeTimer.start();
                }else if(!sConfig.isBuffer){
                    sSignal.showCollectorSchedule(schedule,true);
                    if(progressBar.visible){
                        sSignal.setLiveFollowing(true,true);
                        progressBar.visible=false;
                    }
                }
            }else if(type===4||type===5||type===6){
                if(schedule===100)
                {
                    progressBar.visible=false;
                    
                    if(type===6){
                        sConfig.isStopDecode=false;
                        sConfig.isGlitchRemovaling=false;
                        controller.restartAllDecode();
                        controller.startDecodeTmp();
                        sConfig.isRun=false;
                    }else if(type===5)
                    {
                        refreshDisplayRangeTimer.start();
                        sSignal.closeNotDataChannels();
                        for(let i=0;i<sessionChannelCount_;i++)
                            sSignal.channelNameSignalChanged(i, controller.getChannelName(i));
                    }else if(type===4)
                    {
                        if(!connectMessagePopup.isOpen)
                            connectMessagePopup.isOpen=true;
                        connectMessagePopup.type=5;
                        connectMessagePopup.schedule=0;
                        controller.deleteSetDirectory(tempDir+"/temp/save")
                        if(Config.isExit&&state_===1)
                            Signal.closeAppSaveNext();
                    }
                    sSignal.channelRefresh(-1);
                    sConfig.isRun=false;
                }
                else if(!progressBar.visible)
                {
                    sConfig.isRun=true;
                    progressBar.visible=true;
                }
            }
        }

        onMeasureDataChanged: {
            if(sConfig.isExit)
                return;
            measureTreeViewModel.refresh();
        }

        onSendVernierTriggerPosition: {
            if(sConfig.isExit)
                return;
            vernierListModel.set(0,"position",position);
            vernierListModel.set(0,"positionStr",time22Unit(position, 6));
            vernierListModel.set(0,"visible_",true);
            sSignal.vernierDataChanged(0);
        }

        onChannelNameModification: sSignal.channelNameSignalChanged(channelID, name)

        onAppendMeasure: {
            if(sConfig.isExit)
                return;
            measureTreeViewModel.append();
            sSignal.measurePopupAppend(measureID);
            sSignal.measureRefreshShow();
            sSignal.channelRefresh(-1);
        }

        onAppendVernier: {
            if(sConfig.isExit)
                return;
            vernierListModel.append();
        }

        onSessionDrawUpdate: {
            if(sConfig.isExit)
                return;
            if(isPoll)
                sSignal.channelRefresh(-2);
            else
                sSignal.channelRefresh(-1);
        }

        onZoomUnitShow: Signal.showZoomString(text);

        onTimerDrawUpdate: {
            refreshChannelTimer.showStart=showStart
            if(!refreshChannelTimer.running)
                refreshChannelTimer.start();
        }

        onCalcMeasureDataComplete: sSignal.measureRefreshData(measureID);

        onMeasureSelectChanged: {
            sSignal.measureRefreshShow();
            sSignal.channelRefresh(-1);
        }
    }

    QMessageBox{
        id: errorDialog
        anchors.fill: parent
        onVisibleChanged: {
            if(!visible)
                sessionError.isClose=true
            else
                sSignal.stop(0);
            sConfig.isErrorShow=visible
        }
    }

    QProgressBarWindow{
        id: progressBar
        width: parent.width-sidebar.width
        height: parent.height
        currentValue: 0
        onCloseButtonClick: {
            if(type<4){
                sConfig.loopState=-1;
                controller.stop();
                sSignal.setLiveFollowing(false,false);
            }else{
                controller.threadStop();
            }
        }
    }

    Connections{
        target: root
        function onDeviceToFile(windowID, newPort){
            if(windowID===sessionID_)
            {
                sSignal.stop(0);
                sessionPort_=newPort;
                sessionType_=Config.SessionType.File;
                sConfig.pwm1Start=false;
                sConfig.pwm0Start=false;
                isConnect=2;
                sSettings.save();
                sSettings.lockSave();
                sSignal.sessionSelect();
                Config.isHardwarePageConnect=false;
            }
        }

        function onLanguageChanged(){
            controller.forceRefresh();
        }
    }

    Connections{
        target: Signal
        function onSaveData(path,savePath,saveType)
        {
            if(Config.tabSelectButton===sessionID_ && !sConfig.isRun && sConfig.loopState===-1 && controller.hasData()){
                sConfig.isRun=true;
                if(savePath)
                    sConfig.filePath=path;
                if(saveType===0||saveType===2)
                    controller.saveData(path, sessionName_);
                else if(saveType===1)
                    controller.saveDecodeTable(path);
            }
        }

        function onImportProject(path){
            if(!sConfig.isRun && sConfig.loopState===-1 && isConnect===1 && sessionType_===Config.SessionType.Device){
                if(sConfig.setPath!=="")
                    controller.deleteSetDirectory(sConfig.setPath);
                sConfig.setPath="";
                sSignal.resetSearch();
                sSignal.removeAllDecode();
                sSignal.measureRemoveAll();
                vernierListModel.removeAll(true);
                controller.openFile(path);
                sConfig.filePath=path;
            }
        }

        function onResetConnectDevice(sessionID, port, complete){
            if(!complete && sessionID===sessionID_ && isConnect===2 && sessionType_===Config.SessionType.File){
                sessionPort_=port;
                sessionType_=Config.SessionType.Device;
                controller.sessionPort=port;
                controller.resetBindingPort();
                sSettings.unlockSave();
                sSettings.load();
                isConnect=1;
                sConfig.pwm1Start=false;
                sConfig.pwm0Start=false;
                resetCompleted();
                Signal.resetConnectDevice(sessionID, port, true);
                Signal.tabSelected(sessionID);
            }
        }
    }

    Loader{
        id: sessionLoader
        width: parent.width-sidebar.width-(sConfig.isSidebarContentShow?sidebarContent.width:0)
        height: parent.height
        anchors.left: parent.left
        asynchronous: true  
    }

    SideBarContent{
        id: sidebarContent
        anchors.right: sidebar.left
        width: Config.sideBarContentWidth
        height: parent.height+1
        visible: false
        clip: true
    }

    Connections{
        target: sConfig
        function onIsSidebarContentShowChanged()
        {
            if(sConfig.isSidebarContentShow)
            {
                sidebarContent.visible=true;
                sidebarClose.stop();
                sidebarOpen.start();
            }
            else{
                if(sidebarContent.x!==sidebarClose.to){
                    sidebarOpen.stop();
                    sidebarClose.start();
                }
            }
        }
    }

    XAnimator{
        target: sidebarContent;
        id: sidebarClose
        from: sidebar.x-sidebar.width
        to: sidebar.x
        duration: 100
        alwaysRunToEnd: false
        onFinished: sidebarContent.visible=false;
    }

    XAnimator {
        target: sidebarContent;
        id: sidebarOpen
        from: sidebar.x
        to: sidebar.x-sidebar.width
        duration: 100
        alwaysRunToEnd: false
    }

    SideBar{
        id: sidebar
        width: Config.sideBarWidth
        height: parent.height+1
        anchors.right: parent.right
    }

    ListModel{
        id: vernierModel
        ListElement { showText: "Start"; cost: -1 }
        ListElement { showText: "End"; cost: -1 }
    }

    ListModel{
        id: channelModel
    }

    MeasureTreeViewModel{
        id: measureTreeViewModel
        Component.onCompleted: init(sessionID_);
    }

    VernierListModel{
        id: vernierListModel
        Component.onCompleted: init(sessionID_);
        onClosePopup: sSignal.closeVernierPopup();
        onCountChanged: sSignal.channelRefresh(-1);
    }

    Connections{
        target: vernierListModel
        function onCountChanged()
        {
            vernierListShowModel.clear();
            vernierListShowModel.append({"showText":"Start","cost":-2,"id":-2});
            vernierListShowModel.append({"showText":"End","cost":-1,"id":-1});
            for(var i=0;i<vernierListModel.count();i++){
                var data=vernierListModel.get(i);
                vernierListShowModel.append({"showText":data.name,"cost":data.position,"id":data.vernierID})
            }
            sSignal.searchModelChanged();
        }
    }

    ListModel{
        id: vernierListShowModel
    }

    DecodeTableModel{
        id: decodeTableModel
        Component.onCompleted: init(sessionID_);
    }

    DecodeListModel{
        id: decodeSearchListModel
        Component.onCompleted: refresh();
    }

    Timer{
        id: searchModelRefreshCountTimer
        interval: 500
        onTriggered: searchTableModel.refreshCount();
    }

    SearchTableModel{
        id: searchTableModel
        Component.onCompleted: init(sessionID_);
        onRefreshCountTimer: {
            if(isRun){
                if(!searchModelRefreshCountTimer.running)
                    searchModelRefreshCountTimer.start();
            }else
                searchModelRefreshCountTimer.stop();
        }
    }


    
    Component{
        id: sessionComponent

        Item{
            id: sessionComponentItem

            onWidthChanged: {
                testFileSelectPopup.refReshPosition();
                setTipPopup.refreshWidth();
            }

            onHeightChanged: {
                testFileSelectPopup.refReshPosition();
                setTipPopup.refreshHeight();
            }

            function refreshScrollBar(){
                var showConfig=controller.getShowConfig();
                if(horizontalScrollBar.position!==showConfig.position){
                    horizontalScrollBar.positionSet=false;
                    horizontalScrollBar.position=showConfig.position;
                }
                horizontalScrollBar.size=Math.min(showConfig.size,1);
            }
            Connections{
                target: sSignal
                function onMeasurePopupAppend(measureID){
                    Qt.createQmlObject('import "../style"; MeasurePopup{measureID:'+measureID+'}',measureContainer)
                }

                function onMeasureRemove(measureID){
                    for(var i in measureContainer.children){
                        if(measureContainer.children[i].measureID===measureID){
                            measureTreeViewModel.remove(measureID);
                            measureContainer.children[i].destroy();
                            sSignal.channelRefresh(-1);
                            break;
                        }
                    }
                }

                function onMeasureRemoveAll(){
                    for(var i in measureContainer.children){
                        if(typeof(measureContainer.children[i].measureID)!=="undefined"){
                            measureTreeViewModel.remove(measureContainer.children[i].measureID);
                            measureContainer.children[i].destroy();
                        }
                    }
                    sSignal.channelRefresh(-1);
                }

                function onSetChannelCursor(cursor){
                    cursorShapeMouseArea.z=cursor===Qt.ArrowCursor?100:2000
                    cursorShapeMouseArea.cursorShape=cursor;
                }

                function onChannelRefresh(channelID)
                {
                    if(channelID<0&&!sConfig.isExit){
                        drawBackground.update();
                        if(channelID===-1)
                            sessionComponentItem.refreshScrollBar();
                    }
                }

                function onSetLiveFollowing(isEnable,visible_){
                    sConfig.isLiveFollowingPopupShow=visible_
                    liveFollowingPopup.visible=visible_;
                    liveFollowingPopup.setLiveFollowing(isEnable);
                }

                function onOpenTestFileSelectPopup(){
                    newDemoButton.onPressed();
                }

                function onShowMouseZoom(show, x, y, width, height){
                    drawBackground.setMouseZoom(show, x,y , width, height);
                }

                function onSendChannelY(channelID, type, y){
                    if(type===2)
                        drawBackground.setMouseYOffset(y);
                    else if(typeof(channelID)==="string" && "DrawBackgroundFloorID"===channelID && type===3)
                        eventDrawBackground.adsorbChannelID=y;
                }

                function onVernierCreate(state){
                    if(state===1)
                        eventDrawBackground.vernierCreateModel=true;
                    else if (state===2)
                        eventDrawBackground.vernierCreateModel=false;
                }

                function onShowSetTips(type, visible_){
                    if(visible_)
                        setTipPopup.open();
                    else
                        setTipPopup.close();
                    switch(type){
                    case 1:
                        setTipPopup.topMargin_=150;
                        setTipImage.source="qrc:resource/icon/"+Config.tp+"/AcquisitionParameterTip_"+Config.language+".png"
                        break;
                    case 2:
                        setTipPopup.topMargin_=398;
                        setTipImage.source="qrc:resource/icon/"+Config.tp+"/IngestOptionsTip_"+Config.language+".png"
                        break;
                    }
                }
            }

            Connections{
                target: Signal
                function onHotKey(type)
                {
                    if(Config.tabSelectButton===sessionID_ && !sConfig.isErrorShow){
                        switch(type){
                        case Config.ShortcutKey.JumpZero:
                            refreshZoom(4);
                            break;
                        case Config.ShortcutKey.ZoomIn:
                            refreshZoom(2);
                            break;
                        case Config.ShortcutKey.ZoomOut:
                            refreshZoom(3);
                            break;
                        case Config.ShortcutKey.ZoomFull:
                            refreshZoom(1);
                            break;
                        case Config.ShortcutKey.VernierCreate:
                            if(!sConfig.isRun && !sConfig.isChannelMethodRun && !sConfig.isErrorShow)
                                sSignal.vernierCreate(0);
                            break;
                        }
                    }
                }
            }

            ChannelHeader{
                id: channelHeader
                height: Config.headerHeight
                width: parent.width
                disable: !channelViewLoader.visible
                z:2
            }

            Rectangle{
                anchors.fill: parent
                color: Config.background2Color
                visible: !channelViewLoader.visible
                Image {
                    id: homeDropShadowImage
                    width: 436
                    height: 322
                    source: "../../resource/image/HomeDropShadow.png"
                    anchors{
                        centerIn: parent
                        verticalCenterOffset: -44
                    }
                    Rectangle{
                        id: homeBackgroundRectangle
                        width: 420
                        height: 306
                        color: Config.backgroundColor
                        radius: 8
                        anchors{
                            top: parent.top
                            topMargin: 6
                            horizontalCenter: parent.horizontalCenter
                        }
                        Image {
                            id: homeBackgroundImage
                            width: 293
                            height: 115
                            source: "../../resource/image/HomeBackground_"+Config.tp+".png"
                            anchors{
                                left: parent.left
                                leftMargin: 71
                                top: parent.top
                                topMargin: 70
                            }
                        }
                        Text {
                            text: qsTr("请连接设备")
                            anchors{
                                horizontalCenter: parent.horizontalCenter
                                top: homeBackgroundImage.bottom
                                topMargin: 62
                            }
                            font.pixelSize: 18
                            color: Config.textColor
                        }
                    }
                    Image {
                        id: orImage
                        anchors{
                            top: homeBackgroundRectangle.bottom
                            topMargin: 26
                            horizontalCenter: parent.horizontalCenter
                        }
                        source: "../../resource/icon/DemoOr.png"
                    }

                    Row{
                        width: 180
                        height: 65
                        spacing: 60
                        anchors{
                            top: homeBackgroundRectangle.bottom
                            topMargin: 66
                            horizontalCenter: parent.horizontalCenter
                        }
                        ImageButton{
                            id: newDemoButton
                            height: 65
                            width: 65
                            showText: "Demo"
                            underline: true
                            textColor: Config.subheadColor
                            fontSize: 14
                            padding: 3
                            imageSource: "resource/icon/Demo.png"
                            imageEnterSource: "resource/icon/Demo.png"
                            backgroundColor: Config.background2Color
                            backgroundMouseEnterColor: Config.mouseEnterColor
                            backgroundPressedColor: backgroundMouseEnterColor
                            onPressed: testFileSelectPopup.visible=true
                        }
                        ImageButton{
                            height: 65
                            width: 65
                            showText: qsTr("打开文件")
                            underline: true
                            textColor: Config.subheadColor
                            fontSize: 14
                            padding: 3
                            imageSource: "resource/icon/DemoOpenFile.png"
                            imageEnterSource: "resource/icon/DemoOpenFile.png"
                            backgroundColor: Config.background2Color
                            backgroundMouseEnterColor: Config.mouseEnterColor
                            backgroundPressedColor: backgroundMouseEnterColor
                            onPressed: Signal.openFile();
                        }
                    }
                }
            }

            TestFileSelectPopup{
                id: testFileSelectPopup
                visible: false
                image.x: homeDropShadowImage.x
                image.y: homeDropShadowImage.y
                z: 1000
            }

            ChannelsView{
                id: channelViewLoader
                width: parent.width
                height: parent.height-channelHeader.height-10
                visible: (sessionType_===Config.SessionType.Demo&&demoShowData)||sessionType_!==Config.SessionType.Demo
                anchors.top: channelHeader.bottom
                z:1
            }

            Rectangle{
                width: parent.width
                height: horizontalScrollBar.height
                visible: channelViewLoader.visible
                anchors{
                    left: parent.left
                    top: horizontalScrollBar.top
                }
                color: Config.background2Color
            }

            Timer{
                property bool isLeft: false
                id: scrollBarTimer
                interval: 100
                repeat: true
                onTriggered: {
                    var stepSize=0.1;
                    horizontalScrollBar.positionSet=true;
                    if(isLeft)
                        horizontalScrollBar.position=(horizontalScrollBar.position-stepSize<0?0:horizontalScrollBar.position-stepSize);
                    else if(horizontalScrollBar.position<1)
                    {
                        var temp=horizontalScrollBar.position+stepSize;
                        horizontalScrollBar.position=temp>1.0-horizontalScrollBar.size?1.0-horizontalScrollBar.size:temp;
                    }
                }
            }

            ImageButton{
                height: horizontalScrollBar.height
                width: height
                visible: channelViewLoader.visible
                enabled: horizontalScrollBar.position!==0
                imageSource: "resource/icon/"+Config.tp+"/ScrollBarButton.png"
                imageEnterSource: "resource/icon/"+Config.tp+"/ScrollBarButtonEnter.png"
                imageDisableSource: "resource/icon/"+Config.tp+"/ScrollBarButtonDisable.png"
                z: 5
                backgroundColor: Config.lineColor
                backgroundMouseEnterColor: "#909090"
                backgroundPressedColor: "#909090"
                cursorShape: Qt.ArrowCursor
                anchors{
                    top: horizontalScrollBar.top
                    right: horizontalScrollBar.left
                }
                onPressed: {
                    scrollBarTimer.isLeft=true;
                    scrollBarTimer.start();
                    scrollBarTimer.triggered();
                }
                onReleased: scrollBarTimer.stop();
                onEnabledChanged: {
                    if(!enabled)
                        scrollBarTimer.stop();
                }
            }

            QScrollBar{
                property bool positionSet: false
                id: horizontalScrollBar
                orientation: Qt.Horizontal
                height: 10
                width: drawBackground.width-20
                visible: channelViewLoader.visible
                padding: 0
                z: 5
                anchors{
                    top: channelViewLoader.bottom
                    right: rightScrollBarButton.left
                }
                position: 0
                onPositionChanged: {
                    if(positionSet)
                        controller.setScrollBarPosition(position);
                }
                onPressedChanged: positionSet=true;
                onWidthChanged: sessionComponentItem.refreshScrollBar();
            }

            ImageButton{
                id: rightScrollBarButton
                height: horizontalScrollBar.height
                width: height
                visible: channelViewLoader.visible
                enabled: (horizontalScrollBar.position+horizontalScrollBar.size)!==1
                imageSource: "resource/icon/"+Config.tp+"/ScrollBarButton.png"
                imageEnterSource: "resource/icon/"+Config.tp+"/ScrollBarButtonEnter.png"
                imageDisableSource: "resource/icon/"+Config.tp+"/ScrollBarButtonDisable.png"
                z: 5
                backgroundColor: Config.lineColor
                backgroundMouseEnterColor: "#909090"
                backgroundPressedColor: "#909090"
                rotation: 180
                cursorShape: Qt.ArrowCursor
                anchors{
                    top: horizontalScrollBar.top
                    right: drawBackground.right
                }
                onPressed: {
                    scrollBarTimer.isLeft=false;
                    scrollBarTimer.start();
                    scrollBarTimer.triggered();
                }
                onReleased: scrollBarTimer.stop();
                onEnabledChanged: {
                    if(!enabled)
                        scrollBarTimer.stop();
                }
            }

            DrawBackground{
                property int leftMargin: (sConfig.isExpandSidebar?Config.headerWidth:Config.closeHeaderWidth)
                id: drawBackground
                width: parent.width-leftMargin-15
                height: channelViewLoader.height
                visible: channelViewLoader.visible
                isExit: sConfig.isExit
                z:100
                anchors{
                    top: channelHeader.bottom
                    left: parent.left
                    leftMargin: leftMargin+5
                }
                Component.onCompleted: {
                    drawBackground.init(sessionID_)
                }
            }

            DrawBackgroundFloor{
                id: eventDrawBackground
                width: drawBackground.width
                height: drawBackground.height
                visible: drawBackground.visible
                isExit: sConfig.isExit
                z: 0
                anchors{
                    top: drawBackground.top
                    right: drawBackground.right
                }

                onHoverLeave: {
                    showCursor=false;
                    sSignal.setChannelCursor(Qt.ArrowCursor);
                }

                onShowCursorChanged: {
                    if(showCursor)
                        sSignal.setChannelCursor(Qt.SizeHorCursor);
                    else if(!sConfig.isMeasureStart)
                        sSignal.setChannelCursor(Qt.ArrowCursor);
                }

                onCloseVernierPopup: sSignal.closeVernierPopup();

                onVernierDataChanged: {
                    sSignal.vernierDataChanged(vernierID);
                    vernierListModel.refresh(vernierID);
                }

                onMeasureSelectChanged: {
                    sSignal.measureRefreshShow();
                    sSignal.channelRefresh(-1);
                }
                Component.onCompleted: eventDrawBackground.init(sessionID_)

                onMouseZoom: {
                    sSignal.showMouseZoom(show, x, y, width, height);
                    sSignal.sendChannelY(-1, 2, 0);
                }

                onShowViewScope: controller.showViewScope(start,end,true);

                onDeleteVernier: vernierListModel.remove(vernierID);

                onVernierCreateComplete: sSignal.vernierCreate(2);

                onGetAdsorbChannelID: sSignal.getChannelY("DrawBackgroundFloorID", 3, y);

                onVernierMoveState: {
                    if(isMove)
                        sConfig.isVernierStart=true;
                    else{
                        sConfig.isVernierStart=false;
                        sSignal.vernierMoveComplete(id);
                    }
                }
            }

            MouseArea{
                id: sideBarWheelEventMouseArea
                anchors{
                    top: channelHeader.bottom
                    left: parent.left
                    bottom: horizontalScrollBar.top
                    right: eventMouseArea.right
                }
                z:100
                visible: channelViewLoader.visible
                propagateComposedEvents: true
                acceptedButtons: Qt.NoButton
                onWheel: {
                    sSignal.channelWheel(wheel.angleDelta.y>0);
                    wheel.accepted = true
                }
            }
            MouseArea{
                id: eventMouseArea
                width: drawBackground.width
                height: channelViewLoader.height
                visible: channelViewLoader.visible
                onWidthChanged: controller.setWidth(width)
                anchors{
                    top: channelHeader.bottom
                    right: parent.right
                    rightMargin: 10
                }
                z:100
                preventStealing: true
                propagateComposedEvents: true
                acceptedButtons: Qt.AllButtons
                onMouseXChanged: mouse.accepted = false
                onWheel: {
                    if(Config.isControlEnter)
                        sSignal.channelWheel(wheel.angleDelta.y>0);
                    else
                        controller.wheelRoll(wheel.angleDelta.y>0,wheel.x);
                    wheel.accepted = true
                }
            }
            Item{
                id: measureContainer
                clip: true
                anchors.fill: eventMouseArea
                visible: channelViewLoader.visible
                z: 500
                VernierPopup{}
                MouseMeasurePopup{}
            }
            LiveFollowingPopup{
                id: liveFollowingPopup
            }
            MouseArea{
                id: cursorShapeMouseArea
                width: drawBackground.width
                height: channelViewLoader.height
                visible: channelViewLoader.visible
                anchors{
                    top: channelHeader.bottom
                    right: parent.right
                    rightMargin: 10
                }
                z: 100
                preventStealing: true
                propagateComposedEvents: true
                acceptedButtons: Qt.NoButton
            }
            MouseArea{
                id: scrollBarWheelEventMouseArea
                anchors{
                    top: channelHeader.bottom
                    left: eventMouseArea.right
                    bottom: horizontalScrollBar.top
                    right: parent.right
                }
                visible: channelViewLoader.visible
                z:100
                preventStealing: true
                propagateComposedEvents: true
                acceptedButtons: Qt.NoButton
                onMouseXChanged: mouse.accepted = false
                onWheel: {
                    sSignal.channelWheel(wheel.angleDelta.y>0);
                    wheel.accepted = true
                }
            }
            Popup{
                property int topMargin_
                id: setTipPopup
                parent: parent
                closePolicy: Popup.NoAutoClose
                height: setTipImage.implicitHeight+10
                width: setTipImage.implicitWidth+10
                padding: 0
                background: Rectangle{
                    radius: 8
                    color: "transparent"
                    Image {
                        id: setTipImage
                        fillMode: Image.Pad
                        anchors{
                            fill: parent
                            margins: 5
                        }
                    }
                }

                onWidthChanged: refreshWidth();

                onHeightChanged: refreshHeight();

                function refreshWidth(){
                    let tmp=sessionComponentItem.width-width-10
                    if(tmp<0)
                        tmp=0;
                    x=tmp;
                }

                function refreshHeight(){
                    let tmp=topMargin_
                    if(tmp+height>sessionComponentItem.height-5)
                        tmp=sessionComponentItem.height-height-5;
                    if(tmp<0)
                        tmp=0;
                    y=tmp;
                }
            }
            Item{
                id: zoomItem
                width: 23
                height: 95
                visible: channelViewLoader.visible && Setting.viewToolPopup
                z: 105
                anchors{
                    right: parent.right
                    rightMargin: 23
                    bottom: parent.bottom
                    bottomMargin: 16
                }
                Rectangle{
                    anchors.fill: parent
                    color: Config.backgroundColor
                    border{
                        width: 1
                        color: Config.lineColor
                    }
                    radius: 3
                }
                Column{
                    spacing: 0
                    anchors.margins: 2
                    ImageButton{
                        height: 23
                        width: 23
                        imageSource: "resource/icon/SetZero.png"
                        imageEnterSource: "resource/icon/SetZero.png"
                        verticalCenterOffset: 0
                        horizontalCenterOffset: 0
                        fillMode: Image.PreserveAspectFit
                        imageHeight: 15
                        imageWidth: 15
                        onPressed: refreshZoom(4)
                    }
                    Rectangle{
                        height: 1
                        width: parent.width
                        border{
                            width: 1
                            color: Config.lineColor
                        }
                    }
                    ImageButton{
                        height: 23
                        width: 23
                        imageSource: "resource/icon/ZoomIn.png"
                        imageEnterSource: "resource/icon/ZoomIn.png"
                        verticalCenterOffset: 0
                        horizontalCenterOffset: 0
                        fillMode: Image.PreserveAspectFit
                        imageHeight: 15
                        imageWidth: 15
                        onPressed: refreshZoom(2)
                    }
                    Rectangle{
                        height: 1
                        width: parent.width
                        border{
                            width: 1
                            color: Config.lineColor
                        }
                    }
                    ImageButton{
                        height: 23
                        width: 23
                        imageSource: "resource/icon/ZoomOut.png"
                        imageEnterSource: "resource/icon/ZoomOut.png"
                        verticalCenterOffset: 0
                        fillMode: Image.PreserveAspectFit
                        imageHeight: 15
                        imageWidth: 15
                        onPressed: refreshZoom(3)
                    }
                    Rectangle{
                        height: 1
                        width: parent.width
                        border{
                            width: 1
                            color: Config.lineColor
                        }
                    }
                    ImageButton{
                        height: 23
                        width: 23
                        imageSource: "resource/icon/ZoomFull.png"
                        imageEnterSource: "resource/icon/ZoomFull.png"
                        verticalCenterOffset: 0
                        fillMode: Image.PreserveAspectFit
                        imageHeight: 15
                        imageWidth: 15
                        onPressed: refreshZoom(1)
                    }
                }
            }
            Image {
                visible: zoomItem.visible
                anchors{
                    left: zoomItem.left
                    top: zoomItem.top
                }
                source: "../../resource/image/ZoomDropShadow.png"
                z: 100
            }
        }
    }

    DropArea
    {
        anchors.fill: parent
        z: 3000
        enabled: true
        onDropped:
        {
            if(drop.hasUrls)
            {
                let files = drop.urls
                let tmp=[];
                let isError=false;
                for(let j=0;j<files.length;j++){
                    if(files[j].substring(files[j].length-6)===".atkdl")
                        tmp.push(files[j]);
                    else
                        isError=true;
                }
                if(isError){
                    windowError.error_msg=qsTr("包含未识别的文件格式，请重新选择文件。");
                }else{
                    if(tmp.length===1&&isConnect===1){
                        Signal.importProject(tmp[0]);
                    }else{
                        for(let i in tmp)
                            root.openFile(tmp[i]);
                    }
                }
            }
        }
    }

    function time2Unit(time, decimal){
        if(typeof(time)==="undefined")
            return "";
        var unitList=["ms","s","m","h"]
        var index=0;
        var unit=unitList[index];
        if(time>=1000){
            index++;
            unit=unitList[index];
            time/=1000.0;
            while(time>=60 && index+1<unitList.length){
                index++;
                unit=unitList[index];
                time/=60;
            }
        }
        return time.toFixed(decimal).toString().replace(/(\.){0,1}0+$/,'')+" "+unit;
    }

    function time22Unit(time, decimal){
        let isNegative=time<0;
        time=Math.abs(time);
        if(typeof(time)==="undefined")
            return "";
        var unitList=["ns","μs","ms","s"]
        var index=0;
        var unit=unitList[index];
        while(time>=1000){
            index++;
            unit=unitList[index];
            time/=1000.0;
        }
        return (isNegative?"-":"")+time.toFixed(decimal).toString().replace(/(\.){0,1}0+$/,'')+" "+unit;
    }

    function hz2Unit(hz){
        if(typeof(hz)==="undefined")
            return "";
        var unitList=["Hz","KHz","MHz","GHz"]
        var index=0;
        var unit=unitList[index];
        while(hz>=1000){
            index++;
            unit=unitList[index];
            hz/=1000.0;
        }
        return hz.toFixed(6).toString().replace(/(\.){0,1}0+$/,'')+" "+unit;
    }

    function sa2Unit(sa){
        if(typeof(sa)==="undefined")
            return "";
        var unitList=["Sa","KSa","MSa","GSa"]
        var index=0;
        var unit=unitList[index];
        while(sa>=1000 && index+1<unitList.length){
            index++;
            unit=unitList[index];
            sa/=1000.0;
        }
        return sa.toFixed(6).toString().replace(/(\.){0,1}0+$/,'')+" "+unit;
    }

    function refreshZoom(state){
        if(sConfig.isRun && !sConfig.isBuffer && sessionType_===Config.SessionType.Device && sConfig.isLiveFollowingPopupShow)
            sSignal.setLiveFollowing(false,true);
        controller.onRefreshZoom(state);
        controller.sessionDrawUpdate();
    }


    function refreshMenuPopupRunState(){
        if(Config.tabSelectButton===sessionID_&&sessionType_!==Config.SessionType.Demo){
            Signal.menuRunStateChanged(sConfig.isRun,sConfig.loopState!==-1,controller.hasData(),sConfig.filePath);
        }
    }
}
