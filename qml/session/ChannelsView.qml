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
import "./channel"
import "../config"
import "../style"

Rectangle{
    property int contentYRecode: -1
    property int channelHeight: 62
    property var parentItem
    id: main
    color: Config.background2Color

    Flickable{
        id: listviewParentItem
        contentHeight: list_view.height<main.height?list_view.height:list_view.height
        contentY: vbar.position*(contentHeight/(1-vbar.size))
        onContentYChanged: setContentY()
        onContentHeightChanged: setContentY()
        flickableDirection: Flickable.AutoFlickDirection
        boundsBehavior: Flickable.StopAtBounds
        height: contentHeight>main.height?main.height:contentHeight
        anchors{
            top: parent.top
            right: vbar.left
            left: parent.left
        }
        Component.onCompleted: {
            var heightCount=[];
            var jsonArray=[{"data":{"channelID": -1,"channelName":"NULL","isDisable":false,
                                   "channelVisible":true, "decodeChannelName": "",
                                   "channelHeight_":1,"minChannelHeight":1}}];
            for(var i=0;i<sessionChannelCount_;i++)
            {
                jsonArray.push({"data":{"channelID": i,"channelName":"","isDisable":false,
                                       "channelVisible":sSettings.channelsSet[i].enable, "decodeChannelName": "",
                                       "channelHeight_":channelHeight,"minChannelHeight":62}})
                if(sSettings.channelsSet[i].enable)
                    heightCount.push({"data":i})
            }
            dataModel.append({"channelsData": jsonArray,"initHeight":heightCount,"isExpand":true,"decode":{"isDecode":false}})
            list_view.update();
            list_view.forceLayout();
            parentItem=list_view.itemAtIndex(0);
        }

        ListView{
            id: list_view
            interactive: false
            reuseItems: false
            orientation: ListView.Vertical
            snapMode: ListView.NoSnap
            width: parent.width
            height: contentHeight
            onContentHeightChanged:{
                if(contentHeight==0){
                    positionViewAtIndex(0,ListView.Beginning)
                }
            }
            move: Transition {
                NumberAnimation { property: "y"; duration: 10 }
            }
            moveDisplaced: Transition {
                NumberAnimation { property: "y"; duration: 150 }
            }
            model: ListModel {
                id: dataModel
            }
            delegate: ChannelRow{}
        }

    }

    Rectangle{
        width: 10
        height: parent.height
        anchors{
            right: parent.right
            top: parent.top
        }
        color: Config.background2Color
        z: 1
    }

    Timer{
        property bool isUp: false
        id: scrollBarTimer
        interval: 100
        repeat: true
        onTriggered: sSignal.channelWheel(isUp)
    }

    ImageButton{
        id: topScrollBarButton
        width: vbar.width
        height: width
        z: 5
        enabled: vbar.position!==0
        imageSource: "resource/icon/"+Config.tp+"/ScrollBarButton.png"
        imageEnterSource: "resource/icon/"+Config.tp+"/ScrollBarButtonEnter.png"
        imageDisableSource: "resource/icon/"+Config.tp+"/ScrollBarButtonDisable.png"
        backgroundColor: Config.lineColor
        backgroundMouseEnterColor: "#909090"
        backgroundPressedColor: "#909090"
        rotation: 90
        anchors{
            top: parent.top
            right: parent.right
        }
        onPressed: {
            scrollBarTimer.isUp=true;
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
        id: vbar
        z: 5
        width: 10
        height: parent.height-20
        padding: 0
        anchors{
            top: topScrollBarButton.bottom
            right: parent.right
        }
        onPositionChanged: setContentY()
        onSizeChanged: setContentY()
        size: main.height/listviewParentItem.contentHeight
        visible: true
        policy: ScrollBar.AlwaysOn
    }
    ImageButton{
        width: vbar.width
        height: width
        z: 5
        enabled: (vbar.position+vbar.size)<1
        imageSource: "resource/icon/"+Config.tp+"/ScrollBarButton.png"
        imageEnterSource: "resource/icon/"+Config.tp+"/ScrollBarButtonEnter.png"
        imageDisableSource: "resource/icon/"+Config.tp+"/ScrollBarButtonDisable.png"
        backgroundColor: Config.lineColor
        backgroundMouseEnterColor: "#909090"
        backgroundPressedColor: "#909090"
        rotation: 270
        anchors{
            top: vbar.bottom
            right: parent.right
        }
        onPressed: {
            scrollBarTimer.isUp=false;
            scrollBarTimer.start();
            scrollBarTimer.triggered();
        }
        onReleased: scrollBarTimer.stop();
        onEnabledChanged: {
            if(!enabled)
                scrollBarTimer.stop();
        }
    }

    Connections{
        target: sSignal
        function onChannelWheel(isUp){
            if(listviewParentItem.contentHeight<main.height)
                return;
            let stepSize=0.1;
            if(isUp)
                vbar.position=(vbar.position-stepSize<0?0:vbar.position-stepSize);
            else if(vbar.position<1)
            {
                let temp=vbar.position+stepSize;
                vbar.position=temp>1.0-vbar.size?1.0-vbar.size:temp;
            }
        }

        function onDecode(decodeID,decodeJson,channelList){
            setDecodeChannel(0,decodeJson,decodeID,channelList);
        }

        function onResetDecode(decodeID,decodeJson,channelList,isDelModel){
            var removeChannel=[];
            for(var i=0;i<dataModel.count;i++){
                var item=dataModel.get(i);
                if(item["decode"]["isDecode"]&&item["decode"]["decodeID"]===decodeID){
                    if(isDelModel){
                        for(var j=0; j<item["channelsData"].count;j++){
                            for(var k=0;k<channelList.length;k++){
                                if(item["channelsData"].get(j)["data"]["channelID"].toString()===channelList[k].toString())
                                    break;
                                if((k+1)===channelList.length)
                                    removeChannel.push(item["channelsData"].get(j)["data"]["channelID"]);
                            }
                        }
                        for(var arr in removeChannel)
                            sSignal.setChannelDisable(removeChannel[arr],false);
                        if(controller.resetDecode(decodeID,decodeJson)){
                            dataModel.remove(i);
                            setDecodeChannel(i,decodeJson,decodeID,channelList);
                            sSignal.resetDecodeComplete(decodeID);
                        }else
                            sSignal.removeDecode(decodeID,0);
                    }else{
                        if(sConfig.selectDecodeID===decodeID)
                            decodeTableModel.stopAll(false);
                        controller.setRecodeJSON(decodeID,decodeJson);
                    }
                    break;
                }
            }
        }

        function onRemoveDecode(decodeID,state_){
            if(state_===1){
                var targetItem=dataModel.get(dataModel.count-1);
                for(var i=0;i<dataModel.count;i++){
                    var item=dataModel.get(i);
                    if(item["decode"]["isDecode"]){
                        if(item["decode"]["decodeID"]===decodeID){
                            for(var j=0;j<item["channelsData"].count;j++)
                                sSignal.setChannelDisable(item["channelsData"].get(j)["data"]["channelID"],false);
                            dataModel.remove(i);
                            break;
                        }
                    }
                }
                controller.removeDecode(decodeID);
            }
        }

        function onGetChannelY(channelID_2,type,y){
            if(type===3)
            {
                getSelectChannel(channelID_2,y);
                return;
            }
            if(typeof(channelID_2)==="string"){
                for(let i=0;i<dataModel.count;i++){
                    let lv1=list_view.itemAtIndex(i);
                    let lv1data=dataModel.get(i);
                    if(lv1data["decode"]["decodeID"]===channelID_2)
                    {
                        sSignal.sendChannelY(channelID_2,type,lv1.y-listviewParentItem.contentY+5)
                        return;
                    }
                }
            }else{
                if(channelID_2>=0)
                {
                    for(let i=0;i<list_view.model.count;i++){
                        let lv1=list_view.itemAtIndex(i);
                        for(let j=0;j<lv1.list.model.count;j++){
                            let lv2=lv1.list.list.itemAtIndex(j);
                            let lv2item=lv1.list.model.get(j);
                            let top=lv1.y+lv2.y+lv1.list.y-listviewParentItem.contentY+5;
                            if(lv2item.channelID===channelID_2){
                                if(type===2)
                                    sSignal.sendChannelY(channelID_2,type,top)
                                else if(type===1){
                                    if(top>100)
                                        sSignal.sendChannelY(channelID_2,type,top-80)
                                    else
                                        sSignal.sendChannelY(channelID_2,type,top+lv2.height)
                                }
                                else
                                    sSignal.sendChannelY(channelID_2,type,top+lv2.height)
                                return;
                            }
                        }
                    }
                }
            }
        }
    }

    function getSelectChannel(channelID_2,y){
        let array=[];
        for(let i=0;i<list_view.model.count;i++){
            let lv1=list_view.itemAtIndex(i);
            let lv1data=dataModel.get(i);
            if(lv1data["decode"]["decodeID"]===channelID_2)
                y+=lv1.y-listviewParentItem.contentY+5;
            for(let j=0;j<lv1.list.list.model.count;j++){
                let lv2=lv1.list.list.itemAtIndex(j);
                let lv2item=lv1.list.list.model.get(j);
                let temp={"channelID":lv2item.channelID,"top":lv1.y+lv2.y+lv1.list.y-listviewParentItem.contentY+5};
                temp["bottom"]=temp["top"]+lv2.height;
                if(lv2item.channelID===channelID_2)
                    y+=temp["top"];
                array.push(temp);
            }
        }
        for(let j in array){
            if(array[j]["top"]<=y&&array[j]["bottom"]>=y)
            {
                sSignal.sendChannelY(channelID_2,3,array[j]["channelID"])
                break;
            }
        }
    }

    function setDecodeChannel(insertIndex,decodeJson,decodeID,channelList){
        let nameArray=[];
        for(let js in decodeJson){
            if(decodeJson[js]["channels"])
            {
                for(let js2 in decodeJson[js]["channels"]){
                    if(decodeJson[js]["channels"][js2]["value"]!=="-")
                        nameArray.push({"name":decodeJson[js]["channels"][js2]["name"],"channelID":decodeJson[js]["channels"][js2]["value"]});
                }
            }

            if(decodeJson[js]["opt_channels"])
            {
                for(let js3 in decodeJson[js]["opt_channels"]){
                    if(decodeJson[js]["opt_channels"][js3]["value"]!=="-")
                        nameArray.push({"name":decodeJson[js]["opt_channels"][js3]["name"],"channelID":decodeJson[js]["opt_channels"][js3]["value"]});
                }
            }
        }

        let heightCount=[];
        let jsonArray=[{"data":{"channelID": -1,"channelName":"NULL","isDisable":false,
                               "channelVisible":true, "decodeChannelName": "",
                               "channelHeight_":1,"minChannelHeight":1}}];
        let showText=decodeJson["main"]["first"];
        if(decodeJson["main"]["second"] && decodeJson["main"]["second"].length>0)
            showText=decodeJson["main"]["second"][decodeJson["main"]["second"].length-1];
        var decode={"isDecode":true,"name":showText,"decodeID":decodeID,"colorIndex":decodeJson["main"]["colorIndex"]};
        for(let i in channelList){
            for(let j=0;j<parentItem.list.model.count;j++){
                let lv2=parentItem.list.list.itemAtIndex(j);
                let channelData=parentItem.list.model.get(j);
                if(channelData["channelID"]===parseInt(channelList[i])){
                    let decodeChannelName="";
                    for(let l in nameArray){
                        if(parseInt(nameArray[l]["channelID"])===channelData["channelID"]){
                            decodeChannelName=nameArray[l]["name"];
                            break;
                        }
                    }
                    jsonArray.push({"data":{"channelID": channelData["channelID"],"channelName": channelData["channelName"],
                                           "channelVisible": channelData["channelVisible"], "isDisable": false, "decodeChannelName": decodeChannelName,
                                           "channelHeight_": channelData["channelHeight_"], "minChannelHeight": channelData["minChannelHeight"]}})
                    if(channelData["channelVisible"])
                        heightCount.push({"data":channelData["channelID"]});
                    sSignal.setChannelDisable(channelData["channelID"],true);
                }
            }
        }
        dataModel.insert(0,{"channelsData": jsonArray,"initHeight":heightCount,"isExpand":true,"decode":decode});
    }

    function setContentY(){
        if(listviewParentItem.contentHeight<main.height)
            vbar.position=0;
        var temp=vbar.position*((listviewParentItem.contentHeight-main.height>0?listviewParentItem.contentHeight-main.height:0)/(1-vbar.size));
        if(contentYRecode!==-1)
        {
            if(contentYRecode>listviewParentItem.contentHeight)
                contentYRecode=listviewParentItem.contentHeight;
            temp=contentYRecode;
        }
        if(isNaN(temp))
            temp=0;
        if(listviewParentItem.contentY!==temp)
            listviewParentItem.contentY=temp;
        sSignal.measureRefreshY();
    }
}




