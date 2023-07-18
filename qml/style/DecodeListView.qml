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

ListView {
    property var availableChannels_
    property var outJson
    property string firstDecode
    property string selectFavorites
    property point titlePoint
    property var showClashRecode: []
    signal showClashTipsSignal(var showText_, var state_)
    signal removeItem(var name)
    signal dataChanged();

    onRemoveItem: {
        for(var i=0;i<model.count;i++){
            if(model.get(i)["name"]===name)
                model.remove(i);
        }
        delete outJson[name];
        for(var j in outJson["main"]["second"]){
            if(outJson["main"]["second"][j]===name){
                outJson["main"]["second"].splice(j,1);
                break;
            }
        }
    }

    id: listView
    headerPositioning: ListView.OverlayHeader
    flickableDirection: Flickable.AutoFlickDirection
    boundsBehavior: Flickable.StopAtBounds
    ScrollBar.vertical: vbar
    ScrollBar.horizontal: hbar
    delegate: listDelegate
    contentWidth: width
    clip: true
    currentIndex: -1
    onFirstDecodeChanged: outJson={"main":{"first":firstDecode,"second":[]}};

    ScrollBar {
        id: hbar
        policy: ScrollBar.AlwaysOff
    }

    QScrollBar {
        id: vbar
        anchors{
            top: parent.top
            right: parent.right
        }
    }
    move: Transition {
        NumberAnimation { properties: "x,y"; duration: 100 }
    }

    Component{
        id: listDelegate
        ListView{
            width: listView.contentWidth-20
            height: contentHeight
            clip: true
            currentIndex: -1
            delegate: listItemDelegate
            flickableDirection: Flickable.AutoFlickDirection
            boundsBehavior: Flickable.StopAtBounds
            model: ListModel{
                id: listModel
            }
            Component.onCompleted: {
                if(outJson["main"]["first"]!==name&&outJson["main"]["second"].indexOf(name)===-1)
                    outJson["main"]["second"].push(name);
                outJson[name]={"annotation_rows":[],"channels":[],"opt_channels":[],"options":[]}
                var itemJson={"name":name,"id":name,"showText":name,"type":0,"isShow":true,"showNameText":"","itemHeight":34,"values":[{"showText":"-","cost":"-"}],"def":"-","dataType":1};
                var json={};
                var i,k;
                var setValue;
                json["id"]=id;
                json["name"]=name;
                outJson[name]["main"]=json;
                if(annotation_rows.count===0)
                    itemJson["itemHeight"]=37;
                listModel.append(itemJson);
                if(main["isSingle"]||annotation_rows.count===0&&annotations.count===1){
                    json={};
                    json["id"]=annotations.get(0)["id"];
                    setValue=true;
                    if(annotation_rows.count===1&&typeof(annotation_rows.get(0)["value"])!=="undefined")
                        setValue=annotation_rows.get(0)["value"];
                    json["desc"]=annotations.get(0)["desc"];
                    json["isShow"]=setValue;
                    let arr=[];
                    arr.push(0);
                    json["ann_classes"]=arr;
                    outJson[name]["annotation_rows"][0]=json;
                    outJson[name]["main"]["isSingle"]=true;
                }else{
                    for(i=0;i<annotation_rows.count;i++){
                        json={};
                        json["id"]=annotation_rows.get(i)["id"];
                        setValue=true;
                        if(typeof(annotation_rows.get(i)["value"])!=="undefined")
                            setValue=annotation_rows.get(i)["value"];
                        json["desc"]=annotation_rows.get(i)["desc"];
                        json["isShow"]=setValue;
                        let arr=[];
                        for(k=0;k<annotation_rows.get(i)["ann_classes"].count;k++)
                            arr.push(annotation_rows.get(i)["ann_classes"].get(k)["data"]);
                        json["ann_classes"]=arr;
                        outJson[name]["annotation_rows"][i]=json;
                    }
                }
                
                itemJson["type"]=2;
                itemJson["itemHeight"]=40;
                for(var j in availableChannels_)
                    itemJson["values"].push(availableChannels_[j]);
                for(i=0;i<channels.count;i++){
                    json={};
                    json["id"]=channels.get(i)["id"];
                    json["name"]=channels.get(i)["name"];
                    json["desc"]=channels.get(i)["desc"];
                    setValue="-";
                    if(typeof(channels.get(i)["def"])!=="undefined")
                        setValue=channels.get(i)["def"];
                    json["value"]=setValue;
                    outJson[name]["channels"][i]=json;
                    itemJson["id"]=json["id"];
                    itemJson["def"]=setValue;
                    itemJson["showNameText"]=json["name"];
                    itemJson["showText"]="  ("+json["desc"]+")";
                    listModel.append(itemJson);
                }
                itemJson["type"]=3;
                for(i=0;i<opt_channels.count;i++){
                    json={};
                    json["id"]=opt_channels.get(i)["id"];
                    json["name"]=opt_channels.get(i)["name"];
                    json["desc"]=opt_channels.get(i)["desc"];
                    setValue="-";
                    if(typeof(opt_channels.get(i)["def"])!=="undefined")
                        setValue=opt_channels.get(i)["def"];
                    json["value"]=setValue;
                    outJson[name]["opt_channels"][i]=json;
                    itemJson["id"]=json["id"];
                    itemJson["def"]=setValue;
                    itemJson["showNameText"]=json["name"];
                    itemJson["showText"]="  ("+json["desc"]+")";
                    listModel.append(itemJson);
                }
                itemJson["type"]=4;
                for(i=0;i<options.count;i++){
                    var v=options.get(i)["values"];
                    json={};
                    json["id"]=options.get(i)["id"];
                    json["value"]=options.get(i)["def"];
                    outJson[name]["options"][i]=json;
                    itemJson["id"]=json["id"];
                    itemJson["showText"]=options.get(i)["desc"];
                    itemJson["values"]=[];
                    itemJson["def"]=json["value"];
                    if(typeof(v) === "undefined"){
                        if(options.get(i)["type"]===2)
                            itemJson["dataType"]=2;
                        else
                            itemJson["dataType"]=0;
                    }
                    else{
                        itemJson["dataType"]=1;
                        for(k=0;k<v.count;k++){
                            itemJson["values"].push({"showText":v.get(k)["data"],"cost":v.get(k)["data"]})
                        }
                    }
                    listModel.append(itemJson);
                }
            }
        }
    }

    Component{
        id: listItemDelegate
        Row{
            width: listView.contentWidth-20
            height: itemHeight+(showErrorColumn.implicitHeight>0?showErrorColumn.implicitHeight+5:0)
            Connections{
                target: listView
                function onShowClashTipsSignal(showText_, state_)
                {
                    if(state_){
                        if(showText_===showText)
                            combobox.isError=true;
                    }else
                        combobox.isError=false;
                }
            }
            Item{
                id: showItem
                height: showTextRow.height
                width: parent.width/2
                clip: true
                anchors{
                    top: parent.top
                    topMargin: 4
                }
                Item{
                    id: showItemItem
                    width: showTextRow.width>showItem.width-10?showItem.width-10:showTextRow.width
                    height: showTextRow.height
                    Row{
                        id: showTextRow
                        width: showItemNameText.contentWidth+showItemDescText.contentWidth
                        height: showItemDescText.height<showItemNameText.height?showItemNameText.height:showItemDescText.height
                        spacing: 0
                        Text {
                            id: showItemNameText
                            text: showNameText?showNameText:""
                            visible: type>=2 && type<=3
                            font{
                                bold: true
                                pixelSize: 14
                            }
                            elide: Text.ElideRight
                            color: Config.textColor
                            anchors.verticalCenter: parent.verticalCenter
                        }
                        Text {
                            id: showItemDescText
                            text: showText
                            font{
                                bold: type===0
                                pixelSize: type===0?16:14
                            }
                            elide: Text.ElideRight
                            color: type===0?Config.mouseCheckColor:Config.textColor
                            anchors{
                                verticalCenter: parent.verticalCenter
                                verticalCenterOffset: showItemNameText.visible?-1:0
                            }
                        }
                    }
                }
                Text {
                    visible: type===2
                    text: "*"
                    font.pixelSize: 14
                    color: Config.red
                    anchors{
                        left: showItemItem.right
                        leftMargin: 3
                        top: parent.top
                    }
                }
            }
            Item{
                height: parent.height
                width: parent.width/2
                clip: true
                anchors{
                    top: parent.top
                }
                Column{
                    id: showErrorColumn
                    spacing: 5
                    anchors{
                        top: combobox.bottom
                        topMargin: 5
                    }
                    Text {
                        id: showRequiredText
                        visible: showRequiredTips && type===2
                        text: qsTr("此为必填项目，请选择。")
                        font.pixelSize: 12
                        color: Config.red
                    }
                }
                QTextInput{
                    id: textInput
                    visible: type>=2 && dataType===2
                    width: parent.width
                    height: 30
                    onShowTextChanged: {
                        if(type>=2 && dataType===2){
                            for(var i in outJson[name]["options"]){
                                if(outJson[name]["options"][i]["id"]===id){
                                    dataChanged();
                                    outJson[name]["options"][i]["value"]=showText;
                                    break;
                                }
                            }
                        }
                    }
                    Component.onCompleted: {
                        if(type>=2 && dataType===2)
                            showText=def;
                    }
                }

                QComboBox{
                    id: combobox
                    visible: type>=2 && dataType===1
                    width: parent.width
                    height: 30
                    popupMaxHeight: 30*5
                    model: ListModel{
                        id: comboboxListView
                    }
                    anchors{
                        top: parent.top
                    }
                    onCurrentModelChildrenChanged: {
                        if(currentModelChildren&&typeof(currentModelChildren)!="undefined"){
                            if(type>=2 && dataType===1){
                                var temp="";
                                switch(type){
                                case 2:
                                    temp="channels";
                                    showClashRecode[showText]=currentModelChildren.cost;
                                    calcClash();
                                    break;
                                case 3:
                                    temp="opt_channels";
                                    showClashRecode[showText]=currentModelChildren.cost;
                                    calcClash();
                                    break;
                                case 4:
                                    temp="options";
                                    break;
                                }
                                if(temp!=""){
                                    for(var i in outJson[name][temp]){
                                        if(outJson[name][temp][i]["id"]===id){
                                            dataChanged();
                                            outJson[name][temp][i]["value"]=currentModelChildren.cost;
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    }
                    Component.onCompleted: {
                        if(type>=2 && dataType===1)
                        {
                            var isSetDef=false;
                            var i;
                            for(i=0;i<values.count;i++){
                                comboboxListView.append({"showText":values.get(i)["showText"],"cost":values.get(i)["cost"]})
                                if(values.get(i)["cost"]===def)
                                {
                                    currentIndex=i;
                                    isSetDef=true;
                                }
                            }
                            if(!isSetDef){
                                for(i=0;i<values.count;i++){
                                    if(values.get(i)["cost"].indexOf(def)!==-1)
                                    {
                                        currentIndex=i;
                                        isSetDef=true;
                                        break;
                                    }
                                }
                            }
                            if(!isSetDef)
                                currentIndex=0;
                        }else{
                            comboboxListView.append({"showText": "","cost": ""})
                            currentIndex=0;
                        }
                    }

                    function calcClash(){
                        showClashTips=false;
                        listView.showClashTipsSignal("",false);
                        for(let i in showClashRecode){
                            for(let ii in showClashRecode){
                                if(i!==ii && showClashRecode[i]===showClashRecode[ii]){
                                    if(showClashRecode[i]!=="-"){
                                        listView.showClashTipsSignal(i,true);
                                        listView.showClashTipsSignal(ii,true);
                                        showClashTips=true;
                                    }
                                }
                            }
                        }
                    }
                }

                QSpinBox{
                    id: spinBox
                    visible: type>=2 && dataType===0
                    width: parent.width
                    height: 30
                    from: -2147483640
                    to: 2147483646
                    decimalDigits: 0
                    postfixText: ""
                    onValueChanged: {
                        if(type>=2 && dataType===0){
                            for(var i in outJson[name]["options"]){
                                if(outJson[name]["options"][i]["id"]===id){
                                    dataChanged();
                                    outJson[name]["options"][i]["value"]=value;
                                    break;
                                }
                            }
                        }
                    }
                    Component.onCompleted: {
                        if(type>=2 && dataType===0){
                            if(def.indexOf(".")!==-1){
                                decimalDigits=def.split(".")[1].length;
                            }
                            value=def;
                        }
                    }
                }
            }
        }
    }
}
