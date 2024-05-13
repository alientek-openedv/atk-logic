import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.3
import Qt.labs.platform 1.0 as Platform
import atk.qml.Controls 1.0
import atk.qml.App 1.0
import atk.qml.Model 1.0
import "./control"
import "./config"
import "./style"

FramelessWindow {
    id: root
    visible: true
    minimumHeight: 550
    minimumWidth: 600
    color: "#00000000"
    isLinuxMemoryLimit: Setting.isLinuxMemoryLimit;
    rootDir: tempDir

    property int borderWidth: 4
    property color borderColor: "#01FFFFFF"
    property var loadingWindow
    property bool settingIsInit: false
    property var decodeLogWindow

    Component.onCompleted: {
        root.visible=false;
        initTimer.start();
    }

    Timer{
        id: initTimer
        interval: 10
        onTriggered: {
            while(true){
                if(setRoot)
                    break;
            }
            Setting.init();
            if(Setting.theme<0 || Setting.theme>1)
            {
                Setting.theme=0;
                Setting.save();
            }
            if(typeof(Setting.decodeConfig)==="undefined")
                Setting.decodeConfig={};
            if(Setting.decodeLogLevel<0 || Setting.decodeLogLevel>5)
                Setting.decodeLogLevel=2;
            root.setDecodeLogLevel(Setting.decodeLogLevel);
            if(Setting.width<minimumWidth)
                Setting.width=900;
            if(Setting.height<minimumHeight)
                Setting.width=650;
            if(Setting.isSetWindow){
                root.x=Setting.x;
                root.y=Setting.y;
                root.width=Setting.width;
                root.height=Setting.height;
                var rect=root.getScreenQRect();
                if(root.width>rect.width)
                    root.width=rect.width;
                if(root.height>rect.height)
                    root.height=rect.height;
                if(root.x>rect.x+rect.width-root.width)
                    root.x=rect.x+rect.width-root.width;
                else if(root.x<rect.x)
                    root.x=rect.x;
                if(root.y>rect.y+rect.height-root.height)
                    root.y=rect.y+rect.height-root.height;
                else if(root.y<rect.y)
                    root.y=rect.y;
                if(Setting.windowState!==2)
                    Setting.windowState=0;
                if(Setting.windowState==2)
                    root.showMaximized();
            }
            settingIsInit=true;
            root.initLanguage();
        }
    }

    Timer{
        interval: 5000
        repeat: true
        running: true
        onTriggered: {
            if(settingIsInit){
                Setting.isSetWindow=true;
                if(root.windowState===0){
                    Setting.width=root.width;
                    Setting.height=root.height;
                    Setting.x=root.x;
                    Setting.y=root.y;
                }
                Setting.windowState=root.windowState;
                Setting.save();
            }
            //清除日志记录
            if(!decodeLogWindow || (decodeLogWindow && decodeLogWindow.visibility===0))
                root.getDecodeLogList();

            root.checkUpdateNow();
        }
    }

    onFirstLanguageChanged: {
        Config.language=firstLanguage
        var component = Qt.createComponent("qrc:/qml/window/LoadingWindow.qml");
        if (component.status === Component.Ready){
            loadingWindow = component.createObject(root, {"screen": root.getScreenQRect()});
            loadingWindow.show();
            loadingWindow.visible=true;
            windowLoader.sourceComponent=windowComponent;
        }
    }

    onSendConnectSchedule: {
        if(state_<0)
            connectMessagePopup.isOpen=false;
        else{
            if(!connectMessagePopup.isOpen)
                connectMessagePopup.isOpen=true;
            connectMessagePopup.type=state_;
            connectMessagePopup.schedule=schedule;
        }
    }

    onSendDownloadSchedule: {
        if(type===4)
            connectMessagePopup.type=4;
        else
            connectMessagePopup.type=3;
        connectMessagePopup.schedule=schedule;
    }

    onSendHardwareCheckUpdateDate: {
        if(state===1||state===4)
            Signal.showUpdateTag(true,true);
        else
            Signal.showUpdateTag(true,false);
        Config.updateJson={"data":json,"state":state,"url":url};
        Config.isHardwareUpdate=state!==2&&state!==3;
    }

    FpsItem{
        id: fps
    }

    MessagePopup{
        id: connectMessagePopup
        y: parent.height - height - 60
        visible: false;
    }

    Item{
        Shortcut {
            enabled: true
            context: Qt.ApplicationShortcut
            sequence: "Esc"
            onActivated: Signal.escKeyPress();
        }

        Shortcut {
            enabled: !Config.isSetModel && Setting.startCollecting!==""
            context: Qt.ApplicationShortcut
            sequence: Setting.startCollecting
            onActivated: Signal.hotKey(Config.ShortcutKey.StartCollecting);
        }

        Shortcut {
            enabled: !Config.isSetModel && Setting.stopCollecting!==""
            context: Qt.ApplicationShortcut
            sequence: Setting.stopCollecting
            onActivated: Signal.hotKey(Config.ShortcutKey.StopCollecting);
        }

        Shortcut {
            enabled: !Config.isSetModel && Setting.switchVernierUp!==""
            context: Qt.ApplicationShortcut
            sequence: Setting.switchVernierUp
            onActivated: Signal.hotKey(Config.ShortcutKey.SwitchVernierUp);
        }

        Shortcut {
            enabled: !Config.isSetModel && Setting.switchVernierDown!==""
            context: Qt.ApplicationShortcut
            sequence: Setting.switchVernierDown
            onActivated: Signal.hotKey(Config.ShortcutKey.SwitchVernierDown);
        }

        Shortcut {
            enabled: !Config.isSetModel && Setting.parameterMeasure!==""
            context: Qt.ApplicationShortcut
            sequence: Setting.parameterMeasure
            onActivated: Signal.hotKey(Config.ShortcutKey.ParameterMeasure);
        }

        Shortcut {
            enabled: !Config.isSetModel && Setting.vernierCreate!==""
            context: Qt.ApplicationShortcut
            sequence: Setting.vernierCreate
            onActivated: Signal.hotKey(Config.ShortcutKey.VernierCreate);
        }

        Shortcut {
            enabled: !Config.isSetModel && Setting.switchPageUp!==""
            context: Qt.ApplicationShortcut
            sequence: Setting.switchPageUp
            onActivated: Signal.hotKey(Config.ShortcutKey.SwitchPageUp);
        }

        Shortcut {
            enabled: !Config.isSetModel && Setting.switchPageDown!==""
            context: Qt.ApplicationShortcut
            sequence: Setting.switchPageDown
            onActivated: Signal.hotKey(Config.ShortcutKey.SwitchPageDown);
        }

        Shortcut {
            enabled: !Config.isSetModel && Setting.jumpZero!==""
            context: Qt.ApplicationShortcut
            sequence: Setting.jumpZero
            onActivated: Signal.hotKey(Config.ShortcutKey.JumpZero);
        }

        Shortcut {
            enabled: !Config.isSetModel && Setting.zoomIn!==""
            context: Qt.ApplicationShortcut
            sequence: Setting.zoomIn
            onActivated: Signal.hotKey(Config.ShortcutKey.ZoomIn);
        }

        Shortcut {
            enabled: !Config.isSetModel && Setting.zoomOut!==""
            context: Qt.ApplicationShortcut
            sequence: Setting.zoomOut
            onActivated: Signal.hotKey(Config.ShortcutKey.ZoomOut);
        }

        Shortcut {
            enabled: !Config.isSetModel && Setting.zoomFull!==""
            context: Qt.ApplicationShortcut
            sequence: Setting.zoomFull
            onActivated: Signal.hotKey(Config.ShortcutKey.ZoomFull);
        }

        Shortcut {
            enabled: !Config.isSetModel && Setting.saveFile!==""
            context: Qt.ApplicationShortcut
            sequence: Setting.saveFile
            onActivated: Signal.hotKey(Config.ShortcutKey.Save);
        }

        Shortcut {
            enabled: !Config.isSetModel && Setting.saveAs!==""
            context: Qt.ApplicationShortcut
            sequence: Setting.saveAs
            onActivated: Signal.hotKey(Config.ShortcutKey.SaveAs);
        }

        Shortcut {
            enabled: !Config.isSetModel && Setting.exportFile!==""
            context: Qt.ApplicationShortcut
            sequence: Setting.exportFile
            onActivated: Signal.hotKey(Config.ShortcutKey.ExportFile);
        }

        Shortcut {
            enabled: !Config.isSetModel && Setting.deviceConfig!==""
            context: Qt.ApplicationShortcut
            sequence: Setting.deviceConfig
            onActivated: Signal.hotKey(Config.ShortcutKey.DeviceConfig);
        }

        Shortcut {
            enabled: !Config.isSetModel && Setting.protocolDecode!==""
            context: Qt.ApplicationShortcut
            sequence: Setting.protocolDecode
            onActivated: Signal.hotKey(Config.ShortcutKey.ProtocolDecode);
        }

        Shortcut {
            enabled: !Config.isSetModel && Setting.labelMeasurement!==""
            context: Qt.ApplicationShortcut
            sequence: Setting.labelMeasurement
            onActivated: Signal.hotKey(Config.ShortcutKey.LabelMeasurement);
        }

        Shortcut {
            enabled: !Config.isSetModel && Setting.closeSession!==""
            context: Qt.ApplicationShortcut
            sequence: Setting.closeSession
            onActivated: Signal.hotKey(Config.ShortcutKey.CloseSession);
        }

        Shortcut {
            enabled: !Config.isSetModel && Setting.dataSearch!==""
            context: Qt.ApplicationShortcut
            sequence: Setting.dataSearch
            onActivated: Signal.hotKey(Config.ShortcutKey.DataSearch);
        }
    }

    ShortcutListener{
        id: shortcutListener
        Component.onCompleted: {
            init(root);
            Enable=true;
        }

        onFocusOut: {
            Signal.focusOut();
            Config.isControlEnter=false;
            Config.isAltEnter=false;
        }

        onKeyPress: {
            if(key===Qt.Key_Control)
                Config.isControlEnter=true;
            else if(key===Qt.Key_Delete)
                Signal.hotKey(Config.ShortcutKey.DelVernierMeasure);
//            console.log(["onKeyPress",key,Qt.Key_Delete])
        }

        onKeyRelease: {
            if(key===Qt.Key_Control)
                Config.isControlEnter=false;
        }
    }

    Platform.FileDialog {
        property bool isImport: false
        id: fileDialog
        title: qsTr("请选择打开文件")
        folder: Platform.StandardPaths.standardLocations(Platform.StandardPaths.DesktopLocation)[0]
        fileMode: Platform.FileDialog.OpenFile
        nameFilters: ["Supported files (*.atkdl)"]
        onAccepted: {
            if(!Config.isFixed)
                Signal.menuStateChanged(Config.MenuState.NoDisplay);
            let suffix=fileDialog.file.toString().substring(fileDialog.file.toString().lastIndexOf(".")).toLowerCase();
            switch(suffix){
            case ".atkdl":
                if(!isImport)
                    root.openFile(fileDialog.file);
                else
                    Signal.importProject(fileDialog.file);
                break;
            default:
                Signal.showError(qsTr("请选择受支持的文件。"));
            }
        }
    }

    Platform.FileDialog {
        property int saveType: 0//0=导出数据，1=导出协议表格数据，2=保存atkdl，3=导出原始数据
        id: exportDialog
        title: qsTr("请选择导出文件路径")
        folder: Platform.StandardPaths.standardLocations(Platform.StandardPaths.DesktopLocation)[0]
        fileMode: Platform.FileDialog.SaveFile
        nameFilters: saveType!==Config.SaveType.BinDATA?["CSV file (*.csv)"]:["BIN file (*.bin)"]
        defaultSuffix: saveType!==Config.SaveType.BinDATA?"csv":"bin"
        onAccepted: {
            let suffix=".csv"
            if(saveType===Config.SaveType.BinDATA)
                suffix=".bin"
            if(exportDialog.file.toString().substring(exportDialog.file.toString().lastIndexOf(".")).toLowerCase()!==suffix)
                Signal.showError(qsTr("请选择正确文件。"));
            else
            {
                if(saveType!==Config.SaveType.BinDATA)
                    Signal.saveData(exportDialog.file,false,saveType,"");
                else
                    Signal.sendExportPath(exportDialog.file);
                if(!Config.isFixed)
                    Signal.menuStateChanged(Config.MenuState.NoDisplay);
            }
        }
    }

    Platform.FileDialog {
        id: saveDialog
        title: qsTr("请选择保存文件路径")
        folder: Platform.StandardPaths.standardLocations(Platform.StandardPaths.DesktopLocation)[0]
        fileMode: Platform.FileDialog.SaveFile
        nameFilters: ["session file (*.atkdl)"]
        defaultSuffix: "atkdl"
        onAccepted: {
            if(saveDialog.file.toString().substring(saveDialog.file.toString().lastIndexOf(".")).toLowerCase()!==".atkdl")
                windowError.error_msg=qsTr("请选择正确文件。");
            else
            {
                Signal.saveData(saveDialog.file,true,Config.SaveType.ATKDL,"");
                if(!Config.isFixed)
                    Signal.menuStateChanged(Config.MenuState.NoDisplay);
            }
        }
        onRejected: Config.isExit=false;
    }

    Connections{
        target: Signal
        function onShowInitText(state, text)
        {
            loadingWindow.visible=true;
            if(state===0)
            {
                checkUpdate(parseInt(app_VERSION_NUM), false);
                loadingCloseTimer.start();
                root.initMethod();
            }
        }

        function onSetDecodeConfig(decodeName, attributeName, data){
            let deleteData;
            switch(attributeName){
            case "height":
                deleteData=1;
                break;
            case "isLockRow":
                deleteData=false;
                break;
            }
            if(deleteData===data){
                if(Setting.decodeConfig[decodeName] && Setting.decodeConfig[decodeName][attributeName])
                    delete Setting.decodeConfig[decodeName][attributeName];
                if(JSON.stringify(Setting.decodeConfig[decodeName])==="{}")
                    delete Setting.decodeConfig[decodeName];
            }else{
                if(!Setting.decodeConfig[decodeName])
                    Setting.decodeConfig[decodeName]={};
                Setting.decodeConfig[decodeName][attributeName]=data;
            }
        }
    }

    ParallelAnimation {
        id: rootShowAnim
        NumberAnimation {  target: root; property: "opacity"; from:0; to: 1; duration: 500; easing.type: Easing.OutCubic }
        onStarted: root.visible=true;
        onFinished: root.activateWindow();
    }

    Timer{
        id: loadingCloseTimer
        interval: 1500
        onTriggered: {
            loadingWindow.visible=false
            loadingWindow.close();
            rootShowAnim.start();
        }
    }

    Loader{
        id: windowLoader
        anchors.fill: parent
        asynchronous: true  //异步加载会话
        onLoaded: {
            root.initSet(decode_init_code);
            root.createSession("Home", maxChannelNum, Config.SessionType.Demo, -1);
            Signal.showInitText(1, qsTr("加载协议库"));
        }
    }

    Component{
        id: windowComponent
        Rectangle{
            id: rootRectangle
            anchors.fill: parent
            clip: true
            color: "transparent"

            Connections{
                target: Signal
                function onSetCursor(cursor)
                {
                    if(cursor!==Qt.ArrowCursor)
                    {
                        mainMouseArea.cursorShape=cursor;
                        mainMouseArea.visible=true;
                    }else
                        mainMouseArea.visible=false;
                }

                function onSetLanguage(path){
                    root.setLanguage(path);
                }

                function onShowError(str){
                    windowError.error_msg=str;
                }
            }

            Connections{
                target: root
                function onWindowState_Changed(){
                    contentRectangle.refreshMargins();
                }
            }

            //主体边框
            Rectangle{
                id: contentRectangle
                color: "transparent"
                radius: parent.radius
                clip: true
                Component.onCompleted: refreshMargins();

                function refreshMargins(){
                    if(root.windowState===0||!root.isWindows())
                        contentRectangle.anchors.margins=0
                    else if(root.windowState===2)
                        contentRectangle.anchors.margins=8
                }

                border{
                    id: mainBorder
                    width: 1
                    color: Config.lineColor
                }
                anchors{
                    left: parent.left
                    top: parent.top
                    right: parent.right
                    bottom: parent.bottom
                }
                MouseArea{
                    id: mainMouseArea
                    visible: false
                    anchors.fill: parent
                    preventStealing: true
                    propagateComposedEvents: true
                    acceptedButtons: Qt.NoButton
                    z: 100
                }

                //主体
                Rectangle{
                    color: "transparent"
                    radius: parent.radius
                    clip: true
                    anchors{
                        fill: parent
                        margins: mainBorder.width
                    }

                    WindowError{
                        id: windowError
                        objectName: "windowError"
                        onError_msgChanged: {
                            if(error_msg!="")
                            {
                                console.log("windowError:"+error_msg);
                                errorDialog.titleText=qsTr("错误");
                                errorDialog.showText=error_msg;
                                errorDialog.type=0;
                                errorDialog.visible=true
                            }
                            error_msg="";
                        }
                    }

                    TestFileTableModel{
                        id: testFileTableModel
                    }

                    Item{
                        id: showItem
                        anchors.fill: parent

                        Connections{
                            target: root
                            function onSendCheckUpdateDate(state, url, json){
                                Signal.showUpdateTag(false,false);
                                if(state===2)
                                    windowError.error_msg=qsTr("检查更新失败，请检查网络连接。");
                                else if(state===3){
                                    Signal.showUpdateTag(false,true);
                                    if(json["version"][0]!==Setting.notRemindVersion){
                                        updatePopup.json=json;
                                        updatePopup.url=url;
                                        updatePopup.isUpdate=true;
                                        updatePopup.isHardwareUpdate=false;
                                        updatePopup.version=json["version"][0];
                                        updatePopup.showNotRemindButton=true;
                                        updatePopup.visible=true;
                                    }
                                }
                                else
                                {
                                    if(state!==0)
                                        Signal.showUpdateTag(false,true);
                                    updatePopup.json=json;
                                    updatePopup.url=url;
                                    updatePopup.isUpdate=state===1;
                                    updatePopup.isHardwareUpdate=false;
                                    updatePopup.showNotRemindButton=false;
                                    updatePopup.visible=true;
                                }
                            }

                            function onSendDownloadSchedule(schedule, type, index){
                                if(schedule===100&&type===1)
                                {
                                    if(index===0)
                                        root.sendDownload(index+1);
                                    else if(index===1)
                                        root.startDownloadFirmware();
                                }
                            }
                        }

                        UpdatePopup{
                            id: updatePopup
                            anchors{
                                top: menuBar.bottom
                                left: parent.left
                                right: parent.right
                                bottom: parent.bottom
                            }
                            z: 1000
                            visible: false
                        }

                        QMessageBox{
                            id: errorDialog
                            anchors.fill: updatePopup
                            onVisibleChanged: {
                                if(!visible)
                                    windowError.isClose=true
                            }
                        }

                        AboutUs{
                            id: aboutUs
                            anchors.fill: updatePopup
                        }

                        AMenuBar{
                            id: menuBar
                            width: parent.width
                            height: 35
                            z: 101
                        }

                        AMenuPopup{
                            id: menuPopup
                            anchors{
                                top: menuBar.bottom
                                right: parent.right
                                left: parent.left
                            }
                            height: showItem.height
                            visible: false
                            z: 100
                        }

                        DropArea
                        {
                            anchors.fill: parent
                            enabled: true
                            onDropped: {
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
                                        for(let i in tmp)
                                            root.openFile(tmp[i]);
                                    }
                                }
                            }
                        }

                        ATabView{
                            id:tabView
                            anchors{
                                top: menuBar.bottom
                                left: parent.left
                                right: parent.right
                                bottom: parent.bottom
                            }
                            z: 1
                        }

                        Connections{
                            target: Signal
                            function onMenuStateChanged(state)
                            {
                                switch(state){
                                case Config.MenuState.Fixed:
                                    menuPopup.height=80;
                                    menuPopup.clip=true;
                                    tabView.anchors.top=menuPopup.bottom
                                    break;
                                case Config.MenuState.NoFixed:
                                    menuPopup.height=showItem.height;
                                    tabView.anchors.top=menuBar.bottom
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    /* 获取相对于根窗口的全局的坐标，输入参数为需要获取全局坐标的对象 */
    function getGlobalPosition(targetObject) {
        var positionX = 0
        var positionY = 0
        var obj = targetObject

        /* 遍历所有的父窗口 */
        while (obj !== null) {
            /* 累加计算坐标 */
            positionX += obj.x
            positionY += obj.y

            obj = obj.parent
        }

        return {"x": positionX, "y": positionY}
    }

    function jsonToString(json){
        return JSON.stringify(json);
    }

    function showDecodeLogWindow(){
        if(!decodeLogWindow){
            var component = Qt.createComponent("qrc:/qml/window/DecodeLogWindow.qml");
            if (component.status === Component.Ready){
                var dataJson={};
                var subParams = {
                    "screen": root.getScreenQRect(),
                    "root":root
                }
                decodeLogWindow = component.createObject(root, subParams);
                decodeLogWindow.show();
            }
        }else
            decodeLogWindow.show();
    }
}
