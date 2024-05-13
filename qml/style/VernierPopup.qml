import QtQuick 2.15
import QtQuick.Controls 2.5
import "../config"

Rectangle {
    property int vernierID: -1
    property var data_
    id: popupRoot
    width: 184
    height: 64
    radius: 6
    y: 10
    visible: false
    color: Config.measurePopupBackground2

    MouseArea{
        anchors.fill: parent
        hoverEnabled: true
        acceptedButtons: Qt.AllButtons
    }

    Connections{
        target: sSignal
        function onShowVernierPopup(vernierID_, mouse_x){
            if(sConfig.isRun)
                return;
            vernierID=vernierID_;
            popupRoot.visible=true;
            if(mouse_x+popupRoot.width+5>=measureContainer.width)
            {
                mouse_x-=(popupRoot.width+20);
                if(mouse_x+popupRoot.width+5>=measureContainer.width)
                    mouse_x=measureContainer.width-popupRoot.width-5
            }
            if(mouse_x<0)
                mouse_x=0;
            popupRoot.x=mouse_x;
            data_=vernierListModel.getAt(vernierID_);
            noteTextInput.showText=data_.note;
            var select=data_.dataColor.toString().toUpperCase().substring(1);
            for(var i in vernierButtonsRow.children){
                vernierButtonsRow.children[i].checked=(vernierButtonsRow.children[i].buttonID===select);
            }
        }
        function onCloseVernierPopup(){
            popupRoot.visible=false;
        }
    }

    QTextInput{
        id: noteTextInput
        width: parent.width-30
        height: 20
        showBorder: false
        textColor: Config.textDisabledColor
        backgroundColor: "transparent"
        noteText: qsTr("注释")
        anchors{
            top: parent.top
            topMargin: 8
            left: parent.left
            leftMargin: 16
        }
        onShowTextChanged: {
            if(vernierID!==-1)
                vernierListModel.set(vernierID,"note",showText);
        }
    }

    ButtonGroup{
        buttons: vernierButtonsRow.children
        onCheckedButtonChanged: {
            vernierListModel.set(vernierID,"dataColor","#"+checkedButton.buttonID);
            sSignal.channelRefresh(-1);
        }
    }

    Row{
        id: vernierButtonsRow
        spacing: 5
        anchors{
            left: parent.left
            leftMargin: 20
            top: noteTextInput.bottom
            topMargin: 5
        }
        VernierButton{
            buttonID: "ED5756"
        }
        VernierButton{
            buttonID: "0DA074"
        }
        VernierButton{
            buttonID: "7F5DCD"
        }
        VernierButton{
            buttonID: "F59647"
        }
        VernierButton{
            buttonID: "CE437E"
        }
        VernierButton{
            buttonID: "26A3D9"
        }
    }

    ImageButton{
        width: 11
        height: 11
        imageWidth: 9
        imageHeight: 11
        anchors{
            right: parent.right
            rightMargin: 10
            bottom: vernierButtonsRow.bottom
        }
        imageSource: "resource/icon/"+Config.tp+"/Delete.png"
        imageEnterSource: "resource/icon/DeleteEnter.png"
        onPressed: {
            vernierListModel.remove(vernierID)
            popupRoot.visible=false;
        }
    }
}
