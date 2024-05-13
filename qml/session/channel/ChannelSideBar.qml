import QtQuick 2.15
import QtQuick.Controls 2.5
import "../../config"
import "../../style"

Rectangle {
    property color channelDataColor
    property alias dragActive: mouse_area.drag.active
    color: "transparent"
    clip: true

    Component.onCompleted: {
        if(channelID>=0)
        {
            sSignal.channelNameSignalChanged(channelID, controller.getChannelName(channelID));
        }
    }

    Connections{
        target: Signal

        function onFocusOut(){
            mouse_area.onReleasedFunction();
        }
    }

    Connections{
        target: sSignal

        function onChannelNameSignalChanged(channelID_, channelName_){
            if(channelNameEdit.channelIDRecode===channelID_)
            {
                if(channelName!==channelName_)
                    channelName=channelName_;
                if(channelNameEdit.text!==channelName_)
                    channelNameEdit.text=channelName_;
                controller.channelNameChanged(channelID_,channelName_);
            }
        }

        function onRefreshEdgeButton(){
            if(channelID>=0){
                risingEdgeButton.isChecked=sSettings.channelsSet[channelID].triggerType===Config.ChannelTriggerButton.RisingEdge;
                highLevelButton.isChecked=sSettings.channelsSet[channelID].triggerType===Config.ChannelTriggerButton.HighLevel;
                fallingEdgeButton.isChecked=sSettings.channelsSet[channelID].triggerType===Config.ChannelTriggerButton.FallingEdge;
                lowLevelButton.isChecked=sSettings.channelsSet[channelID].triggerType===Config.ChannelTriggerButton.LowLevel;
                doubleEdgeButton.isChecked=sSettings.channelsSet[channelID].triggerType===Config.ChannelTriggerButton.DoubleEdge;
            }
        }
    }

    MouseArea {
        id: mouse_area
        z:1
        anchors.fill: parent
        drag.target: list_item_nest
        cursorShape: Qt.OpenHandCursor
        enabled: channelID>=0
        //防止被view窃取事件
        preventStealing: true
        propagateComposedEvents: true
        onPressed: {
            backAnim.stop();
            Signal.setCursor(Qt.ClosedHandCursor);
            if(list_item_nest.pointRecode.x===-1)
                list_item_nest.pointRecode=Qt.point(0, list_item_nest.y);
            list_item_nest.z=1000
        }
        onReleased: onReleasedFunction();

        function onReleasedFunction(){
            Signal.setCursor(Qt.ArrowCursor);
            list_item_nest.z=500;
            var count=list_view_nest.contentY,i=0;
            for(i=0;i<dataModelNest.count;i++)
            {
                if(list_view_nest.itemAtIndex(i)===null)
                    return;
                if(list_view_nest.itemAtIndex(i).y!==count){
                    list_view_nest.itemAtIndex(i).y=count;
                }
                list_view_nest.itemAtIndex(i).pointRecode=Qt.point(0, count);
                count+=list_view_nest.itemAtIndex(i).height;
            }
            backAnimX.from = list_item_nest.x;
            backAnimY.from = list_item_nest.y;
            backAnimY.to = list_item_nest.pointRecode.y;
            backAnim.start()
            sSignal.sendChannelY(channelID,0,list_item_nest.pointRecode.y+list_item_nest.height-listviewParentItem.contentY+5)
        }

        onPositionChanged: {
            var targetY;
            var item;
            var i=0,count=0;
            var isSet=false;
            var showIndex=0;
            var showList=[];
            for(i=0;i<dataModelNest.count;i++)
            {
                var modelItem=dataModelNest.get(i);
                if(modelItem.channelVisible)
                    showList.push(i);
            }
            for(i=0;i<showList.length;i++){
                if(index===showList[i]){
                    showIndex=i;
                    break;
                }
            }
            var itemY=list_item_nest.y-list_view_nest.contentY;
            //0 item是定位item，不能移动
            if(showIndex>1){
                item=list_view_nest.itemAtIndex(showList[showIndex-1]);
                targetY=item.y+item.height/2-list_view_nest.contentY;
                if(itemY+mouse_area.mouseY<targetY){
                    for(i=0;i<showList[showIndex-1];i++)
                        count+=list_view_nest.itemAtIndex(i).height;
                    count+=list_view_nest.contentY;
                    list_item_nest.pointRecode=Qt.point(0, count);
                    dataModelNest.move(index, showList[showIndex-1], 1);
                    isSet=true;
                }
            }
            if(showIndex!==showList.length-1 && !isSet){
                item=list_view_nest.itemAtIndex(showList[showIndex+1]);
                targetY=item.y+item.height/2-list_view_nest.contentY;
                //                console.log(itemY+","+mouse_area.mouseY+","+targetY+","+(itemY+mouse_area.mouseY>targetY)+","+
                //                            list_view_nest.y+","+list_view_nest.contentY);
                if(itemY+mouse_area.mouseY>targetY){
                    for(i=0;i<showList[showIndex+1];i++)
                    {
                        if(i===index)
                            count+=list_view_nest.itemAtIndex(showList[showIndex+1]).height;
                        else
                            count+=list_view_nest.itemAtIndex(i).height;
                    }
                    count+=list_view_nest.contentY;
                    list_item_nest.pointRecode=Qt.point(0, count);
                    dataModelNest.move(index, showList[showIndex+1], 1);
                }
            }
            list_item_nest.x=0;//锁定不能x移动
            sSignal.sendChannelY(channelID,0,list_item_nest.y+list_item_nest.height-listviewParentItem.contentY+5)
        }
    }


    Rectangle{
        anchors{
            left: parent.left
            right: parent.right
            verticalCenter: parent.verticalCenter
            margins: 10
        }
        color: Config.pageColor
        radius: 10
        height: 36
        z:100
        Rectangle{
            id: idRectangle
            width: parent.height
            height: parent.height
            color: channelDataColor
            radius: 10
            Text {
                id: channelIDText
                anchors.centerIn: parent
                font.pixelSize: 13
                text: "D"+channelID
                color: "white"
            }
        }
        Column{
            visible: sConfig.isExpandSidebar
            spacing: 3
            anchors{
                left: idRectangle.right
                leftMargin: 20
                right: parent.right
                verticalCenter: parent.verticalCenter
            }
            Item{
                width: parent.width
                height: channelNameEdit.height
                TextInput {
                    property int channelIDRecode: channelID
                    id: channelNameEdit
                    width: contentWidth>parent.width?(parent.width<0?0:parent.width):contentWidth<80?80:contentWidth
                    font.pixelSize: 16
                    visible: !isDisable
                    clip: true
                    verticalAlignment: Qt.AlignVCenter
                    selectByMouse: true
                    selectionColor: Config.mouseCheckColor
                    renderType: Text.NativeRendering
                    antialiasing: false
                    color: Config.textColor
                    Keys.enabled: true
                    Keys.onPressed: {
                        if(event.key===Qt.Key_Return || event.key===Qt.Key_Enter)
                            mouse_area.focus=true
                        else if(event.key===Qt.Key_Escape){
                            text=channelName
                            mouse_area.focus=true
                        }
                    }
                    onActiveFocusChanged: {
                        if(!activeFocus)
                            sSignal.channelNameSignalChanged(channelID, text)
                    }

                    MouseArea{
                        anchors.fill: parent
                        acceptedButtons: Qt.NoButton
                        hoverEnabled: true
                        onEntered: Signal.setCursor(Qt.IBeamCursor);
                        onExited: Signal.setCursor(Qt.ArrowCursor);
                    }
                }
                Row{
                    visible: channelNameEdit.activeFocus
                    anchors{
                        top: channelNameEdit.bottom
                        left: channelNameEdit.left
                    }
                    width: channelNameEdit.width+5
                    spacing: 2
                    height:1
                    Repeater{
                        width: parent.width
                        height:1
                        model: parent.width/5
                        delegate:Rectangle{
                            width: 3
                            height: 1
                            color: channelNameEdit.color
                        }
                    }
                }
            }

            Row {
                id: radioColumn
                height: 20
                spacing: 3
                z: 100
                ImageButton{
                    id: risingEdgeButton
                    width: 20
                    height: 17
                    checkable: true
                    autoChecked: false
                    isChecked: sSettings.channelsSet[channelID<0?0:channelID].triggerType===Config.ChannelTriggerButton.RisingEdge
                    imageSource: "resource/icon/"+Config.tp+"/RisingEdge.png"
                    imageEnterSource: imageSource
                    imagePressedSource: "resource/icon/RisingEdgeEnter.png"
                    backgroundColor: Config.pageColor
                    backgroundPressedColor: Config.mouseCheckColor
                    backgroundMouseEnterColor: backgroundColor
                    backgroundRectangle.border{
                        width: containsMouse?1:0
                        color: Config.subheadColor
                    }
                    onPressed: setRadioButton(risingEdgeButton,Config.ChannelTriggerButton.RisingEdge)
                }
                ImageButton{
                    id: highLevelButton
                    width: 20
                    height: 17
                    checkable: true
                    autoChecked: false
                    isChecked: sSettings.channelsSet[channelID<0?0:channelID].triggerType===Config.ChannelTriggerButton.HighLevel
                    imageSource: "resource/icon/"+Config.tp+"/HighLevel.png"
                    imageEnterSource: imageSource
                    imagePressedSource: "resource/icon/HighLevelEnter.png"
                    backgroundColor: Config.pageColor
                    backgroundPressedColor: Config.mouseCheckColor
                    backgroundMouseEnterColor: backgroundColor
                    backgroundRectangle.border{
                        width: containsMouse?1:0
                        color: Config.subheadColor
                    }
                    onPressed: setRadioButton(highLevelButton,Config.ChannelTriggerButton.HighLevel)
                }
                ImageButton{
                    id: fallingEdgeButton
                    width: 20
                    height: 17
                    checkable: true
                    autoChecked: false
                    isChecked: sSettings.channelsSet[channelID<0?0:channelID].triggerType===Config.ChannelTriggerButton.FallingEdge
                    imageSource: "resource/icon/"+Config.tp+"/FallingEdge.png"
                    imageEnterSource: imageSource
                    imagePressedSource: "resource/icon/FallingEdgeEnter.png"
                    backgroundColor: Config.pageColor
                    backgroundPressedColor: Config.mouseCheckColor
                    backgroundMouseEnterColor: backgroundColor
                    backgroundRectangle.border{
                        width: containsMouse?1:0
                        color: Config.subheadColor
                    }
                    onPressed: setRadioButton(fallingEdgeButton,Config.ChannelTriggerButton.FallingEdge)
                }
                ImageButton{
                    id: lowLevelButton
                    width: 20
                    height: 17
                    checkable: true
                    autoChecked: false
                    isChecked: sSettings.channelsSet[channelID<0?0:channelID].triggerType===Config.ChannelTriggerButton.LowLevel
                    imageSource: "resource/icon/"+Config.tp+"/LowLevel.png"
                    imageEnterSource: imageSource
                    imagePressedSource: "resource/icon/LowLevelEnter.png"
                    backgroundColor: Config.pageColor
                    backgroundPressedColor: Config.mouseCheckColor
                    backgroundMouseEnterColor: backgroundColor
                    backgroundRectangle.border{
                        width: containsMouse?1:0
                        color: Config.subheadColor
                    }
                    onPressed: setRadioButton(lowLevelButton,Config.ChannelTriggerButton.LowLevel)
                }
                ImageButton{
                    id: doubleEdgeButton
                    width: 20
                    height: 17
                    checkable: true
                    autoChecked: false
                    isChecked: sSettings.channelsSet[channelID<0?0:channelID].triggerType===Config.ChannelTriggerButton.DoubleEdge
                    imageSource: "resource/icon/"+Config.tp+"/DoubleEdge.png"
                    imageEnterSource: imageSource
                    imagePressedSource: "resource/icon/DoubleEdgeEnter.png"
                    backgroundColor: Config.pageColor
                    backgroundPressedColor: Config.mouseCheckColor
                    backgroundMouseEnterColor: backgroundColor
                    backgroundRectangle.border{
                        width: containsMouse?1:0
                        color: Config.subheadColor
                    }
                    onPressed: {
                        setRadioButton(doubleEdgeButton,Config.ChannelTriggerButton.DoubleEdge)
                    }
                }
            }
        }
    }

    function setRadioButton(button_,type_){
        if(sSettings.channelsSet[channelID].triggerType===type_ && button_.isChecked)
            sSettings.channelsSet[channelID].triggerType=Config.ChannelTriggerButton.Null;
        else
            sSettings.channelsSet[channelID].triggerType=type_;
        sSignal.refreshEdgeButton();
    }
}

