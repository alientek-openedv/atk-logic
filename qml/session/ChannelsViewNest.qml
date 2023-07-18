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
import "./channel"
import "../config"
import "../style"

Item{
    property alias model: dataModelNest
    property var showList: []
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
                        dataModelNest.get(i)["isDisable"]=isDisable;
                        var index=-1;
                        for(var j=0;j<sConfig.availableChannels.length;j++){
                            if(sConfig.availableChannels[j]===channelID){
                                index=j;
                                break;
                            }
                        }
                        if(isDisable){
                            if(index!==-1)
                                sConfig.availableChannels.splice(index,1);
                        }
                        else if(index===-1)
                            sConfig.availableChannels.push(channelID);
                        sConfig.availableChannels.sort((x,y)=> x - y);
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
}




