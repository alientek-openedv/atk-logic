import QtQuick 2.15
import QtQuick.Controls 2.5
import "../../config"
import "../../style"

Rectangle {
    property int minHeight: 70
    id: measureMain
    color: Config.background2Color
    height: parent.height

    onHeightChanged: {
        if(measureTreeView.height<minHeight)
        {
            let tmp=minHeight-measureTreeView.height;
            vernierView.height-=tmp;
        }
        if(vernierView.height<minHeight){
            let tmp=minHeight-vernierView.height;
            distanceView.height-=tmp;
        }
    }

    Connections{
        target: sSignal

        function onCloseAllPopup(){
            menuPopup.close();
        }
    }

    Column{
        id: topColumn
        spacing: 20
        anchors{
            top: parent.top
            topMargin: 14
            left: parent.left
            leftMargin: 14
            right: parent.right
            rightMargin: 14
        }

        Text {
            text: qsTr("标签测量")
            font.pixelSize: 18
            color: Config.mouseCheckColor
        }

        Item{
            width: parent.width
            height: mouseMeasureTitle.contentHeight
            Text {
                id: mouseMeasureTitle
                text: qsTr("标签距离")
                font.pixelSize: 16
                color: Config.textColor
            }
            ImageButton{
                id: distanceAdd
                anchors{
                    right: parent.right
                    verticalCenter: parent.verticalCenter
                }
                enabled: moreButton.enabled
                imageWidth: 8
                imageHeight: 8
                imageSource: "resource/icon/Add.png"
                imageEnterSource: "resource/icon/"+Config.tp+"/AddEnter.png"
                imageDisableSource: "resource/icon/"+Config.tp+"/AddDisable.png"
                width: 16
                height: 16
                onPressed: distanceView.model.append({});
            }
        }
    }

    QDistanceView{
        id: distanceView
        anchors{
            top: topColumn.bottom
            topMargin: 5
        }
        height: 110
        width: parent.width
    }

    Rectangle{
        id: distanceSplit
        height: 3
        width: parent.width
        anchors.top: distanceView.bottom
        color: "transparent"
        Rectangle{
            height: 1
            width: parent.width
            anchors.verticalCenter: parent.verticalCenter
            color: Config.lineColor
        }
        MouseArea{
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton
            cursorShape: Qt.SizeVerCursor
            hoverEnabled: true
            preventStealing: true
            onMouseYChanged: {
                if(pressed)
                {
                    let len=mouseY,count=0;
                    if(distanceView.height+len<minHeight){
                        len+=distanceView.height-minHeight;
                        count+=distanceView.height-minHeight;
                        distanceView.height=minHeight;
                        vernierView.height+=count;
                    }
                    else if(vernierView.height-len<minHeight)
                    {
                        len-=vernierView.height-minHeight;
                        count+=vernierView.height-minHeight;
                        vernierView.height=minHeight;
                        if(measureTreeView.height-len<minHeight)
                            count+=measureTreeView.height-minHeight;
                        else
                            count+=len;
                        distanceView.height+=count;
                    }
                    else{
                        distanceView.height+=len;
                        vernierView.height-=len;
                    }
                }
            }
        }
    }

    Column{
        id: vernierColumn
        spacing: 20
        anchors{
            top: distanceSplit.top
            topMargin: 20
            left: parent.left
            leftMargin: 14
            right: parent.right
            rightMargin: 14
        }
        Item{
            width: parent.width
            height: timeTitle.contentHeight
            Text {
                id: timeTitle
                text: qsTr("时间标签")
                font.pixelSize: 16
                color: Config.textColor
            }

            ImageButton{
                id: moreButton
                imageWidth: 2
                imageHeight: 12
                imageSource: "resource/icon/"+Config.tp+"/More.png"
                imageEnterSource: "resource/icon/"+Config.tp+"/MoreEnter.png"
                imageDisableSource: "resource/icon/"+Config.tp+"/MoreDisable.png"
                width: 10
                height: 16
                enabled: sConfig.loopState===-1 && !sConfig.isRun
                anchors{
                    right: parent.right
                    verticalCenter: parent.verticalCenter
                }
                onPressed: menuPopup.open();
            }
            QMenuPopup{
                id: menuPopup
                parent: moreButton
                showShortcutKey: true
                data: [{"showText":qsTr("添加标签"),"shortcutKey":Setting.vernierCreate,"seleteType":sConfig.isChannelMethodRun?-1:0,"buttonIndex":1},
                    {"showText":qsTr("删除全部"),"shortcutKey":"","seleteType":0,"buttonIndex":2}]
                onSelect: {
                    switch(index){
                    case 1:
                        sSignal.vernierCreate(0);
                        break;
                    case 2:
                        sSignal.vernierCreate(2);
                        vernierListModel.removeAll(false);
                        break;
                    }
                }
            }
        }
    }

    QVernierView{
        id: vernierView
        anchors{
            top: vernierColumn.bottom
            topMargin: 5
        }
        height: 110
        width: parent.width
        model: vernierListModel
        onHeightChanged: {
            if(height<minHeight){
                let tmp=minHeight-height;
                height=minHeight;
                distanceView.height-=tmp;
            }
        }
    }
    Rectangle{
        id: vernierSplit
        height: 3
        width: parent.width
        anchors.top: vernierView.bottom
        color: "transparent"
        Rectangle{
            height: 1
            width: parent.width
            anchors.verticalCenter: parent.verticalCenter
            color: Config.lineColor
        }
        MouseArea{
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton
            cursorShape: Qt.SizeVerCursor
            hoverEnabled: true
            preventStealing: true
            onMouseYChanged: {
                if(pressed)
                {
                    let len=mouseY,count=0;
                    if(vernierView.height+len<minHeight){
                        len+=vernierView.height-minHeight;
                        count+=vernierView.height-minHeight;
                        vernierView.height=minHeight;
                        if(distanceView.height+len<minHeight){
                            len+=distanceView.height-minHeight;
                            count+=distanceView.height-minHeight;
                            distanceView.height=minHeight;
                        }else{
                            count+=Math.abs(len);
                            distanceView.height-=count;
                        }
                    }
                    else if(measureTreeView.height-len<minHeight)
                    {
                        var temp=measureTreeView.height-minHeight;
                        vernierView.height+=temp;
                    }
                    else
                        vernierView.height+=len;
                }
            }
        }
    }

    Column{
        width: parent.width
        height: parent.height-topColumn.height-distanceView.height-distanceSplit.height-vernierColumn.height-vernierView.height-vernierSplit.height
        spacing: 10
        anchors{
            top: vernierSplit.bottom
            topMargin: 20
        }
        Item{
            id: measureTitleItem
            width: parent.width-25
            height: measureTitle.contentHeight
            anchors{
                left: parent.left
                leftMargin: 10
            }
            Text {
                id: measureTitle
                text: qsTr("参数测量")
                font.pixelSize: 16
                color: Config.textColor
            }
            ImageButton{
                id: measureChannelAdd
                anchors{
                    right: parent.right
                    verticalCenter: parent.verticalCenter
                }
                enabled: moreButton.enabled
                imageWidth: 8
                imageHeight: 8
                imageSource: "resource/icon/Add.png"
                imageEnterSource: "resource/icon/"+Config.tp+"/AddEnter.png"
                imageDisableSource: "resource/icon/"+Config.tp+"/AddDisable.png"
                width: 16
                height: 16
                onPressed: measureMenuPopup.visible=true
            }

            QMenuPopup{
                id: measureMenuPopup
                parent: measureChannelAdd
                showShortcutKey: true
                data: [{"showText":qsTr("添加通道标记"),"shortcutKey":Setting.parameterMeasure,"seleteType":sConfig.isChannelMethodRun?-1:0,"buttonIndex":0},
                    {"showText":qsTr("删除全部"),"shortcutKey":"","seleteType":0,"buttonIndex":1}]
                onSelect: {
                    switch(index){
                    case 0:
                        if(!sConfig.isRun && !sConfig.isChannelMethodRun)
                            sSignal.measureStart();
                        break;
                    case 1:
                        sSignal.measureEnd();
                        sSignal.measureRemoveAll();
                    }
                }
            }
        }
        QMeasureTreeView{
            id: measureTreeView
            height: parent.height-measureTitleItem.height-65
            width: parent.width
            model: measureTreeViewModel
        }

    }
}
