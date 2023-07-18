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

    property int borderWidth: 4
    property color borderColor: "#01FFFFFF"
    property var loadingWindow

    Component.onCompleted: {
        root.visible=false;
        root.initLanguage();
        Setting.init();
        if(Setting.theme<0 || Setting.theme>1)
        {
            Setting.theme=0;
            Setting.save();
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
        }

        onKeyPress: {
            if(key===Qt.Key_Control)
                Config.isControlEnter=true;
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
        property int saveType: 0
        id: exportDialog
        title: qsTr("请选择导出文件路径")
        folder: Platform.StandardPaths.standardLocations(Platform.StandardPaths.DesktopLocation)[0]
        fileMode: Platform.FileDialog.SaveFile
        nameFilters: ["CSV file (*.csv)"]
        defaultSuffix: "csv"
        onAccepted: {
            if(exportDialog.file.toString().substring(exportDialog.file.toString().lastIndexOf(".")).toLowerCase()!==".csv")
                Signal.showError(qsTr("请选择正确文件。"));
            else
            {
                Signal.saveData(exportDialog.file,false,saveType);
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
                Signal.saveData(saveDialog.file,true,2);
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
        asynchronous: true  
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
                    if(root.windowState===0||!root.isWindows())
                        contentRectangle.anchors.margins=0
                    else if(root.windowState===2)
                        contentRectangle.anchors.margins=8
                }
            }
            Rectangle{
                id: contentRectangle
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
                Rectangle{
                    color: Config.backgroundColor
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
                                else if(state===3)
                                    Signal.showUpdateTag(false,true);
                                else
                                {
                                    if(state!==0)
                                        Signal.showUpdateTag(false,true);
                                    updataPopup.json=json;
                                    updataPopup.url=url;
                                    updataPopup.isUpdate=state===1;
                                    updataPopup.isHardwareUpdate=false;
                                    updataPopup.visible=true;
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

                        UpdataPopup{
                            id: updataPopup
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
                            anchors.fill: updataPopup
                            onVisibleChanged: {
                                if(!visible)
                                    windowError.isClose=true
                            }
                        }

                        AboutUs{
                            id: aboutUs
                            anchors.fill: updataPopup
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
}
