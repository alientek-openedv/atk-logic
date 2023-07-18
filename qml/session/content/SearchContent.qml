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
import QtQuick.Layouts 1.3
import "../../config"
import "../../style"

Rectangle {
    color: Config.background2Color

    Connections{
        target: sSignal
        function onCollectComplete(){
            searchText();
        }

        function onVernierDataChanged(vernierID){
            for(let i=0;i<vernierListShowModel.count;i++){
                let data=vernierListShowModel.get(i);
                if(data["id"]===vernierID){
                    data["cost"]=vernierListModel.getAt(vernierID).position;
                    break;
                }
            }
        }

        function onVernierMoveComplete(id){
            if(dataTableView.startID===id||dataTableView.endID===id)
                searchText();
        }

        function onSearchModelChanged(){
            if(vernierStartComboBox.currentIndex!==-1)
                vernierStartComboBox.currentIndex=-1;
            else{
                if(vernierStartComboBox.currentIndex>=vernierListShowModel.count)
                    vernierStartComboBox.currentIndex=vernierListShowModel.count-1;
                else
                    vernierStartComboBox.currentIndex=vernierStartComboBox.current;
            }

            if(vernierEndComboBox.currentIndex!==-1)
                vernierEndComboBox.currentIndex=-1;
            else{
                if(vernierEndComboBox.currentIndex>=vernierListShowModel.count)
                    vernierEndComboBox.currentIndex=vernierListShowModel.count-1;
                else
                    vernierEndComboBox.currentIndex=vernierEndComboBox.current;
            }
        }

        function onResetSearch(){
            searchTableModel.stopAll();
            stageSearchTextInput.reset();
            dataTableView.resetScrollBarPosition();
        }
    }

    Text {
        id: titleText
        text: qsTr("数据搜索")
        font.pixelSize: 18
        color: Config.mouseCheckColor
        anchors{
            top: parent.top
            topMargin: 14
            left: parent.left
            leftMargin: 14
        }
    }
    Item {
        id: topItem
        width: parent.width-28
        height: stageSearchFlagRow.height+stageSearchTextInput.height+vernierRow.height+textRowLayout.height+46
        anchors{
            top: titleText.bottom
            topMargin: 5
            left: parent.left
            leftMargin: 14
        }
        Row{
            id: stageSearchFlagRow
            spacing: 6
            height: 20
            anchors{
                left: parent.left
                right: parent.right
                top: parent.top
            }
            RowLayout{
                width: 112
                height: parent.height
                spacing: 2
                Text {
                    text: "15"
                    color: Config.textColor
                    font.pixelSize: 14
                }
                Rectangle{
                    height: 1
                    Layout.fillWidth: true
                    color: Config.iceBlue
                }
                Text {
                    text: "8"
                    color: Config.textColor
                    font.pixelSize: 14
                }
            }
            RowLayout{
                width: 107
                height: parent.height
                spacing: 2
                Text {
                    text: "7"
                    color: Config.textColor
                    font.pixelSize: 14
                }
                Rectangle{
                    height: 1
                    Layout.fillWidth: true
                    color: Config.iceBlue
                }
                Text {
                    text: "0"
                    color: Config.textColor
                    font.pixelSize: 14
                }
            }
        }

        TriggerTextInput{
            id: stageSearchTextInput
            disable: sConfig.loopState!==-1 || sConfig.isRun
            anchors{
                top: stageSearchFlagRow.bottom
                topMargin: 5
                left: parent.left
            }
            onShowTextChanged: searchText();
        }

        Row{
            id: vernierRow
            width: 240
            height: vernierStartComboBox.height
            spacing: 10
            anchors{
                top: stageSearchTextInput.bottom
                topMargin: 10
                left: parent.left
            }

            Row{
                width: parent.width/2-5
                height: vernierStartComboBox.height
                spacing: 5
                Text{
                    id: vernierStartText
                    text: qsTr("起始")
                    font.pixelSize: 14
                    color: Config.textColor
                    anchors.verticalCenter: parent.verticalCenter
                }
                QComboBox{
                    property int current: 0
                    id: vernierStartComboBox
                    width: parent.width-vernierStartText.width-5
                    model: vernierListShowModel
                    disable: stageSearchTextInput.disable
                    Component.onCompleted: currentIndex=0
                    onCurrentIndexChanged: {
                        if(currentIndex!=-1&&currentIndex<vernierListShowModel.count){
                            current=currentIndex;
                            searchText();
                        }
                        else if(currentIndex==-1)
                        {
                            if(currentIndex>=vernierListShowModel.count)
                                currentIndex=vernierListShowModel.count-1;
                            else
                                currentIndex=current;
                        }
                    }
                }
            }

            Row{
                width: parent.width/2-5
                height: vernierEndComboBox.height
                spacing: 5
                Text{
                    id: vernierEndText
                    text: qsTr("结束")
                    font.pixelSize: 14
                    color: Config.textColor
                    anchors.verticalCenter: parent.verticalCenter
                }
                QComboBox{
                    property int current: 0
                    id: vernierEndComboBox
                    width: parent.width-vernierEndText.width-5
                    model: vernierListShowModel
                    disable: stageSearchTextInput.disable
                    Component.onCompleted: currentIndex=1
                    onCurrentIndexChanged: {
                        if(currentIndex!=-1&&currentIndex<vernierListShowModel.count){
                            current=currentIndex;
                            searchText();
                        }
                        else if(currentIndex==-1)
                        {
                            if(currentIndex>=vernierListShowModel.count)
                                currentIndex=vernierListShowModel.count-1;
                            else
                                currentIndex=current;
                        }
                    }
                }
            }
        }

        Rectangle{
            id: splitRectangle
            height: 1
            anchors{
                top: vernierRow.bottom
                topMargin: 15
                left: parent.left
                right: parent.right
            }
            color: Config.lineColor
        }

        RowLayout{
            id: textRowLayout
            spacing: 16
            anchors{
                top: splitRectangle.bottom
                topMargin: 10
                left: parent.left
                right: parent.right
            }
            Column{
                spacing: 5
                Text {
                    text: qsTr("X：不关心")
                    font.pixelSize: 14
                    color: Config.subheadColor
                }
                Text {
                    text: qsTr("R：上升沿")
                    font.pixelSize: 14
                    color: Config.subheadColor
                }
            }
            Column{
                spacing: 5
                Text {
                    text: qsTr("0：低电平")
                    font.pixelSize: 14
                    color: Config.subheadColor
                }
                Text {
                    text: qsTr("F：下降沿")
                    font.pixelSize: 14
                    color: Config.subheadColor
                }
            }
            Column{
                spacing: 5
                Text {
                    text: qsTr("1：高电平")
                    font.pixelSize: 14
                    color: Config.subheadColor
                }
                Text {
                    text: qsTr("C：所有沿")
                    font.pixelSize: 14
                    color: Config.subheadColor
                }
            }
        }
    }
    SearchDataTableView{
        property int startID:-1
        property int endID:-1
        id: dataTableView
        height: parent.height - topItem.height - titleText.height - 40
        width: parent.width
        anchors{
            top: topItem.bottom
            topMargin: 20
            left: parent.left
            right: parent.right
        }
        columnsWidth: [62,103,103]
        headerList: [qsTr("序号"),qsTr("起始"),qsTr("长度")]
        view.model: searchTableModel
    }

    function searchText(){
        if(vernierListShowModel.get(vernierStartComboBox.currentIndex) && vernierListShowModel.get(vernierEndComboBox.currentIndex) && !sConfig.isRun){
            vernierListModel.removeBusy(dataTableView.startID,"search",0);
            vernierListModel.removeBusy(dataTableView.endID,"search",1);
            dataTableView.startID=vernierListShowModel.get(vernierStartComboBox.currentIndex).id;
            dataTableView.endID=vernierListShowModel.get(vernierEndComboBox.currentIndex).id;
            vernierListModel.appendBusy(dataTableView.startID,"search",0,"SearchData - Start");
            vernierListModel.appendBusy(dataTableView.endID,"search",1,"SearchData - End");
            if(searchTableModel.search(stageSearchTextInput.showText, vernierListShowModel.get(vernierStartComboBox.currentIndex).cost, vernierListShowModel.get(vernierEndComboBox.currentIndex).cost)){
                dataTableView.resetScrollBarPosition();
                dataTableView.selectRow=-1;
            }
        }
    }
}
