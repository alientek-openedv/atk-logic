import QtQuick 2.15
import QtQuick.Controls 2.5
import "../config"

Rectangle {
    property alias image: rootImage
    id: rootControl
    width: parent.width
    height: parent.height
    color: "#00000000"
    onVisibleChanged: {
        if(visible)
            testFileTableModel.refresh();
    }

    Connections{
        target: Signal
        function onEscKeyPress()
        {
            rootControl.visible=false
        }
    }

    MouseArea{
        anchors.fill: parent
        preventStealing: true
        propagateComposedEvents: false
        hoverEnabled: true
        acceptedButtons: Qt.AllButtons
        onWheel: wheel.acctped=true
    }
    Image {
        id: rootImage
        width: 436
        height: 406
        source: "../../resource/image/TestFileSelectPopupDropShadow.png"
        Rectangle{
            width: 420
            height: 390
            radius: 8
            color: Config.backgroundColor
            anchors{
                top: parent.top
                topMargin: 6
                horizontalCenter: parent.horizontalCenter
            }
            Item{
                width: parent.width
                height: 40
                Text {
                    text: qsTr("测试文档")
                    color: Config.textColor
                    anchors{
                        verticalCenter: parent.verticalCenter
                        left: parent.left
                        leftMargin: 20
                    }
                    font.pixelSize: 18
                }
                MouseArea{
                    property point clickPos: "0,0"
                    anchors.fill: parent
                    acceptedButtons: Qt.LeftButton
                    preventStealing: true
                    onPositionChanged: {
                        var delta = Qt.point(mouse.x-clickPos.x, mouse.y-clickPos.y)
                        if(rootImage.x+delta.x<rootImage.parent.width-rootImage.width)
                            rootImage.x=Math.max(rootImage.x+delta.x,0);
                        else
                            rootImage.x=rootImage.parent.width-rootImage.width;
                        if(rootImage.y+delta.y<rootImage.parent.height-rootImage.height)
                            rootImage.y=Math.max(rootImage.y+delta.y,30);
                        else
                            rootImage.y=rootImage.parent.height-rootImage.height;
                    }
                    onPressed: clickPos=Qt.point(mouse.x,mouse.y)
                }
            }
            ImageButton{
                width: 10
                height: 10
                imageSource: "resource/icon/MessageClose.png"
                imageEnterSource: "resource/icon/MessageClose.png"
                anchors{
                    right: parent.right
                    rightMargin: 10
                    top: parent.top
                    topMargin: 15
                }
                onPressed: rootControl.visible=false
            }
            Rectangle{
                id: splitRectangle
                width: parent.width
                height: 1
                anchors{
                    top: parent.top
                    topMargin: 40
                }
                color: Config.lineColor
            }
            TableView {
                property int rowsHeight: 162
                property int columnsWidth: 130
                id: dataListView
                height: parent.height-61
                width: parent.width-40
                anchors{
                    top: splitRectangle.bottom
                    topMargin: 20
                    left: parent.left
                    leftMargin: 20
                }
                clip: true
                contentWidth: width
                flickableDirection: Flickable.AutoFlickDirection
                boundsBehavior: Flickable.StopAtBounds
                ScrollBar.vertical: vbar
                columnWidthProvider: function(column){return columnsWidth;}
                rowHeightProvider: function(row){return rowsHeight;}
                model: testFileTableModel
                delegate: Item {
                    width: dataListView.columnsWidth;
                    height: dataListView.rowsHeight
                    visible: fileName!==""&&filePath!==""
                    clip: true
                    Item{
                        width: 120
                        height: 120
                        ImageButton{
                            id: imageButton
                            width: parent.width
                            height: parent.height
                            imageWidth: 46
                            imageHeight: 55
                            imageSource: "resource/icon/TestFile.png"
                            imageEnterSource: imageSource
                            backgroundColor: Config.backgroundColor
                            backgroundMouseEnterColor: Config.mouseEnterColor
                            backgroundPressedColor: backgroundMouseEnterColor
                            onPressed: {
                                rootControl.visible=false;
                                if(sConfig.setPath!=="")
                                    controller.deleteSetDirectory(sConfig.setPath);
                                demoShowData=true;
                                sSignal.removeAllDecode();
                                sSignal.measureRemoveAll();
                                vernierListModel.removeAll(true);
                                sConfig.setPath="";
                                controller.openFile(filePath);
                            }
                        }
                        Text {
                            text: fileName
                            font.pixelSize: 14
                            color: Config.subheadColor
                            elide: Qt.ElideRight
                            width: noShowFileName.contentWidth>parent.width?parent.width:noShowFileName.contentWidth
                            height: 12
                            anchors{
                                top: imageButton.bottom
                                topMargin: 7
                                horizontalCenter: parent.horizontalCenter
                            }
                        }
                        Text {
                            id: noShowFileName
                            text: fileName
                            font.pixelSize: 14
                            visible: false
                        }
                    }
                }
            }
            QScrollBar {
                id: vbar
                height: parent.height-41
                anchors{
                    top: splitRectangle.bottom
                    right: parent.right
                }
            }
        }
    }
    function refReshPosition(){
        if(rootControl.parent.width>0&&rootControl.parent.height>0&&rootControl.width>0){
            if(rootImage.x+rootImage.width>rootControl.parent.width)
                rootImage.x=rootControl.parent.width-rootImage.width;
            if(rootImage.y+rootImage.height>rootControl.parent.height)
                rootImage.y=rootControl.parent.height-rootImage.height;
        }
        if(rootImage.x<0)
            rootImage.x=0;
        if(rootImage.y<30)
            rootImage.y=30;
    }
}
