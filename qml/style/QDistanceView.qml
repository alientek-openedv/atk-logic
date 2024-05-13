import QtQuick 2.11
import QtQuick.Controls 2.5
import "../config"

ListView {
    property int rowsHeight: 34
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
    model: ListModel{}
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
            property point vernierRecode
            id: vernierCalcRectangle
            height: vernierLeftComboBox.height+4
            width: parent?parent.width:0
            color: currentIndex===index?Config.mouseEnterColor:"transparent"
            MouseArea{
                id: rowMouseArea
                anchors.fill: parent
                hoverEnabled: true
            }
            Row{
                property bool isContainsMouse: rowMouseArea.containsMouse||deleteButton.containsMouse
                id: vernierCalcRow
                height: parent.height
                width: parent.width
                spacing: 5
                anchors{
                    left: parent.left
                    leftMargin: 10
                }
                onIsContainsMouseChanged: {
                    if(isContainsMouse)
                        listView.selectIndex=index;
                }
                Connections{
                    target: sSignal
                    function onVernierDataChanged(vernierID){
                        if(vernierID===vernierRecode.x || vernierID===vernierRecode.y)
                            vernierCalcRectangle.calcVernier();
                    }
                }
                QComboBox{
                    id: vernierLeftComboBox
                    width: 68
                    model: vernierListModel
                    autoSetCurrentIndex: false
                    currentIndex: -1
                    showArrows: false
                    modelCount: vernierListModel.count_
                    onCurrentModelChildrenChanged: vernierCalcRectangle.calcVernier();
                }
                Text {
                    text: "-"
                    color: Config.textColor
                    font.pixelSize: 12
                    anchors.verticalCenter: parent.verticalCenter
                }
                QComboBox{
                    id: vernierRightComboBox
                    width: 68
                    model: vernierListModel
                    autoSetCurrentIndex: false
                    currentIndex: -1
                    showArrows: false
                    modelCount: vernierLeftComboBox.modelCount
                    onCurrentModelChildrenChanged: vernierCalcRectangle.calcVernier()
                }
                Text {
                    text: "="
                    color: Config.textColor
                    font.pixelSize: 12
                    anchors.verticalCenter: parent.verticalCenter
                }
                Column{
                    width: parent.width
                    Text {
                        id: vernierCalcText
                        height: 12
                        color: Config.textColor
                        font.pixelSize: 12
                    }
                    Text {
                        id: vernierCalcHzText
                        height: 12
                        color: Config.textColor
                        font.pixelSize: 12
                    }
                    anchors{
                        verticalCenter: parent.verticalCenter
                        verticalCenterOffset: -1
                    }
                }
            }
            ImageButton{
                id: deleteButton
                visible: vernierCalcRow.isContainsMouse && listView.selectIndex===index
                imageSource: "resource/icon/"+Config.tp+"/Delete.png"
                imageEnterSource: "resource/icon/DeleteEnter.png"
                imageWidth: 9
                imageHeight: 11
                width: 16
                height: 16
                z: 5
                anchors{
                    verticalCenter: parent.verticalCenter
                    right: parent.right
                    rightMargin: 2
                }
                onPressed: listView.model.remove(index);
            }
            function calcVernier(){
                if(vernierLeftComboBox.currentIndex<0 || vernierRightComboBox.currentIndex<0){
                    vernierCalcText.text="";
                    vernierCalcHzText.text="";
                    return;
                }
                let left=vernierListModel.get(vernierLeftComboBox.currentIndex);
                let right=vernierListModel.get(vernierRightComboBox.currentIndex);
                vernierRecode=Qt.point(left.vernierID, right.vernierID);
                let num=left.position-right.position;
                if(!isNaN(num)){
                    vernierCalcText.text=time22Unit(num, 3)
                    if(num!==0)
                        vernierCalcHzText.text=hz2Unit(1000000000/Math.abs(num), 3);
                    else
                        vernierCalcHzText.text="0 Hz"
                }
            }
        }
    }
}
