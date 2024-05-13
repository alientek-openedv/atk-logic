import QtQuick 2.15
import QtQuick.Controls 2.5
import "../config"

Item{
    property alias view: dataListView
    property var headerList: model.isQmlModel?model.getHeaderList():Object.keys(model.get(0)).reverse()
    property var columnsWidth: []
    property int headerHeight: 28
    property int rowsHeight: 24
    property int minColumnsWidth: 30
    property color headerTextColor: Config.textColor
    property color headerBackgroundColor: Config.background2Color
    property int headerTextSize: 14
    property color backgroundColor: Config.background2Color
    property color alternateBackgroundColor: Config.backgroundColor
    property color textColor: Config.textColor
    property int textSize: 12
    property alias selectRow: dataListView.selectIndex

    signal columnWidthChanged()

    Component.onCompleted: {
        decodeTableModel.setLineHeight(rowsHeight);
    }

    onColumnWidthChanged: {
        for(var i=0;i<repeaterRow.children.length;i++){
            repeaterRow.children[i].width=columnsWidth[i]+(i===0?2:0)
        }
        dataListView.forceLayout();
    }

    onWidthChanged: {
        var widthCount=width;
        var i;
        for(i=0;i<columnsWidth.length;i++){
            widthCount-=columnsWidth[i];
        }
        if(widthCount>=0)
            columnsWidth[columnsWidth.length-1]+=widthCount;
        else{
            for(i=columnsWidth.length-1;i>=0;i--){
                if(columnsWidth[i]+widthCount>=minColumnsWidth){
                    columnsWidth[i]+=widthCount;
                    break;
                }else{
                    widthCount+=columnsWidth[i]-30
                    columnsWidth[i]=30;
                }
            }
        }
        columnWidthChanged();
    }

    height: parent.height
    width: parent.width
    clip: true
    Rectangle{
        id: headerItem
        width: dataListView.contentWidth+1
        height: headerHeight
        x: -1
        z: 15
        color: headerBackgroundColor
        border{
            width: 1
            color: Config.lineColor
        }
        Row{
            id: repeaterRow
            Repeater{
                id:headerRepeater
                model:headerList
                Item{
                    width: columnsWidth[index]+(index===0?2:0)
                    height: headerHeight
                    z: 1000
                    clip: true
                    Text{
                        width: parent.width-7
                        color: headerTextColor
                        text: headerList[index]
                        font.pixelSize: headerTextSize
                        elide:Text.ElideRight
                        anchors{
                            verticalCenter: parent.verticalCenter
                            left: parent.left
                            leftMargin: 5
                        }
                    }
                    Rectangle{
                        visible: index!==columnsWidth.length-1
                        width: 1
                        height: parent.height
                        z: 1000
                        anchors{
                            right: parent.right
                            rightMargin: 1
                        }
                        color: Config.lineColor
                        MouseArea{
                            width: 3
                            height: parent.height
                            acceptedButtons: Qt.LeftButton
                            cursorShape: Qt.SplitHCursor
                            hoverEnabled: true
                            preventStealing: true
                            anchors{
                                verticalCenter: parent.verticalCenter
                                horizontalCenter: parent.horizontalCenter
                            }
                            onMouseXChanged: {
                                if(pressed)
                                {
                                    let count=mouseX;
                                    let currentIndex=index;
                                    while(count!=0){
                                        let countWidth=columnsWidth[currentIndex]+columnsWidth[currentIndex+1];
                                        if(count<0){
                                            if(columnsWidth[currentIndex]+mouseX<minColumnsWidth){
                                                count+=columnsWidth[currentIndex]-minColumnsWidth;
                                                columnsWidth[currentIndex+1]+=columnsWidth[currentIndex]-minColumnsWidth;
                                                columnsWidth[currentIndex]=minColumnsWidth;
                                            }
                                            else{
                                                columnsWidth[currentIndex]+=count;
                                                columnsWidth[currentIndex+1]-=count;
                                                count=0;
                                            }
                                            currentIndex--;
                                            if(currentIndex<0)
                                                break;
                                        }else{
                                            if(columnsWidth[currentIndex+1]-count<minColumnsWidth){
                                                count-=columnsWidth[currentIndex+1]-minColumnsWidth;
                                                columnsWidth[currentIndex]+=columnsWidth[currentIndex+1]-minColumnsWidth;
                                                columnsWidth[currentIndex+1]=minColumnsWidth;
                                            }else{
                                                columnsWidth[currentIndex+1]-=count;
                                                columnsWidth[currentIndex]+=count;
                                                count=0;
                                            }
                                            currentIndex++;
                                            if(currentIndex+1>=columnsWidth.length)
                                                break;
                                        }
                                    }
                                    columnWidthChanged();
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    TableView {
        property int maxIndex: columnsWidth.length-1
        property int focusIndex: -1
        property int selectIndex: -1
        id: dataListView
        height: parent.height-headerHeight
        width: parent.width
        anchors.top: headerItem.bottom
        clip: true
        contentWidth: width
        flickableDirection: Flickable.AutoFlickDirection
        boundsBehavior: Flickable.StopAtBounds
        ScrollBar.vertical: ScrollBar {
            policy: ScrollBar.AlwaysOff
        }
        reuseItems: false
        columnWidthProvider: function(column){return columnsWidth[column];}
        rowHeightProvider: function(row){return rowsHeight;}
        onHeightChanged: refreshScrollBar()
        onContentYChanged: refreshScrollBar()

        delegate: Rectangle {
            property bool isContainsMouse: rowMouseArea.containsMouse||moreButton.containsMouse
            id: parentRectangle
            width: columnsWidth[column];
            height: rowsHeight
            color: dataListView.selectIndex===row?Config.mouseCheckColor:row % 2 == 0 ? backgroundColor:alternateBackgroundColor
            clip: true
            onIsContainsMouseChanged: {
                if(isContainsMouse)
                    dataListView.focusIndex=row;
                else
                    dataListView.focusIndex=-1;
            }

            MouseArea{
                id: rowMouseArea
                anchors.fill: parent
                hoverEnabled: true
                onDoubleClicked: controller.showViewScope(start,end,!Setting.jumpZoom);
                onPressed: dataListView.selectIndex=row;
            }
            Text{
                text: display?display:""
                width: parent.width-5
                elide: Qt.ElideRight
                color: textColor
                anchors{
                    verticalCenter: parent.verticalCenter
                    left: parent.left
                    leftMargin: 5
                }
            }
            ImageButton{
                id: moreButton
                visible: column===dataListView.maxIndex && dataListView.focusIndex===row
                imageSource: "resource/icon/DecodeMore.png"
                imageEnterSource: "resource/icon/DecodeMore.png"
                width: 16
                height: 16
                anchors{
                    verticalCenter: parent.verticalCenter
                    right: parent.right
                    rightMargin: vbar.visible?13:5
                }
                onPressed: {
                    var data=decodeTableModel.get(row);
                    var str=data[0];
                    for(let i=1;i<data["size"];i++)
                        str+=", "+data[i]
                    menuPopup.start=start
                    menuPopup.end=end
                    menuPopup.dataText=data[3]
                    menuPopup.allDataText=str
                    menuPopup.parent=moreButton
                    menuPopup.visible=true
                }
            }
            Rectangle{
                visible: column!==dataListView.maxIndex
                width: 1
                height: parent.height
                anchors{
                    right: parent.right
                    top: parent.top
                }
                color: Config.lineColor
            }
            Rectangle{
                width: parent.width
                height: 1
                anchors{
                    bottom: parent.bottom
                    left: parent.left
                }
                color: Config.lineColor
            }
        }

        onFlickEnded: {
            var t=decodeTableModel.refreshBuffer(dataListView.contentY);
            dataListView.contentY+=t;
            //decodeTableModel.refreshBuffer(dataListView.contentY);
        }
    }

//    Connections{
//        target: sSignal
//        function onCloseAllPopup(){
//            menuPopup.close();
//        }
//    }
    QMenuPopup{
        property var start
        property var end
        property string dataText
        property string allDataText
        id: menuPopup
        showShortcutKey: false
        isShowTop: true
        data: [{"showText":qsTr("跳转"),"shortcutKey":"","seleteType":0,"buttonIndex":1},
            {"showText":"-","shortcutKey":"","seleteType":0,"buttonIndex":-1},
            {"showText":qsTr("复制\"数据\""),"shortcutKey":"","seleteType":0,"buttonIndex":2},
            {"showText":qsTr("复制整行数据"),"shortcutKey":"","seleteType":0,"buttonIndex":3}]
        onSelect: {
            switch(index){
            case 1:
                controller.showViewScope(start,end,!Setting.jumpZoom);
                break;
            case 2:
                clipboard.setText(dataText);
                break;
            case 3:
                clipboard.setText(allDataText);
                break;
            }
        }
    }

    QScrollBar {
        id: vbar
        height: dataListView.height
        dely: 0
        anchors{
            top: dataListView.top
            right: parent.right
        }
        onPositionChanged: {
            if(pressed)
            {
                var t=decodeTableModel.scrollPosition(position, dataListView.height);
                dataListView.contentY=t;
            }
        }
    }

    Connections{
        target: decodeTableModel
        function onModelDataChanged()
        {
            if(!vbar.pressed)
                refreshScrollBar();
        }
    }

    Connections{
        target: sSignal
        function onChannelRefresh(channelID)
        {
            if(!sConfig.isBind || channelID!==-1)
                return;
            var t=decodeTableModel.scrollPosition(decodeTableModel.showData(), dataListView.height);
            if(t>0)
                t+=24;
            dataListView.contentY=t;
        }
    }

    function refreshScrollBar(){
        var count=decodeTableModel.getShowCount();
        vbar.position=(decodeTableModel.getShowFirst()+dataListView.contentY/rowsHeight)/count;
        vbar.size=Math.min(dataListView.height/rowsHeight/count,1);
    }

    function resetScrollBarPosition(){
        vbar.position=0;
        var t=decodeTableModel.scrollPosition(0, dataListView.height);
        dataListView.contentY=t;
    }
}


