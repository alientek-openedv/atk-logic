import QtQuick 2.15
import QtQuick.Controls 2.5
import "../config"

Rectangle{
    id: rootRectangle
    color: "#7d000000"
    z: 1000
    visible: false

    Connections{
        target: Signal
        function onEscKeyPress()
        {
            rootRectangle.visible=false
        }

        function onSendExportPath(path)
        {
            showPathText.text=root.pathRepair_(path);
        }
    }

    onVisibleChanged: {
        if(visible)
        {
            showPathText.text="";
            unitComboBox.currentIndex=0;
            channelListModel.clear();
            channelListModel.append({ "showText": qsTr("所有启用通道"), "cost": -1 });
            for(let i in sSettings.channelsSet){
                if(sSettings.channelsSet[i].enable)
                    channelListModel.append({ "showText": "D"+i+": "+controller.getChannelName(i), "cost": parseInt(i) });
            }
            channelComboBox.currentIndex=0;
        }
    }

    MouseArea{
        anchors.fill: parent
        preventStealing: true
        propagateComposedEvents: false
        hoverEnabled: true
        acceptedButtons: Qt.AllButtons
    }

    Rectangle{
        width: 340
        height: 218+(showPathText.text===""?0:30)
        color: Config.backgroundColor
        radius: 10
        anchors.centerIn: parent
        Item{
            id: titleItem
            width: parent.width
            height: 38
            Text {
                text: qsTr("输出原始数据")
                font.pixelSize: 18
                anchors{
                    left: parent.left
                    leftMargin: 20
                    verticalCenter: parent.verticalCenter
                }
                color: Config.textColor
            }
            ImageButton{
                z: 100
                width: 10
                height: 10
                imageSource: "resource/icon/MessageClose.png"
                imageEnterSource: "resource/icon/MessageClose.png"
                anchors{
                    right: parent.right
                    rightMargin: 10
                    verticalCenter: parent.verticalCenter
                }
                onPressed: rootRectangle.visible=false;
            }
        }

        Rectangle{
            id: titleSplit
            height: 1
            width: parent.width
            anchors.top: titleItem.bottom
            color: Config.lineColor
        }

        Column{
            id: contentColumn
            width: parent.width-40
            spacing: 10
            anchors{
                top: titleSplit.bottom
                topMargin: 10
                bottom: contentSplit.top
                bottomMargin: 10
                left: parent.left
                leftMargin: 20
            }
            Column{
                height: parent.height
                width: parent.width
                spacing: 10

                Row{
                    Text {
                        width: 182
                        text: qsTr("输出单位：")
                        font.pixelSize: 12
                        color: Config.textColor
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    QComboBox{
                        id: unitComboBox
                        width: 120
                        model: ListModel{
                            ListElement { showText: qsTr("采样点"); cost: 0 }
                        }
                    }
                }

                Row{
                    Text {
                        width: 182
                        text: qsTr("输出通道：")
                        font.pixelSize: 12
                        color: Config.textColor
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    QComboBox{
                        id: channelComboBox
                        width: 120
                        model: ListModel{
                            id: channelListModel
                        }
                    }
                }

                Row{
                    Text {
                        width: 182
                        text: qsTr("保存路径：")
                        font.pixelSize: 12
                        color: Config.textColor
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    TextButton{
                        width: 120
                        height: 30
                        backgroundColor: Config.borderBackgroundColor
                        backgroundEnterColor: backgroundColor
                        backgroundPressedColor: backgroundColor
                        text: qsTr("选择路径...")
                        padding: 3
                        textHorizontalAlignment: Text.AlignLeft
                        onClicked: {
                            exportDialog.saveType=Config.SaveType.BinDATA;
                            exportDialog.open();
                        }
                    }
                }

                Text {
                    id: showPathText
                    text: ""
                    width: parent.width
                    font.pixelSize: 12
                    color: Config.textDisabledColor
                    elide: Text.ElideMiddle
                }
            }
        }

        Rectangle{
            id: contentSplit
            height: 1
            width: parent.width
            anchors{
                bottom: footItem.top
                bottomMargin: 2
            }
            color: Config.lineColor
        }

        Item{
            id: footItem
            width: parent.width
            height: 40
            anchors{
                bottom: parent.bottom
                bottomMargin: 5
            }
            Row{
                spacing: 10
                anchors{
                    right: parent.right
                    rightMargin: 20
                    verticalCenter: parent.verticalCenter
                }
                TextButton{
                    text: qsTr("取消")
                    width: 46
                    height: 30
                    onClicked: rootRectangle.visible=false;
                }

                TextButton{
                    text: qsTr("确定")
                    width: 46
                    height: 30
                    backgroundColor: Config.mouseCheckColor
                    backgroundEnterColor: backgroundColor
                    backgroundPressedColor: backgroundColor
                    textColor: "white"
                    textEnterColor: textColor
                    onPressed: {
                        rootRectangle.visible=false;
                        let arr=channelComboBox.currentModelChildren.cost.toString();
                        if(channelComboBox.currentIndex===0){
                            for(let i=1;i<channelListModel.count;i++){
                                if(i===1)
                                    arr=channelListModel.get(i).cost.toString();
                                else
                                    arr+=","+channelListModel.get(i).cost.toString();
                            }
                        }
                        if(showPathText.text!=="" && unitComboBox.currentIndex>=0)
                            controller.saveBinData(showPathText.text, 0, arr);
                    }
                }
            }
        }
    }
}


