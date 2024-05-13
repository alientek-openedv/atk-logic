import QtQuick 2.11
import QtQuick.Controls 2.5
import "../config"

ListView {
    property int rowsHeight: 28
    property int changedExtendIndex
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
            property var datacolor_: dataColor
            onDatacolor_Changed: canvas.requestPaint()
            width: listView.contentWidth
            height: visible?rowsHeight:0
            visible: visible_
            color: currentIndex===index?Config.mouseEnterColor:"transparent"
            Column{
                spacing: 0
                width: parent.width
                height: parent.height
                Rectangle{
                    property bool isContainsMouse: rowMouseArea.containsMouse||deleteButton.containsMouse||moreButton.containsMouse
                    id: parentRectangle
                    width: parent.width
                    height: rowsHeight
                    color: (parentRectangle.isContainsMouse&&listView.selectIndex===index)?Config.mouseEnterColor:"transparent"
                    onIsContainsMouseChanged: {
                        if(isContainsMouse)
                            listView.selectIndex=index;
                        else
                            listView.selectIndex=-1;
                    }
                    ImageButton{
                        id: deleteButton
                        visible: parentRectangle.isContainsMouse&&listView.selectIndex===index
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
                        onPressed: {
                            sSignal.vernierCreate(2);
                            vernierListModel.remove(vernierID);
                        }
                    }
                    ImageButton{
                        id: moreButton
                        visible: parentRectangle.isContainsMouse
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
                        data: [{"showText":qsTr("跳到该处"),"shortcutKey":"","seleteType":0,"buttonIndex":0},
                            {"showText":"-","shortcutKey":"","seleteType":0,"buttonIndex":-1},
                            {"showText":qsTr("删除"),"shortcutKey":"","seleteType":0,"buttonIndex":1}]
                        onSelect: {
                            switch(index){
                            case 0:
                                controller.showViewScope(position,position,!Setting.jumpZoom);
                                break;
                            case 1:
                                sSignal.vernierCreate(2);
                                vernierListModel.remove(vernierID);
                                break;
                            }
                        }
                    }
                    MouseArea{
                        id: rowMouseArea
                        anchors.fill: parent
                        hoverEnabled: true
                        onDoubleClicked: controller.showViewScope(position,position,!Setting.jumpZoom);
                    }
                    Row{
                        spacing: 0
                        anchors.verticalCenter: parent.verticalCenter
                        Item{
                            width: 16
                            height: 16
                        }
                        Item{
                            width: 10
                            height: parent.height
                            Canvas{
                                property int drawHeight: 10
                                id: canvas
                                width: 6
                                height: drawHeight
                                z: 10
                                anchors.verticalCenter: parent.verticalCenter
                                onPaint: {
                                    var ctx = getContext("2d");
                                    ctx.lineWidth = 0
                                    ctx.strokeStyle = dataColor
                                    ctx.fillStyle = dataColor
                                    ctx.beginPath();
                                    ctx.moveTo(0, 0)
                                    ctx.lineTo(canvas.width, 0);
                                    ctx.lineTo(canvas.width, canvas.height-3);
                                    ctx.lineTo(canvas.width/2, canvas.height);
                                    ctx.lineTo(0, canvas.height-3);
                                    ctx.closePath();
                                    ctx.fill()
                                    ctx.stroke();
                                }
                            }
                        }
                        Text {
                            text: name+" = "+positionStr
                            color: Config.textColor
                            anchors.verticalCenter: parent.verticalCenter
                        }
                    }
                }
            }
        }
    }
}
