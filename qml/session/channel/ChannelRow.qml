import QtQuick 2.15
import QtQuick.Controls 2.5
import QtGraphicalEffects 1.0
import atk.qml.Controls 1.0
import "../../config"
import "../../style"
import "../"

Component{
    Item{
        property alias list: list_item
        property point pointRecode:"-1,-1"
        id: list_item_root
        width: parent?parent.width-10:0
        height: channelRowRectangle.height+5
        Rectangle{
            id: channelRowRectangle
            clip: true
            width: parent.width
            height: channelRowColumn.height
            color: Config.pageColor
            radius: 20
            anchors{
                left: parent.left
                leftMargin: 5
                top: parent.top
                topMargin: 5
            }
            Column {
                id: channelRowColumn
                width: parent.width
                height: list_item.height+(header.sourceComponent?header.height:0)
                spacing: 0
                Component.onCompleted: {
                    if(decode["isDecode"])
                        header.sourceComponent=headerComponent;
                }
                Loader{
                    id: header
                    height: 62
                    width: parent.width
                    visible: decode["isDecode"]
                }
                Component{
                    id: headerComponent
                    Item {
                        width: parent.width
                        height: parent.height
                        clip: true
                        Item{
                            id: headerSideBar
                            width: sConfig.isExpandSidebar?Config.headerWidth:Config.closeHeaderWidth
                            height: parent.height-1
                            Behavior on width {
                                NumberAnimation {
                                    duration: 100
                                }
                            }
                            clip: true
                            Item{
                                width: parent.width-18
                                height: 38
                                clip: true
                                anchors{
                                    verticalCenter: parent.verticalCenter
                                    left: parent.left
                                    margins: 10
                                }

                                ParallelAnimation {
                                    id: backAnim
                                    NumberAnimation { id: backAnimX; target: list_item_root; property: "x"; to: 0; duration: 100; easing.type: Easing.OutCubic}
                                    NumberAnimation { id: backAnimY; target: list_item_root; property: "y"; duration: 100; easing.type: Easing.OutCubic}
                                    onFinished: {
                                        list_item_root.z=1;
                                        var count=list_view.contentY,i=0;
                                        for(i=0;i<dataModel.count;i++)
                                        {
                                            //纠正移位的通道
                                            if(list_view.itemAtIndex(i).y!==count){
                                                list_view.itemAtIndex(i).y=count;
                                            }
                                            count+=list_view.itemAtIndex(i).height;
                                        }
                                    }
                                }

                                Rectangle{
                                    anchors.fill: parent
                                    radius: 10
                                    color: Config.channelDataColor[decode["colorIndex"]%16]
                                    MouseArea {
                                        id: mouse_area
                                        anchors.fill: parent
                                        drag.target: list_item_root
                                        cursorShape: Qt.OpenHandCursor
                                        //防止被view窃取事件
                                        preventStealing: true
                                        propagateComposedEvents: true

                                        onPressed: {
                                            backAnim.stop();
                                            Signal.setCursor(Qt.ClosedHandCursor);
                                            if(list_item_root.pointRecode.x===-1)
                                                list_item_root.pointRecode=Qt.point(0, list_item_root.y);
                                            list_item_root.z=1000
                                        }
                                        onReleased: {
                                            Signal.setCursor(Qt.ArrowCursor);
                                            list_item_root.z=1;
                                            var count=list_view.contentY,i=0;
                                            for(i=0;i<dataModel.count;i++)
                                            {
                                                if(list_view.itemAtIndex(i).y!==count){
                                                    list_view.itemAtIndex(i).y=count;
                                                }
                                                list_view.itemAtIndex(i).pointRecode=Qt.point(0, count);
                                                count+=list_view.itemAtIndex(i).height;
                                            }
                                            backAnimX.from = list_item_root.x;
                                            backAnimY.from = list_item_root.y;
                                            backAnimY.to = list_item_root.pointRecode.y;
                                            backAnim.start()
                                        }

                                        onPositionChanged: {
                                            var targetY;
                                            var item;
                                            var i=0,count=0;
                                            var isSet=false;
                                            var itemY=list_item_root.y-list_view.contentY;
                                            //0 item是定位item，不能移动
                                            if(index>0){
                                                item=list_view.itemAtIndex(index-1);
                                                targetY=item.y+item.height/2-list_view.contentY;
                                                if(itemY+mouse_area.mouseY<targetY){
                                                    for(i=0;i<index-1;i++)
                                                        count+=list_view.itemAtIndex(i).height;
                                                    count+=list_view.contentY;
                                                    list_item_root.pointRecode=Qt.point(0, count);
                                                    dataModel.move(index, index-1, 1);
                                                    isSet=true;
                                                }
                                            }
                                            if(index<dataModel.count-2 && !isSet){
                                                item=list_view.itemAtIndex(index+1);
                                                targetY=item.y+item.height/2-list_view.contentY;
                                                //                console.log(itemY+","+mouse_area.mouseY+","+targetY+","+(itemY+mouse_area.mouseY>targetY)+","+
                                                //                            list_view.y+","+list_view.contentY);
                                                if(itemY+mouse_area.mouseY>targetY){
                                                    for(i=0;i<index+1;i++)
                                                    {
                                                        if(i===index)
                                                            count+=list_view.itemAtIndex(index+1).height;
                                                        else
                                                            count+=list_view.itemAtIndex(i).height;
                                                    }
                                                    count+=list_view.contentY;
                                                    list_item_root.pointRecode=Qt.point(0, count);
                                                    dataModel.move(index, index+1, 1);
                                                }
                                            }
                                        }
                                    }
                                }

                                Item{
                                    width: parent.width-38-foldImageButton.width-(setButton.visible?setButton.width:0)
                                    height: parent.height
                                    clip: true
                                    anchors{
                                        left: parent.left
                                        leftMargin: 8
                                    }
                                    Text {
                                        text: decode["name"]
                                        color: "white"
                                        elide: Qt.ElideRight
                                        font{
                                            pixelSize: 19
                                            bold: true
                                        }
                                        width: contentWidthText.contentWidth<parent.width?contentWidthText.contentWidth:parent.width
                                        height: contentHeight
                                        anchors.centerIn: parent
                                    }
                                    Text {
                                        id: contentWidthText
                                        text: decode["name"]
                                        font{
                                            pixelSize: 19
                                            bold: true
                                        }
                                        visible: false
                                    }
                                }

                                ImageButton{
                                    id: setButton
                                    anchors{
                                        verticalCenter: parent.verticalCenter
                                        right: foldImageButton.left
                                        rightMargin: 10
                                    }
                                    visible: sConfig.isExpandSidebar
                                    width: 14
                                    height: 14
                                    enabled: sConfig.loopState===-1 && !sConfig.isRun
                                    imageSource: "resource/icon/SetWhite.png"
                                    imageEnterSource: imageSource
                                    imageDisableSource: imageSource
                                    onPressed: {
                                        for(var i in sSettings.decodeJson){
                                            if(sSettings.decodeJson[i]["decodeID"]===decode["decodeID"]){
                                                switch(sSettings.decodeJson[i]["decodeJson"]["main"]["height"]){
                                                case 2:
                                                    decodeSetMenuPopup.selectIndex=12;
                                                    break;
                                                case 4:
                                                    decodeSetMenuPopup.selectIndex=13;
                                                    break;
                                                case 8:
                                                    decodeSetMenuPopup.selectIndex=14;
                                                    break;
                                                default:
                                                    decodeSetMenuPopup.selectIndex=11;
                                                }
                                                break;
                                            }
                                        }
                                        decodeSetMenuPopup.lastCurrentIndex=-1;
                                        decodeSetMenuPopup.open()
                                    }
                                    onContainsMouseChanged: Signal.setCursor(containsMouse?Qt.PointingHandCursor:Qt.ArrowCursor);
                                }
                                QMenuPopup{
                                    property int selectIndex: 11
                                    id: decodeSetMenuPopup
                                    parent: setButton
                                    isTwoModel: true
                                    x: 0
                                    data: [{"showText":qsTr("编辑"),"shortcutKey":"","seleteType":0,"buttonIndex":0},
                                        {"showText":qsTr("显示行"),"shortcutKey":"->","seleteType":0,"buttonIndex":1},
                                        {"showText":qsTr("删除"),"shortcutKey":"","seleteType":0,"buttonIndex":2},
                                        {"showText":"-","shortcutKey":"","seleteType":0,"buttonIndex":-1},
                                        {"showText":qsTr("1X协议高度"),"shortcutKey":"","seleteType":selectIndex==11?2:1,"buttonIndex":11},
                                        {"showText":qsTr("2X协议高度"),"shortcutKey":"","seleteType":selectIndex==12?2:1,"buttonIndex":12},
                                        {"showText":qsTr("4X协议高度"),"shortcutKey":"","seleteType":selectIndex==13?2:1,"buttonIndex":13},
                                        {"showText":qsTr("8X协议高度"),"shortcutKey":"","seleteType":selectIndex==14?2:1,"buttonIndex":14}]
                                    onSelect: {
                                        var json;
                                        for(var i in sSettings.decodeJson){
                                            if(sSettings.decodeJson[i]["decodeID"]===decode["decodeID"]){
                                                json=sSettings.decodeJson[i]["decodeJson"];
                                                break;
                                            }
                                        }
                                        let decodeName=json["main"]["first"];
                                        if(json["main"]["second"] && json["main"]["second"].length>0)
                                            decodeName=json["main"]["second"][json["main"]["second"].length-1];
                                        switch(index){
                                        case 0:
                                            sSignal.editDecode(decode["decodeID"]);
                                            break;
                                        case 1:
                                            decodeSetShowRowPopup.decodeJson=json;
                                            decodeSetShowRowPopup.decodeName=decodeName;
                                            decodeSetShowRowPopup.open();
                                            break;
                                        case 2:
                                            removeDecodeTimer.decodeName=decode["decodeID"];
                                            removeDecodeTimer.start();
                                            break;
                                        case 11:
                                            selectIndex=11;
                                            json["main"]["height"]=1;
                                            Signal.setDecodeConfig(decodeName, "height", 1);
                                            sSignal.resetDecodeJson(json, json["main"]["decodeID"]);
                                            break;
                                        case 12:
                                            selectIndex=12;
                                            json["main"]["height"]=2;
                                            Signal.setDecodeConfig(decodeName, "height", 2);
                                            sSignal.resetDecodeJson(json, json["main"]["decodeID"]);
                                            break;
                                        case 13:
                                            selectIndex=13;
                                            json["main"]["height"]=4;
                                            Signal.setDecodeConfig(decodeName, "height", 4);
                                            sSignal.resetDecodeJson(json, json["main"]["decodeID"]);
                                            break;
                                        case 14:
                                            selectIndex=14;
                                            json["main"]["height"]=8;
                                            Signal.setDecodeConfig(decodeName, "height", 8);
                                            sSignal.resetDecodeJson(json, json["main"]["decodeID"]);
                                            break;
                                        }
                                    }
                                    onContainsTwo: {
                                        if(containsMouse){
                                            for(var i in sSettings.decodeJson){
                                                if(sSettings.decodeJson[i]["decodeID"]===decode["decodeID"]){
                                                    var json=sSettings.decodeJson[i]["decodeJson"];
                                                    let decodeName=json["main"]["first"];
                                                    if(json["main"]["second"] && json["main"]["second"].length>0)
                                                        decodeName=json["main"]["second"][json["main"]["second"].length-1];
                                                    decodeSetShowRowPopup.decodeJson=json;
                                                    decodeSetShowRowPopup.decodeName=decodeName;
                                                    decodeSetShowRowPopup.open();
                                                }
                                            }
                                        }else
                                            decodeSetShowRowPopup.close();
                                    }
                                }

                                DecodeSetShowRowPopup{
                                    id: decodeSetShowRowPopup
                                    parent: decodeSetMenuPopup.parent
                                    x: decodeSetMenuPopup.x+decodeSetMenuPopup.width
                                    y: decodeSetMenuPopup.y+35
                                }

                                ImageButton{
                                    id: foldImageButton
                                    anchors{
                                        verticalCenter: parent.verticalCenter
                                        right: parent.right
                                        rightMargin: sConfig.isExpandSidebar?15:13
                                    }
                                    width: 12
                                    height: 12
                                    imageWidth: 6
                                    imageHeight: 12
                                    imageSource: "resource/icon/Fold.png"
                                    imageEnterSource: "resource/icon/Fold.png"
                                    rotation: isExpand?90:0
                                    onPressed: {
                                        isExpand=!isExpand
                                        sSignal.refreshChannelHeight();
                                    }
                                    Behavior on rotation{
                                        NumberAnimation { duration: 100 }
                                    }

                                    onContainsMouseChanged: Signal.setCursor(containsMouse?Qt.PointingHandCursor:Qt.ArrowCursor);
                                }
                            }
                        }

                        Connections{
                            target: sSignal

                            function onCloseAllPopup(){
                                decodeSetMenuPopup.close();
                            }

                            function onChannelRefresh(channelID_)
                            {
                                if(channelID_<0&&!sConfig.isExit)
                                    drawHeaderChannel.update();
                            }

                            function onVernierCreate(state){
                                if(state===1)
                                    drawHeaderChannel.vernierCreateModel=true;
                                else if (state===2)
                                    drawHeaderChannel.vernierCreateModel=false;
                            }

                            function onSendChannelY(channelID_,type,y){
                                if(typeof(channelID_)==="string" && decode["decodeID"]===channelID_ && type===3)
                                    drawHeaderChannel.adsorbChannelID=y;
                            }

                            function onCrossChannelMeasureState(isStop){
                                drawHeaderChannel.adsorbChannelID=-1;
                                drawHeaderChannel.crossChannelMeasureStateModel(isStop);
                            }
                        }

                        Connections{
                            target: Config
                            function onTpChanged()
                            {
                                drawHeaderChannel.setTheme(Config.tp);
                            }
                        }

                        DrawChannel{
                            id: drawHeaderChannel
                            anchors{
                                left: headerSideBar.right
                                top: parent.top
                            }
                            width: parent.width-headerSideBar.width
                            height: headerSideBar.height
                            showDataColor: Config.textColor
                            isMouseMeasure: Setting.isMouseMeasure
                            Component.onCompleted: {
                                drawHeaderChannel.decodeInit(sessionID_, decode["decodeID"])
                                setTheme(Config.tp);
                            }
                            onHeightChanged: header.height=height+1;
                            onHoverLeave: {
                                showCursor=false;
                            }
                            onShowCursorChanged: {
                                if(showCursor)
                                    sSignal.setChannelCursor(Qt.SizeHorCursor);
                                else
                                    sSignal.setChannelCursor(Qt.ArrowCursor);
                            }
                            onCloseVernierPopup: sSignal.closeVernierPopup();
                            onMeasureSelectChanged: {
                                sSignal.measureRefreshShow();
                                sSignal.channelRefresh(-1);
                            }
                            onVernierDataChanged: {
                                sSignal.vernierDataChanged(vernierID);
                                vernierListModel.refresh(vernierID);
                            }
                            onShowViewScope: controller.showViewScope(start,end,true);
                            onMouseZoom: {
                                sSignal.showMouseZoom(show,x,y,width,height);
                                if(show)
                                    sSignal.getChannelY(decode["decodeID"], 2, 0);
                            }
                            onDeleteVernier: vernierListModel.remove(vernierID);
                            onVernierCreateComplete: sSignal.vernierCreate(2);
                            onGetAdsorbChannelID: sSignal.getChannelY(decode["decodeID"], 3, y);
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
                            onVernierAppend: {
                                vernierListModel.append();
                                sSignal.channelRefresh(-1);
                            }
                            onCrossChannelMeasureState: sSignal.crossChannelMeasureState(isStop);
                            onSendCrossChannelMeasurePosition: {
                                sSignal.sendCrossChannelMeasurePosition(type, x, y, mouseY, position, isHit);
                                sSignal.getChannelY(decode["decodeID"], 3+type, 0);
                            }
                            onStopXWheel: sSignal.stopXWheel();
                        }
                        Rectangle{
                            visible: isExpand
                            anchors{
                                bottom: parent.bottom
                            }
                            clip: true
                            height: 1
                            width: parent.width-70
                            anchors.right: parent.right
                            color: Config.background2Color
                        }
                    }
                }
                ChannelsViewNest {
                    id: list_item
                    width: main.width
                    Component.onCompleted: {
                        var i;
                        for(i=0;i<channelsData.count;i++){
                            model.append(channelsData.get(i)["data"]);
                        }
                        for(i=0;i<initHeight.count;i++){
                            showList.push(initHeight.get(i)["data"])
                        }
                        refReshHeight();
                    }
                }
            }
        }
    }
}


