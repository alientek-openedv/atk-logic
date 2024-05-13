import QtQuick 2.15
import QtQuick.Controls 2.5
import "../config"
import "../style"

Rectangle {
    property int showModel: 0 //0=正常，1=Demo/File
    property string filePath: ""
    id: menuRoot
    color: "transparent"
    clip: true

    Connections{
        target: Signal
        function onRefreshMenuTabIcon(sidebarContentType, sessionType){
            setButton.imageSource="resource/icon/Set.png";
            setButton.imageEnterSource="resource/icon/Set.png";
            decodeButton.imageSource="resource/icon/Decode.png";
            decodeButton.imageEnterSource="resource/icon/Decode.png";
//            triggerButton.imageSource="resource/icon/Trigger.png";
//            triggerButton.imageEnterSource="resource/icon/Trigger.png";
            measureButton.imageSource="resource/icon/Measure.png";
            measureButton.imageEnterSource="resource/icon/Measure.png";
            searchButton.imageSource="resource/icon/Search.png";
            searchButton.imageEnterSource="resource/icon/Search.png";
            switch(sidebarContentType){
            case Config.SidebarContentType.Set:
                setButton.imageSource="resource/icon/SetActivate.png";
                setButton.imageEnterSource="resource/icon/SetActivate.png";
                break;
            case Config.SidebarContentType.Decode:
                decodeButton.imageSource="resource/icon/DecodeActivate.png";
                decodeButton.imageEnterSource="resource/icon/DecodeActivate.png";
                break;
//            case Config.SidebarContentType.Trigger:
//                triggerButton.imageSource="resource/icon/TriggerActivate.png";
//                triggerButton.imageEnterSource="resource/icon/TriggerActivate.png";
//                break;
            case Config.SidebarContentType.Measure:
                measureButton.imageSource="resource/icon/MeasureActivate.png";
                measureButton.imageEnterSource="resource/icon/MeasureActivate.png";
                break;
            case Config.SidebarContentType.Search:
                searchButton.imageSource="resource/icon/SearchActivate.png";
                searchButton.imageEnterSource="resource/icon/SearchActivate.png";
                break;
            }
            if(sessionType===Config.SessionType.Demo)
            {
                exportButton.enabled=false;
                saveButton.enabled=false;
                saveAsButton.enabled=false;
                exportBINButton.enabled=false;
                showModel=1;
            }else if(sessionType===Config.SessionType.File)
                showModel=1;
            else
                showModel=0;
        }

        function onMenuRunStateChanged(isRun, isLoop, isData, savePath){
            exportButton.enabled=(!isRun && !isLoop && isData);
            saveButton.enabled=exportButton.enabled;
            saveAsButton.enabled=exportButton.enabled;
            exportBINButton.enabled=exportButton.enabled;
            normalButton.enabled=(!isRun && !isLoop);
            loopButton.enabled=normalButton.enabled;
            instantButton.enabled=normalButton.enabled;
            stopButton.enabled=!normalButton.enabled;
            filePath=savePath;
        }

        function onHotKey(type)
        {
            switch(type){
            case Config.ShortcutKey.Save:
                if(saveButton.enabled)
                    saveButton.onPressed();
                break;
            case Config.ShortcutKey.SaveAs:
                if(saveAsButton.enabled)
                    saveAsButton.onPressed();
                break;
            case Config.ShortcutKey.ExportFile:
                if(exportButton.enabled)
                    exportButton.onPressed();
                break;
            }
        }
    }


    Item {
        id: moveItem
        anchors{
            left: parent.left
            right: parent.right
        }
        z:100
        height: rootRectangle.height+1
        Rectangle{
            id:borderRectangle
            anchors.fill: parent
            color: Config.lineColor
            Rectangle {
                id: rootRectangle
                visible: true
                width: parent.width
                height: 80
                anchors{
                    top: parent.top
                    left: parent.left
                }
                color: Config.pageColor
                MouseArea{
                    anchors.fill: parent
                    preventStealing: true
                    hoverEnabled: true
                }
                Connections{
                    target: Signal
                    function onMenuStateChanged(state)
                    {
                        switch(state){
                        case Config.MenuState.Fixed:
                            Config.isFixed=true;
                            break;
                        case Config.MenuState.NoFixed:
                            Config.isFixed=false;
                            break;
                        }
                    }

                    function onOpenFile(){
                        openButton.onPressed();
                    }

                    function onShowUpdateTag(isHardwareUpdate, isShow){
                        if(isHardwareUpdate)
                        {
                            hardwareUpdateButtonTag.visible=isShow
                            if(updatePopup.isHardwareUpdate!==0)
                                updatePopup.visible=false;
                        }
                        else
                            updateButtonTag.visible=isShow
                    }

                    function onSaveDataEvent(){
                        if(saveButton.enabled)
                            saveButton.onPressed();
                    }
                }
                ImageButton{
                    id: topButton
                    width: 20
                    height: 20
                    anchors{
                        right: parent.right
                        bottom: parent.bottom
                        margins: 5
                    }
                    imageWidth: Config.isFixed?10:11
                    imageHeight: Config.isFixed?7:11
                    imageSource: Config.isFixed?"resource/icon/TopShrink.png":"resource/icon/light/TopEnter.png"
                    imageEnterSource: imageSource
                    backgroundColor: Config.pageColor
                    backgroundPressedColor: Config.mouseCheckColor
                    backgroundMouseEnterColor: Config.mouseEnterColor
                    onPressed: {
                        if(Config.isFixed)
                        {
                            Signal.menuStateChanged(Config.MenuState.NoFixed);
                            Signal.menuStateChanged(Config.MenuState.NoDisplay);
                        }else
                        {
                            Signal.menuStateChanged(Config.MenuState.Fixed);
                            Signal.menuStateChanged(Config.MenuState.Display);
                        }
                    }
                }


                Row{
                    id: fileItem
                    visible: Config.menuSelectButton===Config.MenuItemType.File
                    spacing: 20
                    anchors{
                        fill: parent
                        leftMargin: 20
                        rightMargin: 20
                    }
                    Item{
                        width: openRow.width
                        height: parent.height
                        Row{
                            id: openRow
                            spacing: 20
                            anchors{
                                top: parent.top
                                topMargin: 10
                            }
                            ImageButton{
                                id: openButton
                                implicitWidth: 30
                                height: 46
                                showText: qsTr("打开")
                                padding: 3
                                imageWidth: 30
                                imageHeight: 30
                                imageSource: "resource/icon/Open.png"
                                imageEnterSource: imageSource
                                backgroundColor: Config.pageColor
                                backgroundMouseEnterColor: Config.mouseEnterColor
                                backgroundPressedColor: Config.mouseEnterColor
                                onPressed: {
                                    fileDialog.isImport=false;
                                    fileDialog.open();
                                }
                            }
                            ImageButton{
                                id: importProjectButton
                                implicitWidth: 30
                                height: 46
                                showText: qsTr("导入工程")
                                padding: 3
                                enabled: Config.isHardwarePageConnect&&normalButton.enabled
                                imageWidth: 30
                                imageHeight: 30
                                imageSource: "resource/icon/ImportProject.png"
                                imageEnterSource: imageSource
                                imageDisableSource: "resource/icon/"+Config.tp+"/ImportProjectDisable.png"
                                backgroundColor: Config.pageColor
                                backgroundMouseEnterColor: Config.mouseEnterColor
                                backgroundPressedColor: Config.mouseEnterColor
                                onPressed: {
                                    fileDialog.isImport=true;
                                    fileDialog.open();
                                }
                            }
                            ImageButton{
                                id: saveButton
                                implicitWidth: 30
                                height: 46
                                showText: qsTr("保存")
                                padding: 3
                                imageWidth: 30
                                imageHeight: 30
                                imageSource: "resource/icon/Save.png"
                                imageEnterSource: imageSource
                                imageDisableSource: "resource/icon/"+Config.tp+"/SaveDisable.png"
                                backgroundColor: Config.pageColor
                                backgroundMouseEnterColor: Config.mouseEnterColor
                                backgroundPressedColor: Config.mouseEnterColor
                                onPressed: {
                                    if(filePath==="")
                                        saveDialog.open();
                                    else
                                        Signal.saveData(filePath, false, Config.SaveType.ATKDL, "");
                                }
                            }
                            ImageButton{
                                id: saveAsButton
                                implicitWidth: 30
                                height: 46
                                showText: qsTr("另存为")
                                padding: 3
                                imageWidth: 30
                                imageHeight: 30
                                imageSource: "resource/icon/SaveAs.png"
                                imageEnterSource: imageSource
                                imageDisableSource: "resource/icon/"+Config.tp+"/SaveAsDisable.png"
                                backgroundColor: Config.pageColor
                                backgroundMouseEnterColor: Config.mouseEnterColor
                                backgroundPressedColor: Config.mouseEnterColor
                                onPressed: saveDialog.open();
                            }
                        }
                        Text{
                            text: qsTr("工程")
                            font.pixelSize: 11
                            color: Config.subheadColor
                            anchors{
                                bottom: parent.bottom
                                bottomMargin: 3
                                horizontalCenter: parent.horizontalCenter
                            }
                        }
                    }
                    Rectangle{
                        width: 1
                        height: 66
                        anchors.verticalCenter: parent.verticalCenter
                        color: Config.lineColor
                        clip: true
                    }
                    Item{
                        width: exportRow.width
                        height: parent.height
                        Row{
                            id: exportRow
                            spacing: 20
                            anchors{
                                top: parent.top
                                topMargin: 10
                            }
                            ImageButton{
                                id: exportButton
                                implicitWidth: 30
                                height: 46
                                showText: qsTr("导出")
                                padding: 3
                                imageWidth: 30
                                imageHeight: 30
                                imageSource: "resource/icon/Export.png"
                                imageEnterSource: imageSource
                                imageDisableSource: "resource/icon/"+Config.tp+"/ExportDisable.png"
                                backgroundColor: Config.pageColor
                                backgroundMouseEnterColor: Config.mouseEnterColor
                                backgroundPressedColor: Config.mouseEnterColor
                                onPressed: {
                                    exportDialog.saveType=Config.SaveType.DataCSV;
                                    exportDialog.open();
                                }
                            }
                            ImageButton{
                                id: exportBINButton
                                implicitWidth: 30
                                height: 46
                                showText: qsTr("导出原始数据")
                                padding: 3
                                imageWidth: 30
                                imageHeight: 30
                                imageSource: "resource/icon/ExportBIN.png"
                                imageEnterSource: imageSource
                                imageDisableSource: "resource/icon/"+Config.tp+"/ExportBINDisable.png"
                                backgroundColor: Config.pageColor
                                backgroundMouseEnterColor: Config.mouseEnterColor
                                backgroundPressedColor: Config.mouseEnterColor
                                onPressed: {
                                    Signal.showExportBinPopup();
                                    if(!Config.isFixed)
                                        Signal.menuStateChanged(Config.MenuState.NoDisplay);
                                }
                            }
                        }
                        Text{
                            text: qsTr("数据")
                            font.pixelSize: 11
                            color: Config.subheadColor
                            anchors{
                                bottom: parent.bottom
                                bottomMargin: 3
                                horizontalCenter: parent.horizontalCenter
                            }
                        }
                    }
                }


                Row{
                    id: taskItem
                    visible: Config.menuSelectButton===Config.MenuItemType.Property
                    spacing: 20
                    anchors{
                        fill: parent
                        leftMargin: 20
                        rightMargin: 20
                    }
                    Item{
                        width: runRow.width
                        height: parent.height
                        visible: showModel===0
                        Row{
                            id: runRow
                            spacing: 20
                            anchors{
                                top: parent.top
                                topMargin: 10
                            }
                            ImageButton{
                                id: normalButton
                                implicitWidth: 30
                                height: 46
                                showText: qsTr("单次采集")
                                padding: 3
                                imageWidth: 20
                                imageHeight: 20
                                imageSource: "resource/icon/Start_small.png"
                                imageEnterSource: imageSource
                                imageDisableSource: "resource/icon/"+Config.tp+"/Start_small_Disable.png"
                                backgroundColor: Config.pageColor
                                backgroundMouseEnterColor: Config.mouseEnterColor
                                backgroundPressedColor: Config.mouseEnterColor
                                onPressed: start(0);
                            }
                            ImageButton{
                                id: loopButton
                                implicitWidth: 30
                                height: 46
                                showText: qsTr("循环采集")
                                padding: 3
                                imageWidth: 20
                                imageHeight: 20
                                imageSource: "resource/icon/LoopStart_small.png"
                                imageEnterSource: imageSource
                                imageDisableSource: "resource/icon/"+Config.tp+"/LoopStart_small_Disable.png"
                                backgroundColor: Config.pageColor
                                backgroundMouseEnterColor: Config.mouseEnterColor
                                backgroundPressedColor: Config.mouseEnterColor
                                onPressed: start(1);
                            }
                            ImageButton{
                                id: instantButton
                                implicitWidth: 30
                                height: 46
                                showText: qsTr("立即采集")
                                padding: 3
                                imageWidth: 20
                                imageHeight: 20
                                imageSource: "resource/icon/FastStart_small.png"
                                imageEnterSource: imageSource
                                imageDisableSource: "resource/icon/"+Config.tp+"/FastStart_small_Disable.png"
                                backgroundColor: Config.pageColor
                                backgroundMouseEnterColor: Config.mouseEnterColor
                                backgroundPressedColor: Config.mouseEnterColor
                                onPressed: start(2);
                            }
                            ImageButton{
                                id: stopButton
                                implicitWidth: 30
                                height: 46
                                showText: qsTr("停止")
                                padding: 3
                                imageWidth: 20
                                imageHeight: 20
                                imageSource: "resource/icon/Stop_small.png"
                                imageEnterSource: imageSource
                                imageDisableSource: "resource/icon/"+Config.tp+"/Stop_small_Disable.png"
                                backgroundColor: Config.pageColor
                                backgroundMouseEnterColor: Config.mouseEnterColor
                                backgroundPressedColor: Config.mouseEnterColor
                                onClicked: stop();
                            }
                        }
                        Text{
                            text: qsTr("运行")
                            font.pixelSize: 11
                            color: Config.subheadColor
                            anchors{
                                bottom: parent.bottom
                                bottomMargin: 3
                                horizontalCenter: parent.horizontalCenter
                            }
                        }
                    }
                    Rectangle{
                        width: 1
                        height: 66
                        visible: showModel===0
                        anchors.verticalCenter: parent.verticalCenter
                        color: Config.lineColor
                        clip: true
                    }
                    Item{
                        width: setRow.width
                        height: parent.height
                        Row{
                            id: setRow
                            spacing: 20
                            anchors{
                                top: parent.top
                                topMargin: 10
                            }
                            ImageButton{
                                id: setButton
                                implicitWidth: 30
                                height: 46
                                showText: qsTr("更多选项")
                                padding: 3
                                imageHeight: 22
                                imageWidth: 22
                                fillMode: Image.Stretch
                                imageSource: "resource/icon/Set.png"
                                imageEnterSource: imageSource
                                backgroundColor: Config.pageColor
                                backgroundMouseEnterColor: Config.mouseEnterColor
                                backgroundPressedColor: Config.mouseEnterColor
                                onPressed: setSidebarContentType(Config.SidebarContentType.Set)
                            }
                        }
                        Text{
                            text: qsTr("采集选项")
                            font.pixelSize: 11
                            color: Config.subheadColor
                            anchors{
                                bottom: parent.bottom
                                bottomMargin: 3
                                horizontalCenter: parent.horizontalCenter
                            }
                        }
                    }
                    Rectangle{
                        width: 1
                        height: 66
                        anchors.verticalCenter: parent.verticalCenter
                        color: Config.lineColor
                        clip: true
                    }
                    Item{
                        width: viewRow.width
                        height: parent.height
                        Column{
                            id: viewRow
                            spacing: 3
                            anchors{
                                top: parent.top
                                topMargin: 10
                            }
                            Rectangle{
                                width: childrenRect.width
                                height: childrenRect.height
                                border{
                                    width: 1
                                    color: Config.lineColor
                                }
                                color: "transparent"
                                radius: 4
                                Row{
                                    spacing: 0
                                    ImageButton{
                                        height: 22
                                        width: 27
                                        imageSource: "resource/icon/ZoomIn.png"
                                        imageEnterSource: imageSource
                                        verticalCenterOffset: 1
                                        horizontalCenterOffset: 1
                                        imageHeight: 15
                                        imageWidth: 15
                                        onPressed: refreshZoom(2)
                                    }
                                    Rectangle{
                                        height: parent.height
                                        width: 1
                                        border{
                                            width: 1
                                            color: Config.lineColor
                                        }
                                        clip: true
                                    }
                                    ImageButton{
                                        height: 22
                                        width: 27
                                        imageSource: "resource/icon/ZoomOut.png"
                                        imageEnterSource: imageSource
                                        verticalCenterOffset: 1
                                        imageHeight: 15
                                        imageWidth: 15
                                        onPressed: refreshZoom(3)
                                    }
                                    Rectangle{
                                        height: parent.height
                                        width: 1
                                        border{
                                            width: 1
                                            color: Config.lineColor
                                        }
                                        clip: true
                                    }
                                    ImageButton{
                                        height: 22
                                        width: 26
                                        imageSource: "resource/icon/ZoomFull.png"
                                        imageEnterSource: imageSource
                                        verticalCenterOffset: 1
                                        imageHeight: 15
                                        imageWidth: 15
                                        onPressed: refreshZoom(1)
                                    }
                                    Rectangle{
                                        height: parent.height
                                        width: 1
                                        border{
                                            width: 1
                                            color: Config.lineColor
                                        }
                                        clip: true
                                    }
                                    ImageButton{
                                        height: 22
                                        width: 27
                                        imageHeight: 15
                                        imageWidth: 15
                                        imageSource: "resource/icon/SetZero.png"
                                        imageEnterSource: imageSource
                                        verticalCenterOffset: 1
                                        onPressed: refreshZoom(4)
                                    }
                                }
                            }
                            QTextInput{
                                id: zoomTextInput
                                width: parent.width
                                height: 22
                                showText: "28.968714 ms"
                                fontSize: 10
                                editable: false
                                backgroundColor: Config.backgroundColor
                                backgroundBorderColor: Config.lineColor
                            }
                        }
                        Text{
                            text: qsTr("视图")
                            font.pixelSize: 11
                            color: Config.subheadColor
                            anchors{
                                bottom: parent.bottom
                                bottomMargin: 3
                                horizontalCenter: parent.horizontalCenter
                            }
                        }
                    }
                    Rectangle{
                        width: 1
                        height: 66
                        visible: showModel===0
                        anchors.verticalCenter: parent.verticalCenter
                        color: Config.lineColor
                        clip: true
                    }
                    Item{
                        width: hardwareRow.width
                        height: parent.height
                        visible: showModel===0
                        Row{
                            id: hardwareRow
                            spacing: 20
                            anchors{
                                top: parent.top
                                topMargin: 10
                            }
                            ImageButton{
                                implicitWidth: 30
                                height: 46
                                showText: qsTr("固件更新")
                                padding: 3
                                enabled: Config.isHardwareUpdate
                                imageWidth: 30
                                imageHeight: 30
                                imageSource: "resource/icon/Update.png"
                                imageEnterSource: imageSource
                                imageDisableSource: "resource/icon/"+Config.tp+"/UpdateDisable.png"
                                backgroundColor: Config.pageColor
                                backgroundMouseEnterColor: Config.mouseEnterColor
                                backgroundPressedColor: Config.mouseEnterColor
                                onPressed: {
                                    switch(Config.updateJson["state"]){
                                    case 2://获取错误
                                        windowError.error_msg=qsTr("连接服务器失败，请检查网络，并重连设备");
                                        break;
                                    case 3:
                                        windowError.error_msg=qsTr("请连接设备。");
                                        break;
                                    default:
                                        updatePopup.json=Config.updateJson["data"];
                                        updatePopup.sText=qsTr("内部版本")+" "+root.getVersionStr();
                                        updatePopup.url=Config.updateJson["url"];
                                        updatePopup.isUpdate=(Config.updateJson["state"]===1||Config.updateJson["state"]===4);
                                        updatePopup.isHardwareUpdate=1;
                                        updatePopup.showNotRemindButton=false;
                                        if(Config.updateJson["state"]===4)//设备异常
                                            updatePopup.isHardwareUpdate=2;
                                        updatePopup.visible=true;
                                    }
                                }
                                Rectangle{
                                    id: hardwareUpdateButtonTag
                                    width: 6
                                    height: width
                                    radius: width
                                    color: Config.red
                                    visible: false
                                    anchors{
                                        top: parent.top
                                        topMargin: 2
                                        horizontalCenter: parent.horizontalCenter
                                        horizontalCenterOffset: 14
                                    }
                                }
                            }
                        }
                        Text{
                            text: qsTr("硬件")
                            font.pixelSize: 11
                            color: Config.subheadColor
                            anchors{
                                bottom: parent.bottom
                                bottomMargin: 3
                                horizontalCenter: parent.horizontalCenter
                            }
                        }
                    }
                }

                Row{
                    id: showItem
                    visible: Config.menuSelectButton===Config.MenuItemType.Show
                    spacing: 20
                    anchors{
                        fill: parent
                        leftMargin: 20
                        rightMargin: 20
                    }
                    Item{
                        width: tabRow.width
                        height: parent.height
                        Row{
                            id: tabRow
                            spacing: 20
                            anchors{
                                top: parent.top
                                topMargin: 10
                            }
                            ImageButton{
                                id: decodeButton
                                implicitWidth: 30
                                height: 46
                                showText: qsTr("解码")
                                padding: 3
                                imageWidth: 30
                                imageHeight: 30
                                imageSource: "resource/icon/Decode.png"
                                imageEnterSource: imageSource
                                backgroundColor: Config.pageColor
                                backgroundMouseEnterColor: Config.mouseEnterColor
                                backgroundPressedColor: Config.mouseEnterColor
                                onPressed: setSidebarContentType(Config.SidebarContentType.Decode)
                            }
//                            ImageButton{
//                                id: triggerButton
//                                implicitWidth: 30
//                                height: 46
//                                showText: qsTr("触发")
//                                padding: 3
//                                imageSource: "resource/icon/Trigger.png"
//                                imageEnterSource: imageSource
//                                backgroundColor: Config.pageColor
//                                backgroundMouseEnterColor: Config.mouseEnterColor
//                                backgroundPressedColor: Config.mouseEnterColor
//                                onPressed: setSidebarContentType(Config.SidebarContentType.Trigger)
//                            }
                            ImageButton{
                                id: measureButton
                                implicitWidth: 30
                                height: 46
                                showText: qsTr("测量")
                                padding: 3
                                imageWidth: 30
                                imageHeight: 30
                                imageSource: "resource/icon/Measure.png"
                                imageEnterSource: imageSource
                                backgroundColor: Config.pageColor
                                backgroundMouseEnterColor: Config.mouseEnterColor
                                backgroundPressedColor: Config.mouseEnterColor
                                onPressed: setSidebarContentType(Config.SidebarContentType.Measure)
                            }
                            ImageButton{
                                id: searchButton
                                implicitWidth: 30
                                height: 46
                                showText: qsTr("搜索")
                                padding: 3
                                imageWidth: 30
                                imageHeight: 30
                                imageSource: "resource/icon/Measure.png"
                                imageEnterSource: imageSource
                                backgroundColor: Config.pageColor
                                backgroundMouseEnterColor: Config.mouseEnterColor
                                backgroundPressedColor: Config.mouseEnterColor
                                onPressed: setSidebarContentType(Config.SidebarContentType.Search)
                            }
                        }
                        Text{
                            text: qsTr("选项卡")
                            font.pixelSize: 11
                            color: Config.subheadColor
                            anchors{
                                bottom: parent.bottom
                                bottomMargin: 3
                                horizontalCenter: parent.horizontalCenter
                            }
                        }
                    }
                    Rectangle{
                        width: 1
                        height: 66
                        anchors.verticalCenter: parent.verticalCenter
                        color: Config.lineColor
                        clip: true
                    }
                    Item{
                        width: languageRow.width
                        height: parent.height
                        Row{
                            id: languageRow
                            spacing: 20
                            anchors{
                                top: parent.top
                                topMargin: 10
                            }
                            ImageButton{
                                implicitWidth: 30
                                height: 46
                                showText: "简体中文"
                                padding: 3
                                enabled: Config.language!==Config.languageList[0]
                                imageWidth: 30
                                imageHeight: 30
                                imageSource: "resource/icon/Chinese.png"
                                imageEnterSource: imageSource
                                imageDisableSource: "resource/icon/ChineseActivate.png"
                                backgroundColor: Config.pageColor
                                backgroundMouseEnterColor: Config.mouseEnterColor
                                backgroundPressedColor: Config.mouseEnterColor
                                onPressed: setLanguage(Config.languageList[0])
                            }
                            ImageButton{
                                implicitWidth: 30
                                height: 46
                                showText: "English"
                                padding: 3
                                enabled: Config.language!==Config.languageList[1]
                                imageWidth: 30
                                imageHeight: 30
                                imageSource: "resource/icon/English.png"
                                imageEnterSource: imageSource
                                imageDisableSource: "resource/icon/EnglishActivate.png"
                                backgroundColor: Config.pageColor
                                backgroundMouseEnterColor: Config.mouseEnterColor
                                backgroundPressedColor: Config.mouseEnterColor
                                onPressed: setLanguage(Config.languageList[1])
                            }
                        }
                        Text{
                            text: qsTr("语言")
                            font.pixelSize: 11
                            color: Config.subheadColor
                            anchors{
                                bottom: parent.bottom
                                bottomMargin: 3
                                horizontalCenter: parent.horizontalCenter
                            }
                        }
                    }
                    Rectangle{
                        width: 1
                        height: 66
                        anchors.verticalCenter: parent.verticalCenter
                        color: Config.lineColor
                        clip: true
                    }
                    Item{
                        width: themeRow.width
                        height: parent.height
                        Row{
                            id: themeRow
                            spacing: 20
                            anchors{
                                top: parent.top
                                topMargin: 10
                            }
                            ImageButton{
                                implicitWidth: 30
                                height: 46
                                showText: qsTr("明亮")
                                padding: 3
                                enabled: Setting.theme!=1
                                imageWidth: 30
                                imageHeight: 30
                                imageSource: "resource/icon/"+Config.tp+"/LightModel.png"
                                imageEnterSource: imageSource
                                imageDisableSource: imageSource
                                backgroundColor: Config.pageColor
                                backgroundMouseEnterColor: Config.mouseEnterColor
                                backgroundPressedColor: Config.mouseEnterColor
                                onPressed: {
                                    Setting.theme=1;
                                    Setting.save();
                                }
                            }
                            ImageButton{
                                implicitWidth: 30
                                height: 46
                                showText: qsTr("暗黑")
                                padding: 3
                                enabled: Setting.theme!=0
                                imageWidth: 30
                                imageHeight: 30
                                imageSource: "resource/icon/"+Config.tp+"/DarkModel.png"
                                imageEnterSource: imageSource
                                imageDisableSource: imageSource
                                backgroundColor: Config.pageColor
                                backgroundMouseEnterColor: Config.mouseEnterColor
                                backgroundPressedColor: Config.mouseEnterColor
                                onPressed: {
                                    Setting.theme=0;
                                    Setting.save();
                                }
                            }
                        }
                        Text{
                            text: qsTr("主题")
                            font.pixelSize: 11
                            color: Config.subheadColor
                            anchors{
                                bottom: parent.bottom
                                bottomMargin: 3
                                horizontalCenter: parent.horizontalCenter
                            }
                        }
                    }
                    Rectangle{
                        width: 1
                        height: 66
                        anchors.verticalCenter: parent.verticalCenter
                        color: Config.lineColor
                        clip: true
                    }
                    Item{
                        width: windowSetRow.width
                        height: parent.height
                        Row{
                            id: windowSetRow
                            spacing: 20
                            anchors{
                                top: parent.top
                                topMargin: 10
                            }
                            Connections{
                                id: con
                                ignoreUnknownSignals: true
                                function onDataSend(dataJson)
                                {
                                    //                                    console.log(JSON.stringify(dataJson));
                                }

                                function onClosing(){
                                    Config.isSetModel=false;
                                }
                            }
                            ImageButton{
                                implicitWidth: 35
                                height: 46
                                showText: qsTr("设置")
                                padding: 3
                                imageHeight: 22
                                imageWidth: 22
                                fillMode: Image.Stretch
                                imageSource: "resource/icon/Set.png"
                                imageEnterSource: imageSource
                                backgroundColor: Config.pageColor
                                backgroundMouseEnterColor: Config.mouseEnterColor
                                backgroundPressedColor: Config.mouseEnterColor
                                onClicked: {
                                    var component = Qt.createComponent("qrc:/qml/window/SetWindow.qml");
                                    if (component.status === Component.Ready){
                                        var dataJson={};
                                        var subParams = {
                                            "dataJson":dataJson,
                                            "screen": root.getScreenQRect(),
                                            "root":root
                                        }
                                        var showWindow = component.createObject(root, subParams);
                                        showWindow.show();
                                        con.target=showWindow;
                                    }
                                }
                            }
                            ImageButton{
                                implicitWidth: 30
                                height: 46
                                showText: qsTr("软件更新")
                                padding: 3
                                imageWidth: 30
                                imageHeight: 30
                                imageSource: "resource/icon/Update.png"
                                imageEnterSource: imageSource
                                backgroundColor: Config.pageColor
                                backgroundMouseEnterColor: Config.mouseEnterColor
                                backgroundPressedColor: Config.mouseEnterColor
                                onPressed: root.checkUpdate(parseInt(app_VERSION_NUM), true);
                                Rectangle{
                                    id: updateButtonTag
                                    width: 6
                                    height: width
                                    radius: width
                                    color: Config.red
                                    visible: false
                                    anchors{
                                        top: parent.top
                                        topMargin: 2
                                        horizontalCenter: parent.horizontalCenter
                                        horizontalCenterOffset: 14
                                    }
                                }
                            }
                        }
                        Text{
                            text: qsTr("软件设置")
                            font.pixelSize: 11
                            color: Config.subheadColor
                            anchors{
                                bottom: parent.bottom
                                bottomMargin: 3
                                horizontalCenter: parent.horizontalCenter
                            }
                        }
                    }
                }
            }
        }
        Image {
            id: dropShadowImage
            visible: !Config.isFixed
            anchors{
                left: parent.left
                right: parent.right
                top: borderRectangle.bottom
            }
            height: 8
            source: "../../resource/icon/"+Config.tp+"/DropShadow.png"
        }
    }

    MouseArea{
        anchors.fill: moveItem
        preventStealing: true
        propagateComposedEvents: false
        acceptedButtons: Qt.AllButtons
        hoverEnabled: true
        onWheel: {}
        z: 2
    }

    MouseArea{
        anchors.fill: parent
        propagateComposedEvents: true
        onClicked: mouse.accepted=false
        onPressed: {
            Signal.menuStateChanged(Config.MenuState.NoDisplay);
            mouse.accepted=false
        }
        z: 1
    }


    Connections{
        target: Signal
        function onMenuStateChanged(state)
        {
            switch(state){
            case Config.MenuState.NoDisplay:
                if(visible)
                {
                    clip=true;
                    popupOpen.stop()
                    popupClose.start()
                }
                break;
            case Config.MenuState.Display:
                popupClose.stop()
                if(!visible)
                {
                    visible=true;
                    popupOpen.start()
                }else if(Config.isFixed)
                    clip=false;
                break;
            }
        }

        function onShowZoomString(text){
            zoomTextInput.showText=text;
        }
    }

    YAnimator{
        target: moveItem;
        id: popupClose
        from: 0
        to: -80
        duration: 100
        alwaysRunToEnd: false
        onFinished: visible=false;
    }

    YAnimator {
        target: moveItem;
        id: popupOpen
        from: -80
        to: 0
        duration: 100
        alwaysRunToEnd: false
        onFinished: {
            if(Config.isFixed)
                clip=false;
        }
    }

    function setLanguage(path){
        Config.language=path;
        Signal.setLanguage(path);
    }

    function refreshZoom(state){
        tabView.sessionList[tabView.sessionCurrentIndex].sessionSignal.refreshZoom(state);
    }

    function start(state){
        tabView.sessionList[tabView.sessionCurrentIndex].sessionSignal.start(state);
    }

    function stop(){
        tabView.sessionList[tabView.sessionCurrentIndex].sessionSignal.stop(0);
    }

    function setSidebarContentType(sidebarContentType){
        tabView.sessionList[tabView.sessionCurrentIndex].sessionSignal.setSidebarContentType(sidebarContentType);
    }
}

