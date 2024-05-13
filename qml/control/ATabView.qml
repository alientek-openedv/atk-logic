import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import "../config"
import "../style"
import "../session"

Item {
    property var sessionList:[]
    property var sessionIndexList:[]
    property alias sessionCurrentIndex: stackLayout.currentIndex
    property int tabButtonMaxWidth: 170
    z: 1000
    objectName: "sessionTab"
    clip: true

    StackLayout {
        id: stackLayout
        width: parent.width
        height: parent.height-stateBar.height
        z: 1
    }

    Connections{
        target: root

        function onDeviceToFile(windowID, newPort){
            closeRightMenuPopup.close();
            for(let ii=0;ii<tabButtonModle.count;ii++){
                if(tabButtonModle.get(ii).sessionID===windowID)
                {
                    tabButtonModle.get(ii).type=Config.SessionType.File;
                    tabButtonModle.get(ii).isConnect=2;
                    break;
                }
            }
        }
    }

    Timer{
        property int index
        id: closeTimer
        interval: 100
        repeat: true
        onTriggered: {
            if(!sessionList[sessionIndexList[index]].messageBox.visible){
                if(sessionList[sessionIndexList[index]].messageBox.isNotShow)
                {
                    Setting.closeHint=false;
                    Setting.save();
                }

                if(sessionList[sessionIndexList[index]].messageBox.lastButton===0)
                {
                    let isOpen=false;
                    for(let i=index+1;i<sessionIndexList.length;i++){
                        if(sessionList[sessionIndexList[i]].isConnect===2){
                            if(sessionList[sessionIndexList[i]].sessionController.needSaveData())
                            {
                                isOpen=true;
                                showSaveWindow(sessionList[sessionIndexList[i]]);
                                index=i;
                                break;
                            }
                        }
                    }
                    if(!isOpen)
                        root.windowClose();
                }else if(sessionList[sessionIndexList[index]].messageBox.lastButton===1){
                    Config.isExit=true;
                    saveDialog.open();
                    stop();
                }else if(sessionList[sessionIndexList[index]].messageBox.lastButton===2){
                    root.windowClose();
                }else if(sessionList[sessionIndexList[index]].messageBox.lastButton===3){
                    stop();
                }
            }
        }
    }

    function showSaveWindow(session){
        Signal.tabSelected(session.sessionID_);
        session.messageBox.titleText=qsTr("保存");
        session.messageBox.showText=qsTr("数据尚未保存，是否保存数据？");
        session.messageBox.showCancel=true;
        session.messageBox.type=1;
        session.messageBox.visible=true
    }

    Connections{
        target: Signal
        function onHotKey(type)
        {
            if(tabButtonModle.count>1){
                switch(type){
                case Config.ShortcutKey.SwitchPageUp:
                    if(sessionIndexList[stackLayout.currentIndex]===0)
                        Signal.tabSelected(tabButtonModle.get(tabButtonModle.count-1).sessionID);
                    else
                        Signal.tabSelected(tabButtonModle.get(sessionIndexList[stackLayout.currentIndex]-1).sessionID);
                    break;
                case Config.ShortcutKey.SwitchPageDown:
                    if(sessionIndexList[stackLayout.currentIndex]===tabButtonModle.count-1)
                        Signal.tabSelected(tabButtonModle.get(0).sessionID);
                    else
                        Signal.tabSelected(tabButtonModle.get(sessionIndexList[stackLayout.currentIndex]+1).sessionID);
                    break;
                case Config.ShortcutKey.CloseSession:
                    if(tabButtonModle.get(sessionIndexList[stackLayout.currentIndex]).type===Config.SessionType.File)
                        Signal.sessionCloseClicked(tabButtonModle.get(sessionIndexList[stackLayout.currentIndex]).sessionID,0);
                    break;
                }
            }
        }
        function onTabSelected(sessionID)
        {
            closeRightMenuPopup.close();
            for(var i=0;i<tabButtonModle.count;i++)
            {
                if(tabButtonModle.get(i).sessionID===sessionID){
                    tabButtonModle.get(i).select=true;
                    stackLayout.currentIndex=sessionIndexList[i];
                    Config.tabSelectButton=sessionID;
                    sessionList[stackLayout.currentIndex].sessionSignal.sessionSelect(true);
                }else{
                    tabButtonModle.get(i).select=false;
                    sessionList[sessionIndexList[i]].sessionSignal.sessionSelect(false);
                }
            }
            Signal.setCursor(Qt.ArrowCursor)
            refreshSplit();
        }

        function onSetStateText(text)
        {
            stateText.text=text;
        }

        function onSessionCloseClicked(sessionID,state_){
            for(let i in sessionIndexList){
                if(sessionList[sessionIndexList[i]].sessionID_===sessionID){
                    if(state_===0)
                        sessionList[sessionIndexList[i]].sessionSignal.exit(sessionID);
                    else
                        root.removeSession(sessionList[sessionIndexList[i]].sessionID_);
                    break;
                }
            }
        }

        function onCloseApp(){
            let isOpen=false;
            if(Setting.closeHint)
            {
                for(let i in sessionIndexList){
                    if(sessionList[sessionIndexList[i]].isConnect===2){
                        if(sessionList[sessionIndexList[i]].sessionController.needSaveData())
                        {
                            isOpen=true;
                            showSaveWindow(sessionList[sessionIndexList[i]]);
                            closeTimer.index=i;
                            closeTimer.start();
                            break;
                        }
                    }
                }
            }
            if(!isOpen)
                root.windowClose();
        }

        function onResetConnectDevice(sessionID, port, complete){
            if(complete){
                closeRightMenuPopup.close();
                for(let ii=0;ii<tabButtonModle.count;ii++){
                    if(tabButtonModle.get(ii).sessionID===sessionID)
                    {
                        tabButtonModle.get(ii).type=Config.SessionType.Device;
                        tabButtonModle.get(ii).isConnect=1;
                        break;
                    }
                }
            }
        }

        function onCloseAppSaveNext(){
            Config.isExit=false;
            let isOpen=false;
            for(let i=closeTimer.index+1;i<sessionIndexList.length;i++){
                if(sessionList[sessionIndexList[i]].isConnect===2){
                    if(sessionList[sessionIndexList[i]].sessionController.needSaveData())
                    {
                        isOpen=true;
                        showSaveWindow(sessionList[sessionIndexList[i]]);
                        closeTimer.index=i;
                        closeTimer.start();
                        break;
                    }
                }
            }
            if(!isOpen)
                root.windowClose();
        }

        function onRunScheduleMove(sessionID, isRun){
            for(var i=0;i<tabButtonModle.count;i++)
            {
                if(tabButtonModle.get(i).sessionID===sessionID){
                    if(tabButtonModle.get(i).isRunScheduleMove!==isRun)
                        tabButtonModle.get(i).isRunScheduleMove=isRun;
                    break;
                }
            }
        }
    }

    Rectangle{
        id: splicRectangle
        width: parent.width
        height: 1
        color: Config.pageColor
        z: 5
        anchors{
            top: stackLayout.bottom
            left: parent.left
        }
    }

    Rectangle{
        id: stateBar
        width: parent.width
        height: 34
        color: Config.background2Color
        z: 5
        anchors{
            top: splicRectangle.bottom
            left: parent.left
        }

        Timer{
            property bool isLeft: false
            id: moveTimer
            interval: 100
            repeat: true
            onTriggered: {
                var temp;
                if(isLeft){
                    temp=tabButtonFlickable.contentX-100;
                    if(temp<0)
                        temp=0;
                }else{
                    temp=tabButtonFlickable.contentX+100;
                    if(temp+tabButtonFlickable.width>tabButtonFlickable.contentWidth)
                        temp=tabButtonFlickable.contentWidth-tabButtonFlickable.width;
                }
                if(temp!==tabButtonFlickable.contentX)
                    tabButtonFlickable.contentX=temp;
            }
        }

        Flickable{
            id: tabButtonFlickable
            clip: true
            width: parent.width-tabShowRow.width-arrowsRow.width-40
            height: parent.height
            contentHeight: tabButtonListView.height
            contentWidth: tabButtonListView.contentWidth
            flickableDirection: Flickable.AutoFlickDirection
            boundsBehavior: Flickable.StopAtBounds
            z:100
            anchors{
                top: parent.top
                left: parent.left
                leftMargin: 10
            }
            Behavior on contentX{
                NumberAnimation{
                    duration: 100
                    easing.type: Easing.OutCubic
                }
            }
            ListView{
                id: tabButtonListView
                height: parent.height
                width: contentWidth
                spacing: 0
                orientation: ListView.Horizontal
                delegate: ATabButton{
                    height: tabButtonListView.height-3
                    onRightClick: {
                        closeRightMenuPopup.index_=index;
                        closeRightMenuPopup.refreshState();
                        closeRightMenuPopup.open();
                        closeRightMenuPopup.x=x;
                        closeRightMenuPopup.y=tabButtonListView.y+y-closeRightMenuPopup.height-5
                    }
                }
                model: ListModel{
                    id: tabButtonModle
                }
            }

            Timer{
                property bool isSave: false
                property bool isClose: false
                property bool isCloseAll: false
                property bool isCloseSave: false
                property bool isCloseRight: false
                property bool isCloseOthers: false
                id: menuPopupStateRefreshTimer
                interval: 100
                repeat: true
                running: closeRightMenuPopup.opened
                onTriggered: {
                    isSave=closeRightMenuPopup.isSave;
                    isClose=closeRightMenuPopup.isClose;
                    isCloseAll=closeRightMenuPopup.isCloseAll;
                    isCloseSave=closeRightMenuPopup.isCloseSave;
                    isCloseRight=closeRightMenuPopup.isCloseRight;
                    isCloseOthers=closeRightMenuPopup.isCloseOthers;
                    closeRightMenuPopup.refreshState();
                    let changed=false;
                    if(isSave!==closeRightMenuPopup.isSave)
                        changed=true;
                    if(isClose!==closeRightMenuPopup.isClose)
                        changed=true;
                    if(isCloseAll!==closeRightMenuPopup.isCloseAll)
                        changed=true;
                    if(isCloseSave!==closeRightMenuPopup.isCloseSave)
                        changed=true;
                    if(isCloseRight!==closeRightMenuPopup.isCloseRight)
                        changed=true;
                    if(isCloseOthers!==closeRightMenuPopup.isCloseOthers)
                        changed=true;
                    if(changed)
                        closeRightMenuPopup.refreshShow();
                }
            }

            QMenuPopup{
                property int index_
                property bool isSave: false
                property bool isClose: false
                property bool isCloseAll: false
                property bool isCloseSave: false
                property bool isCloseRight: false
                property bool isCloseOthers: false
                id: closeRightMenuPopup
                parent: tabButtonListView
                showShortcutKey: true
                data: [{"showText":qsTr("保存"),"shortcutKey":Setting.saveFile,"seleteType":isSave?0:-1,"buttonIndex":0},
                    {"showText":qsTr("关闭"),"shortcutKey":Setting.closeSession,"seleteType":isClose?0:-1,"buttonIndex":1},
                    {"showText":qsTr("关闭所有"),"shortcutKey":"","seleteType":isCloseAll?0:-1,"buttonIndex":2},
                    {"showText":qsTr("关闭已保存"),"shortcutKey":"","seleteType":isCloseSave?0:-1,"buttonIndex":3},
                    {"showText":qsTr("关闭右侧所有"),"shortcutKey":"","seleteType":isCloseRight?0:-1,"buttonIndex":4},
                    {"showText":qsTr("除此之外关闭所有"),"shortcutKey":"","seleteType":isCloseOthers?0:-1,"buttonIndex":5},]
                onSelect: {
                    switch(index){
                    case 0:
                        Signal.saveDataEvent();
                        break;
                    case 1:
                        if(tabButtonModle.get(index_).type===Config.SessionType.File)
                            Signal.sessionCloseClicked(tabButtonModle.get(index_).sessionID,0);
                        break;
                    case 2:
                    case 3:
                    case 4:
                    case 5:
                        closeSession(index, index_);
                        break;
                    }
                }

                function refreshState(){
                    let session=sessionList[sessionIndexList[index_]];
                    isSave=session.sessionController.hasData();
                    isClose=session.sessionType_===Config.SessionType.File;
                    isCloseAll=false;
                    isCloseSave=false;
                    isCloseRight=false;
                    isCloseOthers=false;
                    for(let i=0;i<sessionIndexList.length;i++){
                        let temp=sessionList[sessionIndexList[i]];
                        if(temp.sessionType_===Config.SessionType.File){
                            isCloseAll=true;
                            if(i>index_)
                                isCloseRight=true;
                            if(i!==index_)
                                isCloseOthers=true;
                            if(isSave&&!session.sessionController.needSaveData())
                                isCloseSave=true;
                        }
                    }
                }
            }
        }

        Row{
            id: arrowsRow
            spacing: 10
            height: parent.height
            x: (tabButtonFlickable.contentWidth>tabButtonFlickable.width?tabButtonFlickable.width:tabButtonFlickable.contentWidth)+20
            anchors.top: parent.top

            ImageButton{
                width: 8
                height: 14
                enabled: tabButtonFlickable.contentX!==0
                imageSource: "resource/icon/"+Config.tp+"/LeftArrows.png"
                imageEnterSource: imageSource
                imageDisableSource: "resource/icon/"+Config.tp+"/LeftArrowsDisable.png"
                anchors.verticalCenter: parent.verticalCenter
                onPressed: {
                    moveTimer.isLeft=true;
                    moveTimer.start();
                    moveTimer.triggered();
                }
                onReleased: moveTimer.stop();
            }

            ImageButton{
                width: 8
                height: 14
                enabled: tabButtonFlickable.contentX+tabButtonFlickable.width<tabButtonFlickable.contentWidth
                imageSource: "resource/icon/"+Config.tp+"/RightArrows.png"
                imageEnterSource: imageSource
                imageDisableSource: "resource/icon/"+Config.tp+"/RightArrowsDisable.png"
                anchors.verticalCenter: parent.verticalCenter
                onPressed: {
                    moveTimer.isLeft=false;
                    moveTimer.start();
                    moveTimer.triggered();
                }
                onReleased: moveTimer.stop();
            }
        }

        Row{
            id: tabShowRow
            height: parent.height
            anchors{
                top: parent.top
                right: parent.right
                rightMargin: 10
            }
            spacing: 20
            clip: true
            z:100
            Text {
                id: stateText
                text: ""
                anchors.verticalCenter: parent.verticalCenter
                font.pixelSize: 12
                color: Config.subheadColor
            }
        }
    }

    function removeSession(id){
        closeRightMenuPopup.close();
        var recode=0;
        for(let i in sessionIndexList){
            if(sessionList[sessionIndexList[i]].sessionID_===id)
            {
                let del=sessionIndexList[i];
                sessionList[del].destroy();
                //                stackLayout.children = Array.from(stackLayout.children).filter(r => r !== sessionList[del])
                sessionList.splice(del, 1);
                sessionIndexList.splice(i, 1);
                for(var ii in sessionIndexList){
                    if(sessionIndexList[ii]>del)
                        sessionIndexList[ii]--;
                }
                break;
            }
        }
        for(let ii=0;ii<tabButtonModle.count;ii++){
            if(tabButtonModle.get(ii).sessionID===id)
            {
                recode=parseInt(ii);
                tabButtonModle.remove(ii);
                if(recode===tabButtonModle.count)
                    recode=tabButtonModle.count-1;
            }
        }
        Signal.tabSelected(sessionList[sessionIndexList[recode]].sessionID_);
    }

    function createSession(name, usb, channelCount, type, port, level)
    {
        closeRightMenuPopup.close();
        if(level===1)
            name+=" Plus";
        if(type===Config.SessionType.Device){
            let isSet=false;
            for(let i in sessionList){
                if(level===sessionList[i].sessionLevel_&&
                        sessionList[i].sessionType_===Config.SessionType.File&&
                        sessionList[i].isConnect===2&&sessionList[i].sessionName_===name&&
                        sessionList[i].sessionChannelCount_===channelCount){
                    isSet=true;
                    Signal.resetConnectDevice(sessionList[i].sessionID_, port, false);
                    break;
                }
            }
            if(isSet)
                return;
        }
        var item =Qt.createQmlObject('import "../session"; Session{width: parent.width; height: parent.height; sessionType_: '+type+'; sessionUSB: "'+usb+
                                     '"; sessionName_: "'+name+'"; sessionChannelCount_: '+channelCount+(type===1?";isConnect: 1;":";")+
                                     (type===2?'sessionPort_: -1; filePath_: "'+port+'"':'sessionPort_: '+port)+'; sessionLevel_: '+level+';}',stackLayout);
        if(type===1)
        {
            tabButtonModle.insert(1,{"sessionID":item.sessionID_,"buttonText":name,"type":type,"isConnect":type===1?1:0,"showFirst":true,
                                      "showBack":true,"select":false,"isRunScheduleMove":false})
            sessionIndexList.splice(1,0,sessionIndexList.length);
        }
        else
        {
            tabButtonModle.append({"sessionID":item.sessionID_,"buttonText":name,"type":type,"isConnect":type===1?1:0,"showFirst":true,
                                      "showBack":true,"select":false,"isRunScheduleMove":false})
            sessionIndexList.push(sessionIndexList.length);
        }
        sessionList.push(item);
        if(root.firstOpenFile&&type===Config.SessionType.Device)
            root.firstOpenFile=false;
        else
            Signal.tabSelected(item.sessionID_);
        item.sessionName_=name;
        item.sessionChannelCount_=channelCount;
        item.sessionType_=type;
        item.sessionPort_=port;
    }

    function refreshSplit(){
        if(tabButtonModle.count>0){
            for(let i=0;i<tabButtonModle.count;i++){
                tabButtonModle.get(i).showFirst=false;
                tabButtonModle.get(i).showBack=true;
            }
            if(sessionIndexList[stackLayout.currentIndex]!==0)
            {
                tabButtonModle.get(sessionIndexList[stackLayout.currentIndex]-1).showBack=false;
                tabButtonModle.get(0).showFirst=true;
            }
            tabButtonModle.get(sessionIndexList[stackLayout.currentIndex]).showBack=false;
        }
    }

    function closeSession(type, index_){
        let recodeSession=sessionList[sessionIndexList[index_]]
        while(true){
            let isDel=false;
            for(let i=0;i<sessionIndexList.length;i++){
                let temp=sessionList[sessionIndexList[i]];
                if(temp.sessionType_===Config.SessionType.File){
                    switch(type){
                    case 2://关闭所有
                        isDel=true;
                        break;
                    case 3://关闭已保存
                        if(isSave&&!session.sessionController.needSaveData())
                            isDel=true;
                        break;
                    case 4://关闭右侧所有
                        if(i>index_)
                            isDel=true;
                        break;
                    case 5://除此之外关闭所有
                        if(recodeSession!==temp)
                            isDel=true;
                        break;
                    }
                    if(isDel)
                    {
                        Signal.sessionCloseClicked(temp.sessionID_,0);
                        break;
                    }
                }
            }
            if(!isDel)
                break;
        }
    }
}

