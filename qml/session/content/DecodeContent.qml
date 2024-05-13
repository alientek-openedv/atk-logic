import QtQuick 2.15
import QtQuick.Controls 2.5
import "../../config"
import "../../style"

Rectangle {
    property var shortcutButtonList:[]
    property var decodeShowRowList: []
    property var lastDecode
    color: Config.background2Color
    width: parent.width
    height: parent.height

    Timer{
        id: decodeRefreshTimer
        interval: 200
        onTriggered: {
            if(decodeTableModel.isStop())
                return;
            decodeTableModel.refresh();
            sSignal.channelRefresh(-2);
        }
    }

    Timer{
        id: initDecodeTimer
        interval: 500
        onTriggered: {
            if(!controller.isGlitchRemoval())
                controller.startDecodeTmp();
        }
    }

    Component.onCompleted: initDecodeTimer.start();

    Connections{
        target: controller
        function onSendDecodeSchedule(decodeID,schedule){
            for(var i=0;i<decodeListModel.count;i++){
                var item=decodeListModel.get(i);
                if(item.decodeID===decodeID)
                    item.dataValue=schedule;
            }
            if(sConfig.isStopDecode){
                decodeRefreshTimer.stop();
            }else{
                if(decodeID===sConfig.selectDecodeID)
                    decodeTableModel.setComplete(schedule===100);
                if(!decodeRefreshTimer.running)
                    decodeRefreshTimer.start();
            }
        }

        function onAppendDecode(json) {
            let js=JSON.parse(json);
            if(typeof(js["decodeJson"])!=="undefined")
                js=js["decodeJson"];
            delete js.main.decodeID;
            dataListView.resetScrollBarPosition();
            jsonStart(js);
        }

        function onSendDecodeReset(decodeID){
            if(sConfig.selectDecodeID===decodeID){
                decodeTableModel.resetSort();
                decodeTableModel.setStop(false);
            }
        }
    }

    Connections{
        target: sSignal

        function onCloseAllPopup(){
            decodeMenuPopup.close();
            decodeDataSearchPopup.close();
            decodeSearchPopup.close();
        }

        function onRemoveAllDecode(){
            decodeTableModel.stopAll(true);
            while(decodeListModel.count){
                sSignal.removeDecode(decodeListModel.get(0).decodeID,0);
            }
        }

        function onRemoveDecode(decodeID,state_){
            if(state_===0){
                for(var i=0;i<decodeListModel.count;i++){
                    var item=decodeListModel.get(i);
                    if(item.decodeID===decodeID)
                    {
                        vernierListModel.removeBusy(item.decodeJson["main"]["startID"],decodeID,0);
                        vernierListModel.removeBusy(item.decodeJson["main"]["endID"],decodeID,1);
                        decodeListModel.remove(i);
                        sSettings.decodeJson.splice(i,1)
                        sSettings.save();
                        break;
                    }
                }

                let isSetDecode=false;
                for(let k in decodeShowRowList){
                    if(decodeShowRowList[k]["isSelect"] && decodeID===decodeShowRowList[k]["decodeID"])
                        decodeTableModel.stopAll(true);
                    if(decodeShowRowList[k]["decodeID"]===decodeID)
                    {
                        if(decodeShowRowList[k]["isSelect"])
                            isSetDecode=true;
                        decodeShowRowList.splice(k,1);
                        break;
                    }
                }
                if(decodeShowRowList.length>0){
                    if(isSetDecode)
                        refreshDecodeOnList(decodeShowRowList[0]["decodeID"]);
                }else
                    decodeTableModel.init(sessionID_);
                sSignal.removeDecode(decodeID,1);
            }
        }

        function onResetDecodeJson(decodeJson, decodeID){
            for(var i=0;i<decodeListModel.count;i++){
                var item=decodeListModel.get(i);
                if(item.decodeID===decodeID){
                    item.decodeJson=decodeJson;
                    sSettings.decodeJson[i]["decodeJson"]=decodeJson;
                    sSettings.save();
                    break;
                }
            }
            controller.setDecodeShowJson(decodeID,decodeJson);
        }

        function onResetDecode(decodeID,decodeJson,channelList,isDelModel){
            for(var i=0;i<decodeListModel.count;i++){
                var item=decodeListModel.get(i);
                if(item.decodeID===decodeID){
                    vernierListModel.removeBusy(item.decodeJson["main"]["startID"],decodeID,0);
                    vernierListModel.removeBusy(item.decodeJson["main"]["endID"],decodeID,1);
                    item.decodeJson=decodeJson;
                    let showText=decodeJson["main"]["first"];
                    if(decodeJson["main"]["second"] && decodeJson["main"]["second"].length>0)
                        showText=decodeJson["main"]["second"][decodeJson["main"]["second"].length-1];
                    vernierListModel.appendBusy(decodeJson["main"]["startID"],decodeID,0,showText+" - Start");
                    vernierListModel.appendBusy(decodeJson["main"]["endID"],decodeID,1,showText+" - End");
                    sSettings.decodeJson[i]["decodeJson"]=decodeJson;
                    sSettings.save();
                    break;
                }
            }
        }

        function onResetDecodeComplete(decodeID){
            refreshDecodeOnList(decodeID);
        }

        function onCollectComplete() {
            dataListView.resetScrollBarPosition();
        }
    }

    Column{
        id: topColumn
        spacing: 20
        width: parent.width
        onWidthChanged: calcShortcutButtonWidth()
        anchors{
            top: parent.top
            left: parent.left
            right: parent.right
            margins: 14
        }
        Text {
            text: qsTr("协议解码")
            font.pixelSize: 18
            color: Config.mouseCheckColor
            height: 25
        }

        Item{
            width: parent.width
            height: decodeTitleText.height+shortcutButtonRow.height+decodeListRow.height+20
            Text {
                id: decodeTitleText
                text: qsTr("协议")
                font.pixelSize: 16
                width: parent.width
                height: contentHeight
                color: Config.textColor
                ImageButton{
                    id: decodeAddButton
                    anchors{
                        right: parent.right
                        verticalCenter: parent.verticalCenter
                    }
                    enabled: sConfig.loopState===-1 && !sConfig.isRun
                    imageSource: "resource/icon/DecodeAdd.png"
                    imageEnterSource: imageSource
                    imageDisableSource: "resource/icon/"+Config.tp+"/DecodeAddDisable.png"
                    width: 16
                    height: 16
                    onPressed: decodeSearchPopup.open();
                    QToolTip{
                        parent: parent
                        isShow: parent.containsMouse
                        showText: qsTr("添加解码器")
                        direction: 2
                    }
                }
                DecodeSearchPopup{
                    id: decodeSearchPopup
                    parent: decodeAddButton
                    maxBottom: dataListView.height+65
                }
                DecodeDocPopup{
                    id: decodeDocPopup
                    parent: decodeAddButton
                    y: decodeSearchPopup.y+46
                    x: decodeSearchPopup.x-contentWidth-10
                }
            }
            Row{
                id: shortcutButtonRow
                spacing: 6
                height: children.length===0?0:20
                anchors{
                    top: decodeTitleText.bottom
                    topMargin: 10
                }
                Component.onCompleted: refReshShortcutButton();
            }

            QMenuPopup{
                id: decodeMenuPopup
                x: decodeAddButton.x+decodeAddButton.width-width
                data: [{"showText":qsTr("编辑"),"shortcutKey":"","seleteType":0,"buttonIndex":0},
                    {"showText":qsTr("重新解码"),"shortcutKey":"","seleteType":0,"buttonIndex":1},
                    {"showText":qsTr("删除"),"shortcutKey":"","seleteType":0,"buttonIndex":2}]
                onSelect: {
                    if(index===0)
                        setDecodeWindow(decodeListRow.selectJson);
                    else if(index===1)
                        jsonStart(decodeListRow.selectJson);
                    else if(index===2)
                        sSignal.removeDecode(decodeListRow.selectDecodeID,0);
                }
            }

            ListView{
                property string selectDecodeID
                property var selectJson
                property int selectIndex: -1
                id: decodeListRow
                anchors{
                    top: shortcutButtonRow.bottom
                    topMargin: 10
                }
                flickableDirection: Flickable.AutoFlickDirection
                boundsBehavior: Flickable.StopAtBounds
                width: parent.width+topColumn.anchors.margins
                height: decodeListModel.count*28>112?112:decodeListModel.count*28
                contentHeight: decodeListModel.count*28
                contentWidth: width
                ScrollBar.vertical: vbar
                clip: true
                currentIndex: -1
                onContentHeightChanged: vbar.size=height/contentHeight
                QScrollBar {
                    id: vbar
                    anchors{
                        top: parent.top
                        right: parent.right
                    }
                    onPositionChanged: decodeMenuPopup.close();
                }
                model: ListModel{
                    id: decodeListModel
                }
                delegate: Item{
                    id: decodeItem
                    width: parent?parent.width:0
                    height: 28
                    MouseArea{
                        id: decodeItemMouseArea
                        anchors.fill: parent
                        hoverEnabled: true
                        onEntered: decodeListRow.selectIndex=index
                        onExited: decodeListRow.selectIndex=-1
                    }

                    Row{
                        spacing: 6
                        QProgressBar{
                            loadingBackground: value===100?Config.channelDataColor[colorIndex%16]:Config.iceBlue
                            loadingTextColor: value===100?"white":"#383838"
                            value: dataValue
                            width: 35
                            anchors.verticalCenter: parent.verticalCenter
                        }
                        Text {
                            text: showText
                            font{
                                pixelSize: 14
                                bold: true
                            }
                            color: Config.textColor
                        }
                    }
                    ImageButton{
                        id: moreButton
                        anchors{
                            right: parent.right
                            rightMargin: topColumn.anchors.margins+2
                            verticalCenter: parent.verticalCenter
                            verticalCenterOffset: -5
                        }
                        imageWidth: 2
                        imageHeight: 12
                        imageSource: "resource/icon/"+Config.tp+"/More.png"
                        imageEnterSource: "resource/icon/"+Config.tp+"/MoreEnter.png"
                        imageDisableSource: "resource/icon/"+Config.tp+"/MoreDisable.png"
                        width: 12
                        height: 16
                        enabled: decodeAddButton.enabled
                        visible: containsMouse||index===decodeListRow.selectIndex
                        onPressed: {
                            decodeListRow.selectDecodeID=decodeID
                            decodeListRow.selectJson=decodeJson
                            decodeMenuPopup.y=decodeItem.y+height+68-decodeListRow.contentY
                            decodeMenuPopup.visible=true
                        }
                    }
                    Connections{
                        target: sSignal

                        function onEditDecode(decodeID_){
                            if(decodeID_===decodeID){
                                decodeListRow.selectDecodeID=decodeID
                                decodeListRow.selectJson=decodeJson
                                setDecodeWindow(decodeListRow.selectJson);
                            }
                        }

                        function onResetAllDecode(){
                            startJsonTimer.json=decodeJson;
                            startJsonTimer.start();
                        }

                        function onVernierMoveComplete(id){
                            if(sConfig.loopState===-1 && !sConfig.isRun){
                                let isSet=false;
                                let json=decodeJson;
                                if(parseInt(json["main"]["startID"])>=0&&parseInt(json["main"]["startID"])===id){
                                    let data=vernierListModel.getAt(id);
                                    json["main"]["start"]=data.position.toString();
                                    isSet=true;
                                }
                                if(parseInt(json["main"]["endID"])>=0&&parseInt(json["main"]["endID"])===id){
                                    let data=vernierListModel.getAt(id);
                                    json["main"]["end"]=data.position.toString();
                                    isSet=true;
                                }
                                if(isSet)
                                    sSignal.resetDecode(json["main"]["decodeID"],json,json["main"]["channelList"],false);
                            }
                        }
                    }
                }
            }
            Rectangle{
                height: 1
                width: parent.width
                color: Config.lineColor
                anchors.top: decodeListRow.bottom
            }
        }
        Item{
            width: parent.width
            height: dataText.height+inputRow.height+10
            Text {
                id: dataText
                text: qsTr("数据")
                color: Config.textColor
                font.pixelSize: 16
                height: 25
                anchors{
                    top: parent.top
                    left: parent.left
                    right: parent.right
                }
            }
            Row{
                id: inputRow
                spacing: 10
                height: 26
                anchors{
                    top: dataText.bottom
                    topMargin: 10
                    left: parent.left
                    right: parent.right
                }
                QTextInput{
                    id: searchInput
                    width: topColumn.width-66
                    height: parent.height
                    onShowTextChanged: {
                        if(showText)
                            decodeTableModel.search(showText);
                        else
                            decodeTableModel.exitSearch();
                        decodeTableModel.refresh();
                        dataListView.resetScrollBarPosition();
                        dataListView.selectRow=-1;
                    }
                    onContainsMouseChanged: sSignal.showSetTips(3,containsMouse);
                }

                ImageButton{
                    id: bindButton
                    anchors.verticalCenter: parent.verticalCenter
                    imageWidth: 16
                    imageHeight: 14
                    imageSource: "resource/icon/"+Config.tp+(sConfig.isBind?"/UnBind":"/Bind")+".png"
                    imageEnterSource: "resource/icon/"+Config.tp+(sConfig.isBind?"/UnBind":"/Bind")+"Enter.png"
                    imageDisableSource: "resource/icon/"+Config.tp+(sConfig.isBind?"/UnBind":"/Bind")+"Disable.png"
                    enabled: decodeTableModel.isExport
                    width: 16
                    height: 16
                    onPressed: sConfig.isBind=!sConfig.isBind
                    QToolTip{
                        parent: parent
                        isShow: parent.containsMouse
                        showText: sConfig.isBind? qsTr("取消跟随") : qsTr("视图跟随")
                        direction: 2
                    }
                }

                ImageButton{
                    id: exportButton
                    anchors.verticalCenter: parent.verticalCenter
                    imageWidth: 16
                    imageHeight: 14
                    imageSource: "resource/icon/"+Config.tp+"/ExportDecode.png"
                    imageEnterSource: "resource/icon/"+Config.tp+"/ExportDecodeEnter.png"
                    imageDisableSource: "resource/icon/"+Config.tp+"/ExportDecodeDisable.png"
                    enabled: decodeTableModel.isExport
                    onEnabledChanged: exportPopup.close();
                    width: 16
                    height: 16
                    onPressed: exportPopup.open()
                    QToolTip{
                        parent: parent
                        isShow: parent.containsMouse
                        showText: qsTr("导出表格")
                        direction: 2
                    }
                }

                ImageButton{
                    id: searchMoreButton
                    anchors.verticalCenter: parent.verticalCenter
                    imageWidth: 2
                    imageHeight: 12
                    imageSource: "resource/icon/"+Config.tp+"/More.png"
                    imageEnterSource: "resource/icon/"+Config.tp+"/MoreEnter.png"
                    imageDisableSource: "resource/icon/"+Config.tp+"/MoreDisable.png"
                    enabled: decodeAddButton.enabled
                    width: 10
                    height: 16
                    onPressed: {
                        if(decodeShowRowList.length>0)
                            decodeDataSearchPopup.open();
                    }
                    QToolTip{
                        parent: parent
                        isShow: parent.containsMouse
                        showText: qsTr("数据筛选")
                        direction: 2
                    }
                }
                ExportPopup{
                    id: exportPopup
                    parent: exportButton
                    maxBottom: dataListView.height+65
                    headers: [qsTr("索引"),qsTr("开始"),qsTr("时间"),qsTr("类型"),qsTr("数据")]
                    onExportClicked: {
                        decodeTableModel.saveTableReady(checks);
                        exportDialog.saveType=Config.SaveType.TableCSV;
                        exportDialog.open();
                        exportPopup.close();
                    }
                }

                DecodeDataSearchPopup{
                    id: decodeDataSearchPopup
                    parent: searchMoreButton
                    maxBottom: dataListView.height+65
                    onSelectDecode: {
                        refreshDecodeOnList(decodeID);
                        refreshShow();
                    }
                    onSelectRow: {
                        dataListView.selectRow=-1;
                        for(var i in decodeShowRowList){
                            if(decodeShowRowList[i]["isSelect"])
                            {
                                for(var j in decodeShowRowList[i]["rowList"]){
                                    if(decodeShowRowList[i]["rowList"][j]["showText"]===rowName)
                                    {
                                        decodeShowRowList[i]["rowList"][j]["isSelect"]=isSelect;
                                        if(isSelect)
                                            decodeTableModel.appendShow(rowName);
                                        else
                                            decodeTableModel.removeShow(rowName);
                                        if(isRefresh){
                                            decodeTableModel.refresh();
                                            dataListView.resetScrollBarPosition();
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    DecodeDataTableView {
        id: dataListView
        height: parent.height - topColumn.implicitHeight-24
        width: parent.width
        anchors{
            top: topColumn.bottom
            topMargin: 10
            left: parent.left
            right: parent.right
        }
        columnsWidth: [67,67,67,67]//总数=组件宽度(268)
        headerList: [qsTr("开始"),qsTr("时间"),qsTr("类型"),qsTr("数据")]
        view.model: decodeTableModel
    }

    Timer{
        property var json
        id: startJsonTimer
        interval: 10
        onTriggered: jsonStart(json);
    }

    Connections{
        id: con
        ignoreUnknownSignals: true
        function onDataSend(decodeJson)
        {
            var isSet=true;
            for(var i in sSettings.favoritesList){
                if(sSettings.favoritesList[i]===lastDecode){
                    isSet=false;
                    break;
                }

            }
            if(isSet){
                while(sSettings.favoritesList.length>2)
                    sSettings.favoritesList.shift();
                sSettings.favoritesList.push(lastDecode);
                refReshShortcutButton();
                sSettings.save();
            }
            startJsonTimer.json=decodeJson;
            startJsonTimer.start();
        }

        function onClosing(){
            Config.isSetModel=false;
        }
    }

    Timer{
        id: calcShortcutButtonWidthTimer
        interval: 100
        onTriggered: calcShortcutButtonWidth();
    }

    function jsonStart(decodeJson){
        dataListView.resetScrollBarPosition();
        var channelList=[];
        for(let i in decodeJson){
            channelList=channelList.concat(jsonGetChannelList(decodeJson[i]["channels"]));
            channelList=channelList.concat(jsonGetChannelList(decodeJson[i]["opt_channels"]));
        }
        channelList = [...new Set(channelList)];
        decodeJson["main"]["channelList"]=channelList;
        if(decodeJson["main"]["decodeID"])
            sSignal.resetDecode(decodeJson["main"]["decodeID"],decodeJson,channelList,true);
        else{
            if(channelList.length===1&&channelList[0]==="-" || channelList.length<0)
                return;
            var decodeID=controller.newDecode();
            decodeJson["main"]["decodeID"]=decodeID;
            decodeJson["main"]["colorIndex"]=sConfig.decodeColorIndex;
            let showText=decodeJson["main"]["first"];
            if(decodeJson["main"]["second"] && decodeJson["main"]["second"].length>0)
                showText=decodeJson["main"]["second"][decodeJson["main"]["second"].length-1];
            decodeJson["main"]["showText"]=showText;
            if(controller.decode(decodeID, decodeJson)){
                sConfig.decodeColorIndex++;
                //添加busy信息
                vernierListModel.appendBusy(decodeJson["main"]["startID"],decodeID,0,showText+" - Start");
                vernierListModel.appendBusy(decodeJson["main"]["endID"],decodeID,1,showText+" - End");
                decodeListModel.append({"dataValue":0,"decodeJson":decodeJson,"colorIndex":decodeJson["main"]["colorIndex"],
                                           "showText":showText,"decodeID":decodeID})
                sSettings.decodeJson.push({"decodeJson":decodeJson,"decodeID":decodeID});
                sSettings.save();
                sSignal.decode(decodeID,decodeJson,channelList);
                sConfig.selectDecodeID=decodeID;
                var rowList=decodeTableModel.initShow(decodeID);
                if(rowList.length>0)
                {
                    var list=[];
                    for(var j in rowList)
                    {
                        decodeTableModel.appendShow(rowList[j]);
                        list.push({"showText":rowList[j],"isSelect":true});
                    }
                    for(var k in decodeShowRowList)
                        decodeShowRowList[k]["isSelect"]=false;
                    decodeShowRowList.push({"name":showText,"decodeID":decodeID,"colorIndex":decodeJson["main"]["colorIndex"],
                                               "rowList":list,"isSelect":true});
                    decodeTableModel.refresh();
                }
            }else
                controller.removeDecode(decodeID);
        }
    }

    function jsonGetChannelList(json){
        var list=[];
        for(var i in json)
            list.push(json[i]["value"]);
        return list;
    }

    function getVernierJson(){
        var ret=[];
        for(var i=0;i<vernierListModel.count();i++){
            var temp={};
            var data=vernierListModel.get(i);
            temp["name"]=data.name;
            temp["id"]=data.vernierID;
            temp["position"]=data.position;
            ret.push(temp);
        }
        return ret;
    }

    function showDecodeWindow(decodeName){
        if(decodeName==="")
            return;
        if(!decodeSearchListModel.containName(decodeName))
        {
            sessionError.error_msg=qsTr("没有找到该解码器，无法创建窗口。")
            return;
        }
        let decodeJson=root.decodeJson["data"];
        for(let k in decodeJson){
            if(decodeJson[k]["name"].toString().toLowerCase()===decodeName.toLowerCase())
            {
                if(decodeJson[k]["parent"]==="NULL")
                {
                    sessionError.error_msg=qsTr("没有找到该解码器的上层协议，无法创建窗口。")
                    return;
                }
                else
                    break;
            }
        }
        lastDecode=decodeName;
        var component = Qt.createComponent("qrc:/qml/window/DecodeSetWindow.qml");
        //屏蔽关闭的通道
        let array=[];
        for(let i in sSettings.channelsSet){
            if(sSettings.channelsSet[i].enable)
                array.push(parseInt(i));
        }
        if (component.status === Component.Ready){
            var subParams = {
                "decodeJson": decodeJson,
                "vernierJson": getVernierJson(),
                "channels": getChannelName(array),
                "screen": root.getScreenQRect(),
                "initDecode": decodeName
            }
            var showWindow = component.createObject(root, subParams);
            showWindow.showNormal();
            con.target=showWindow;
        }
    }

    function setDecodeWindow(selectJson){
        var component = Qt.createComponent("qrc:/qml/window/DecodeSetWindow.qml");
        //屏蔽关闭的通道
        let array=[];
        for(let i in sSettings.channelsSet){
            if(sSettings.channelsSet[i].enable)
                array.push(parseInt(i));
        }
        if (component.status === Component.Ready){
            for(let j=0; j<selectJson["main"]["channelList"].length;j++){
                if(selectJson["main"]["channelList"][j].toString()!=="-" && array.indexOf(parseInt(selectJson["main"]["channelList"][j]))===-1)
                    array.push(parseInt(selectJson["main"]["channelList"][j]))
            }
            var subParams = {
                "decodeJson": root.decodeJson["data"],
                "vernierJson": getVernierJson(),
                "channels": getChannelName(array),
                "screen": root.getScreenQRect(),
                "initJson": selectJson
            }
            var showWindow = component.createObject(root, subParams);
            showWindow.show();
            con.target=showWindow;
        }
    }

    function getChannelName(array){
        let js=[];
        for(let i in array){
            let tmp={};
            tmp.cost=array[i].toString();
            tmp.showText="D"+tmp.cost+": "+controller.getChannelName(array[i]);
            js.push(tmp);
        }
        return js;
    }

    function refReshShortcutButton(){
        var i;
        for(i in shortcutButtonList)
            shortcutButtonList[i].destroy();
        shortcutButtonRow.children=[];
        shortcutButtonList=[];
        for(i in sSettings.favoritesList){
            if(typeof(sSettings.favoritesList[i])==="string"){
                var item =Qt.createQmlObject('import "../../style"; TextButton{text: "'+sSettings.favoritesList[i]+'"; isElide: true ; height: parent?parent.height:0; textColor: "'+
                                             Config.iceBlue+'"; backgroundRectangle.radius: 10; enabled: decodeAddButton.enabled;backgroundRectangle.border.width: 1; backgroundRectangle.border.color: enabled ? "'+
                                             Config.iceBlue+'" : "'+Config.textDisabledColor+'"; onClicked: showDecodeWindow(text);}',shortcutButtonRow);
                shortcutButtonList.push(item);
            }
        }
        calcShortcutButtonWidth();
    }

    function calcShortcutButtonWidth(){
        var i;
        let parentWidth=topColumn.implicitWidth-(shortcutButtonRow.children.length-1)*6;
        let maxWidth=0;
        for(i in shortcutButtonRow.children){
            maxWidth+=shortcutButtonRow.children[i].implicitWidth;
            shortcutButtonRow.children[i].width=shortcutButtonRow.children[i].implicitWidth;
        }
        if(maxWidth>parentWidth){
            let average=parentWidth/shortcutButtonRow.children.length;
            let surplus=0;
            for(i in shortcutButtonRow.children){
                if(shortcutButtonRow.children[i].implicitWidth<average)
                    surplus+=average-shortcutButtonRow.children[i].implicitWidth;
            }
            for(i in shortcutButtonRow.children){
                if(shortcutButtonRow.children[i].implicitWidth>average){
                    if(surplus>0){
                        if(average+surplus>shortcutButtonRow.children[i].implicitWidth)
                            surplus=average+surplus-shortcutButtonRow.children[i].implicitWidth;
                        else{
                            shortcutButtonRow.children[i].width=average+surplus;
                            surplus=0;
                        }
                    }else if(average<shortcutButtonRow.children[i].implicitWidth)
                        shortcutButtonRow.children[i].width=average;
                }
            }
        }
        if(calcShortcutButtonWidthTimer.running)
            calcShortcutButtonWidthTimer.stop();
        calcShortcutButtonWidthTimer.start();
    }

    function refreshDecodeOnList(decodeID){
        sConfig.selectDecodeID=decodeID;
        var rowList=decodeTableModel.initShow(decodeID);
        var list=[];
        for(var j in rowList)
        {
            decodeTableModel.appendShow(rowList[j]);
            list.push({"showText":rowList[j],"isSelect":true});
        }
        for(var k in decodeShowRowList){
            if(decodeShowRowList[k]["decodeID"]===decodeID)
            {
                decodeShowRowList[k]["rowList"]=list;
                decodeShowRowList[k]["isSelect"]=true;
            }else
                decodeShowRowList[k]["isSelect"]=false;
        }
        decodeTableModel.setStop(false);
        decodeTableModel.refresh();
        dataListView.resetScrollBarPosition();
    }
}
