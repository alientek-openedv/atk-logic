import QtQuick 2.15
import QtQuick.Controls 2.5
import atk.qml.Controls 1.0
import "../../config"

Component{
    Item {
        property point pointRecode:"-1,-1"
        id: list_item_nest
        Drag.active: channelSideBar.dragActive
        width: mainNest.width
        height: decode["isDecode"]?channelVisible&&isExpand?channelHeight_:0:channelVisible&&isExpand&&!isDisable?channelHeight_:0
        onVisibleChanged: refReshChannel(channelID);
        visible: channelVisible
        clip: true
        ParallelAnimation {
            id: backAnim
            NumberAnimation { id: backAnimX; target: list_item_nest; property: "x"; to: 0; duration: 100; easing.type: Easing.OutCubic }
            NumberAnimation { id: backAnimY; target: list_item_nest; property: "y"; duration: 100; easing.type: Easing.OutCubic }
            onFinished: {
                list_item_nest.z=1;
                var count=list_view_nest.contentY,i=0;
                for(i=0;i<dataModelNest.count;i++)
                {
                    //纠正移位的通道
                    if(list_view_nest.itemAtIndex(i).y!==count){
                        list_view_nest.itemAtIndex(i).y=count;
                    }
                    count+=list_view_nest.itemAtIndex(i).height;
                }
            }
        }

        ChannelSideBar{
            id: channelSideBar
            anchors{
                left: parent.left
                top: parent.top
            }
            width: sConfig.isExpandSidebar?Config.headerWidth:Config.closeHeaderWidth
            height: parent.height-1
            channelDataColor: sSettings.channelsDataColor[channelID<0?0:channelID]
            Behavior on width {
                NumberAnimation {
                    duration: 100
                }
            }
        }

        Component.onCompleted: {
            if(channelID>=0)
            {
                if(decodeChannelName==="")
                    channelHeight_=sSettings.channelsHeight[channelID];
                refReshHeight();
                refReshChannel(channelID);
            }
        }

        Connections{
            target: sSignal
            function onChannelRefresh(channelID_)
            {
                if(!sConfig.isExit){
                    if(channelID_<0 || channelID===channelID_)
                    {
                        drawChannel.update();
                        refReshChannel(channelID_);
                    }
                }
            }

            function onMeasureStart(){
                if(channelID>=0)
                {
                    sConfig.isMeasureStart=true;
                    if(drawChannel.containsMouse)
                        sSignal.setChannelCursor(Qt.SizeHorCursor);
                    drawChannel.measureState=1;
                }
            }

            function onMeasureEnd(){
                if(channelID>=0)
                {
                    sSignal.setChannelCursor(Qt.ArrowCursor);
                    drawChannel.measureState=-1;
                    sConfig.isMeasureStart=false;
                }
            }

            function onChannelColorChanged(){
                if(channelID>=0)
                {
                    channelSideBar.channelDataColor=sSettings.channelsDataColor[channelID<0?0:channelID];
                    drawChannel.showDataColor=sSettings.channelsDataColor[channelID<0?0:channelID];
                    drawChannel.update();
                }
            }

            function onVernierCreate(state){
                if(channelID>=0)
                {
                    if(state===1)
                        drawChannel.vernierCreateModel=true;
                    else if (state===2)
                        drawChannel.vernierCreateModel=false;
                }
            }

            function onSendChannelY(channelID_,type,y){
                if(typeof(channelID)==="number")
                    if(channelID===channelID_ && type===3 && channelID_>=0)
                        drawChannel.adsorbChannelID=y;
            }

            function onChannelHeightChanged(channelID_, cHeight){
                if(channelID===channelID_ && channelHeight_!==cHeight && !moveMouseArea.pressed)
                    channelHeight_=cHeight;
            }

            function onVernierCancelMove(){
                drawChannel.vernierCancelMove();
            }

            function onCrossChannelMeasureState(isStop){
                sConfig.isCrossChannelMeasure=!isStop;
                drawChannel.crossChannelMeasureStateModel(isStop);
            }
        }

        Connections{
            target: Config
            function onTpChanged()
            {
                if(channelID>=0)
                    drawChannel.setTheme(Config.tp);
            }
        }

        Connections{
            target: sSettings
            function onChannelHeightMultipleChanged()
            {
                if(channelID>=0)
                {
                    channelHeight_=sSettings.channelHeightMultiple*33+29;
                    sSettings.channelsHeight[channelID]=channelHeight_;
                    refReshHeight();
                }
            }
        }

        MouseArea{
            id: moveMouseArea
            height: 6
            width: drawRectangle.width
            anchors{
                bottom: parent.bottom
                right: drawRectangle.right
            }
            preventStealing: true
            propagateComposedEvents: true
            acceptedButtons: Qt.AllButtons
            enabled: channelID>=0 && list_item_nest.height>0
            hoverEnabled: true
            onMouseYChanged: {
                if(pressed)
                {
                    contentYRecode=listviewParentItem.contentY;
                    var office=mouseY-height+1;
                    if(channelHeight_+office>=minChannelHeight && channelHeight_+office<=800)
                        channelHeight_+=office;
                    else if(channelHeight_+office<minChannelHeight)
                        channelHeight_=minChannelHeight;
                    else if(channelHeight_+office>800)
                        channelHeight_=800;
                    sSettings.channelsHeight[channelID]=channelHeight_;
                    sSignal.channelHeightChanged(channelID, channelHeight_);
                    refReshHeight();
                    Signal.setCursor(Qt.SplitVCursor)
                }
            }
            onPressedChanged: {
                if(!pressed)
                    contentYRecode=-1;
            }
            onReleased: Signal.setCursor(Qt.ArrowCursor)
            onEntered: Signal.setCursor(Qt.SplitVCursor)
            onExited: Signal.setCursor(Qt.ArrowCursor)
        }

        Rectangle{
            id: drawRectangle
            anchors{
                bottom: parent.bottom
                left: parent.left
                leftMargin: 60
            }
            visible: !list_item_nest.Drag.active && channelID>=0
            width: channelSideBar.width-anchors.leftMargin
            height: 1
            color: Config.background2Color
            clip: true
        }

        Rectangle{
            anchors{
                bottom: parent.bottom
                left: drawRectangle.right
            }
            visible: !list_item_nest.Drag.active && channelID>=0
            width: parent.width-channelSideBar.width
            height: 1
            color: Config.background2Color
        }

        DrawChannel{
            property bool containsMouse: false
            id: drawChannel
            anchors{
                right: parent.right
                top: parent.top
            }
            width: mainNest.width-channelSideBar.width
            height: parent.height
            showDataColor: sSettings.channelsDataColor[channelID<0?0:channelID]
            isRunCollect: sConfig.isRun
            decodeChannelDesc: decodeChannelName
            isExit: sConfig.isExit
            isMouseMeasure: Setting.isMouseMeasure
            Component.onCompleted: {
                if(channelID>=0)
                {
                    drawChannel.init(sessionID_, channelID);
                    setTheme(Config.tp);
                }
            }
            onHoverEnter: {
                if(sConfig.isMeasureStart)
                    sSignal.setChannelCursor(Qt.SizeHorCursor);
                containsMouse=true;
            }
            onHoverLeave: {
                showCursor=false;
                sSignal.setChannelCursor(Qt.ArrowCursor);
                sSignal.closeMouseMeasurePopup();
                containsMouse=false;
            }
            onCloseMouseMeasurePopup:{
                sSignal.closeMouseMeasurePopup();
            }
            onShowMouseMeasurePopup: {
                sSignal.showMouseMeasurePopup(x_,channelID,period,freq,duty)
                sSignal.getChannelY(channelID, 1, 0);
            }

            onShowCursorChanged: {
                if(showCursor)
                    sSignal.setChannelCursor(Qt.SizeHorCursor);
                else if(!sConfig.isMeasureStart)
                    sSignal.setChannelCursor(Qt.ArrowCursor);
            }
            onMeasureStateChanged: {
                if(measureState===3){
                    measureTreeViewModel.append();
                    sSignal.measurePopupAppend(getLastMeasureID());
                    sSignal.measureRefreshShow();
                    sSignal.channelRefresh(-1);
                }else if(measureState===0)
                    sSignal.measureEnd();
            }
            onCloseVernierPopup: sSignal.closeVernierPopup();
            onMeasureSelectChanged: {
                sSignal.measureRefreshShow();
                sSignal.channelRefresh(-1);
            }
            onMeasureDataChanged: {
                sSignal.measureRefreshData(measureID);
                measureTreeViewModel.refresh(measureID);
            }
            onVernierDataChanged: {
                sSignal.vernierDataChanged(vernierID);
                vernierListModel.refresh(vernierID);
            }

            onShowViewScope: controller.showViewScope(start,end,true);

            onMouseZoom: {
                sSignal.showMouseZoom(show,x,y,width,height);
                if(show)
                    sSignal.getChannelY(channelID, 2, 0);
            }

            onDeleteVernier: vernierListModel.remove(vernierID);

            onVernierCreateComplete: sSignal.vernierCreate(2);

            onGetAdsorbChannelID: sSignal.getChannelY(channelID, 3, y);

            onVernierMoveState: {
                if(isMove)
                    sConfig.isVernierMove=true;
                else{
                    sConfig.isVernierMove=false;
                    sSignal.vernierMoveComplete(id);
                }
            }

            onMeasureMoveState: {
                if(isMove)
                    sConfig.isMeasureMove=true;
                else
                    sConfig.isMeasureMove=false;
            }

            onSetLiveFollowing: {
                if(sConfig.isRun && !sConfig.isBuffer && sConfig.isLiveFollowingPopupShow)
                    sSignal.setLiveFollowing(isEnable,true);
            }

            onStopXWheel: sSignal.stopXWheel();

            onVernierAppend: {
                vernierListModel.append();
                sSignal.channelRefresh(-1);
            }

            onCrossChannelMeasureState: sSignal.crossChannelMeasureState(isStop);

            onSendCrossChannelMeasurePosition: {
                sSignal.sendCrossChannelMeasurePosition(type, x, y, mouseY, position, isHit);
                sSignal.getChannelY(channelID, 3+type, 0);
            }
        }

        function refReshChannel(channelID_){
            if(typeof(channelID)!=="undefined" && channelID>=0){
                if(channelID!==channelID_)
                    channelVisible=sSettings.channelsSet[channelID].enable;
                for(var j in sSettings.channelsSet){
                    if(channelID_===-1 || parseInt(j)===channelID){
                        if(sSettings.channelsSet[j].enable)
                        {
                            showList.push(parseInt(j))
                            showList = [...new Set(showList)];
                        }else{
                            for(var i in showList){
                                if(showList[i]===parseInt(j)){
                                    showList.splice(i,1);
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            if(channelID_===channelID){
                refReshHeight();
            }
        }
    }
}


