import QtQuick 2.15
import QtQuick.Controls 2.5

import "./channel"
import "../config"
import "../style"

Item{
    property alias model: dataModelNest
    property var showList: []//和heightList长度必须一样
    property alias list: list_view_nest
    id: mainNest

    Behavior on height {
        NumberAnimation {
            duration: 100
        }
    }

    onHeightChanged: {
        refreshChannelTimer.stop()
        refreshChannelTimer.start();
    }

    Timer{
        id: refreshChannelTimer
        interval: 100
        onTriggered: sSignal.channelRefresh(-1);
    }

    ListView{
        id: list_view_nest
        reuseItems: true
        width: parent.width
        height: parent.height
        interactive: false
        clip: true
        flickableDirection: Flickable.AutoFlickDirection
        boundsBehavior: Flickable.StopAtBounds
        ScrollBar.vertical: ScrollBar{
            policy: ScrollBar.AlwaysOff
        }
        moveDisplaced: Transition {
            YAnimator { duration: 100 }
        }
        model: ListModel {
            id: dataModelNest
        }
        delegate: ChannelRowNest{}
    }

    Connections{
        target: sSignal

        function onSetChannelDisable(channelID, isDisable){
            if(typeof(decode)!="undefined"&&!decode["isDecode"]&&channelID>=0){
                for(var i=0;i<dataModelNest.count;i++){
                    if(dataModelNest.get(i)["channelID"]===channelID){
                        var index=checkUseChannels(channelID);
                        if(isDisable){
                            sConfig.useChannels.push(channelID);
                            dataModelNest.get(i)["isDisable"]=isDisable;
                        }
                        else if(index!==-1){
                            sConfig.useChannels.splice(index,1);
                            if(checkUseChannels(channelID)===-1)
                                dataModelNest.get(i)["isDisable"]=isDisable;
                        }
                        sConfig.useChannels.sort((x,y)=> x - y);
                        sSignal.refreshChannelHeight();
                        refReshHeight();
                        break;
                    }
                }
            }
        }

        function onRefreshChannelHeight(){
            refReshHeight();
        }
    }

    Timer{
        id: refReshHeightTimer
        interval: 100
        onTriggered: {
            if(sConfig.isExit)
                return;
            if(Config.initHeightState===0){
                Config.initHeightState=1;
                Signal.showInitText(1, qsTr("创建会话"));
            }
            var count=0;
            for(var i=0;i<dataModelNest.count;i++){
                var item=dataModelNest.get(i);
                for(var ii in showList){
                    if(item.channelID===showList[ii]){
                        if(typeof(decode)!="undefined"&&decode["isDecode"]){
                            if(item.channelVisible&&isExpand)
                                count+=item.channelHeight_;
                        }else if(item.channelVisible&&isExpand&&!item.isDisable)
                            count+=item.channelHeight_;
                        break;
                    }
                }
            }
            height=count;
            visible=height>0;
            if(Config.initHeightState===1){
                Config.initHeightState=2;
                Signal.showInitText(0, "");
            }
            sSignal.refreshEdgeButton();
        }
    }

    function refReshHeight(){
        if(!refReshHeightTimer.running)
            refReshHeightTimer.start();
    }

    function checkUseChannels(channelID){
        for(var j=0;j<sConfig.useChannels.length;j++){
            if(sConfig.useChannels[j]===channelID)
                return j;
        }
        return -1;
    }
}




