import QtQuick 2.11
import QtQuick.Controls 2.5
import "../config"

ListView {
    property int rowsHeight: 28
    property int changedExtendIndex
    property int splitWidth: 100
    property int selectIndex: -1

    id: listView
    headerPositioning: ListView.OverlayHeader
    flickableDirection: Flickable.AutoFlickDirection
    boundsBehavior: Flickable.StopAtBounds
    ScrollBar.vertical: vbar
    ScrollBar.horizontal: hbar
    delegate: listDelegate
    contentHeight: height
    contentWidth: width
    height: parent.height
    width: parent.width
    clip: true
    currentIndex: -1
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
        Rectangle{
            id: mainR
            property bool isExtend: false
            property var dataList: [startText,endText,time,rising,falling,minFreq,maxFreq]
            property var dataNameList: ["start","end","time","rising","falling","minFreq","maxFreq"]
            property bool isContainsMouse: rowMouseArea.containsMouse||extendButton.containsMouse||deleteButton.containsMouse||moreButton.containsMouse
            width: listView.contentWidth
            height: isExtend?rowsHeight+20*(mainR.dataNameList.length):rowsHeight
            color: currentIndex===index?Config.mouseEnterColor:"transparent"

            onIsContainsMouseChanged: {
                if(isContainsMouse)
                    listView.selectIndex=index;
                else
                    listView.selectIndex=-1;
            }

            Rectangle{
                width: parent.width
                height: parent.height
                color: (mainR.isContainsMouse&&listView.selectIndex===index)||isSelect?Config.mouseEnterColor:"transparent"
                MouseArea{
                    id: rowMouseArea
                    anchors.fill: parent
                    hoverEnabled: true
                    onPressed: selectMeasure(measureID,channelID);
                    onDoubleClicked: measureFollow(start,end,measureID,channelID);
                }
                Column{
                    spacing: 0
                    width: parent.width
                    height: parent.height
                    Item{
                        width: parent.width
                        height: rowsHeight
                        ImageButton{
                            id: deleteButton
                            visible: moreButton.visible
                            imageSource: "resource/icon/"+Config.tp+"/Delete.png"
                            imageEnterSource: "resource/icon/DeleteEnter.png"
                            imageWidth: 9
                            imageHeight: 11
                            width: 16
                            height: 16
                            z: 5
                            anchors{
                                verticalCenter: parent.verticalCenter
                                right: moreButton.left
                                rightMargin: 2
                            }
                            onPressed: sSignal.measureRemove(measureID);
                        }
                        ImageButton{
                            id: moreButton
                            visible: mainR.isContainsMouse&&listView.selectIndex===index
                            imageWidth: 2
                            imageHeight: 12
                            imageSource: "resource/icon/"+Config.tp+"/More.png"
                            imageEnterSource: "resource/icon/"+Config.tp+"/MoreEnter.png"
                            width: 12
                            height: 16
                            z: 5
                            anchors{
                                verticalCenter: parent.verticalCenter
                                verticalCenterOffset: -1
                                right: parent.right
                                rightMargin: vbar.visible?13:6
                            }
                            onPressed: menuPopup.visible=true
                        }
                        Connections{
                            target: sSignal
                            function onCloseAllPopup(){
                                menuPopup.close();
                            }
                        }
                        QMenuPopup{
                            id: menuPopup
                            parent: moreButton
                            showShortcutKey: false
                            isShowTop: true
                            data: [{"showText":qsTr("跳到该处"),"shortcutKey":"","seleteType":0,"buttonIndex":0},
                            {"showText":"-","shortcutKey":"","seleteType":0,"buttonIndex":-1},
                            {"showText":qsTr("删除"),"shortcutKey":"","seleteType":0,"buttonIndex":1}]
                            onSelect: {
                                switch(index){
                                case 0:
                                    measureFollow(start,end,measureID,channelID);
                                    break;
                                case 1:
                                    sSignal.measureRemove(measureID);
                                    break;
                                }
                            }
                        }
                        Row{
                            spacing: 0
                            anchors.verticalCenter: parent.verticalCenter
                            Item{
                                width: 16
                                height: 16
                                anchors.verticalCenter: parent.verticalCenter
                                ImageButton{
                                    id: extendButton
                                    anchors.centerIn: parent
                                    imageWidth: 4
                                    imageHeight: 6
                                    imageSource: "resource/icon/"+Config.tp+"/Extend.png"
                                    imageEnterSource: imageSource
                                    rotation: mainR.isExtend?90:0
                                    width: 12
                                    height: 12
                                    onPressed: {
                                        mainR.isExtend=!mainR.isExtend
                                        changedExtendIndex=index
                                    }
                                    Behavior on rotation{
                                        NumberAnimation {
                                            duration: 100
                                        }
                                    }
                                }
                            }
                            Item{
                                width: 10
                                height: parent.height
                                Rectangle{
                                    width: 6
                                    height: 6
                                    color: dataColor
                                    anchors.verticalCenter: parent.verticalCenter
                                }
                            }
                            Row{
                                id: leftItem
                                width: splitWidth-27
                                height: parent.height
                                spacing: 5
                                Text {
                                    id: titleText
                                    width: parent.width-vernierTextImage-vernierTextImage.anchors.leftMargin
                                    text: name
                                    elide: Qt.ElideRight
                                    font.pixelSize: 12
                                    color: Config.textColor
                                    anchors.verticalCenter: parent.verticalCenter
                                }
                                Image {
                                    id: vernierTextImage
                                    width: 10
                                    height: 9
                                    fillMode: Image.PreserveAspectFit
                                    anchors.verticalCenter: parent.verticalCenter
                                    source: "../../resource/icon/"+Config.tp+"/VernierTextImage.png"
                                }
                            }
                            Row{
                                height: parent.height
                                spacing: 2
                                Image {
                                    width: 9
                                    height: 9
                                    fillMode: Image.PreserveAspectFit
                                    anchors{
                                        verticalCenter: parent.verticalCenter
                                        verticalCenterOffset: 1
                                    }
                                    source: "../../resource/icon/"+Config.tp+"/delte.png"
                                }
                                Text {
                                    text: time
                                    elide: Qt.ElideRight
                                    font.pixelSize: 12
                                    color: Config.textColor
                                    anchors.verticalCenter: parent.verticalCenter
                                }
                            }
                        }
                    }

                    Repeater{
                        model: mainR.dataNameList.length
                        Row{
                            visible: mainR.isExtend
                            width: parent.width
                            height: 20
                            spacing: 0
                            Item{
                                width: 26
                                height: parent.height
                            }
                            Text {
                                width: splitWidth-26-2
                                text: mainR.dataNameList[index]
                                elide: Qt.ElideRight
                                font.pixelSize: 12
                                color: Config.textColor
                            }
                            Text {
                                text: mainR.dataList[index]
                                elide: Qt.ElideRight
                                font.pixelSize: 12
                                color: Config.textColor
                            }
                        }
                    }
                }
            }
        }
    }

    function measureFollow(start,end,measureID,channelID){
        controller.showViewScope(start,end,!Setting.jumpZoom);
        selectMeasure(measureID,channelID);
    }

    function selectMeasure(measureID,channelID){
        measureTreeViewModel.setSelect(measureID);
        sSignal.channelRefresh(channelID);
        sSignal.measureRefreshShow();
    }
}
