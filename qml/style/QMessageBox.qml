import QtQuick 2.15
import QtQuick.Controls 2.5
import "../config"

Rectangle{
    property string titleText
    property string showText
    property int type: 0//0=确定，1=保存
    property bool showCancel: false
    property int lastButton: -1//0=取消，1=确定，2=立即退出，3=×按钮
    property alias isNotShow: notShowCheckBox.isCheck

    id: rootControl
    color: "#33000000"
    z: 1000
    visible: false
    onVisibleChanged: {
        if(visible)
        {
            notShowCheckBox.isCheck=false;
            lastButton=-1;
            titleText_.focus=true;
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
        anchors.centerIn: parent
        source: "../../resource/image/MessageDropShadow.png"
        width: 416
        height: 216
        Rectangle{
            id: mainRectangle
            width: 400
            height: 200
            radius: 8
            anchors{
                top: parent.top
                topMargin: 6
                horizontalCenter: parent.horizontalCenter
            }
            color: Config.backgroundColor
            Text {
                id: titleText_
                text: titleText
                font.pixelSize: 14
                color: Config.iceBlue
                anchors{
                    top: parent.top
                    left: parent.left
                    margins: 10
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
                onPressed: {
                    lastButton=3;
                    rootControl.visible=false
                }
            }

            Rectangle{
                id: splitRectangle
                height: 1
                anchors{
                    top: titleText_.bottom
                    topMargin: 10
                    left: parent.left
                    right: parent.right
                }
                color: Config.lineColor
            }

            Image {
                id: imageIcon
                width: 24
                height: 24
                fillMode: Image.PreserveAspectFit
                anchors{
                    left: parent.left
                    leftMargin: 10
                    top: splitRectangle.bottom
                    topMargin: 20
                }
                source: "../../resource/icon/Warning.png"
            }

            Text {
                text: showText
                anchors{
                    left: imageIcon.right
                    leftMargin: 10
                    right: parent.right
                    rightMargin: 10
                    top: imageIcon.top
                }
                wrapMode: Text.WordWrap
                font.pixelSize: 16
                color: Config.textColor
            }

            Row{
                spacing: 10
                height: 30
                anchors{
                    right: parent.right
                    bottom: parent.bottom
                    margins: 15
                }

                TextButton{
                    id: okButton
                    text: type===0?qsTr("确定"):qsTr("保存")
                    height: 30
                    backgroundColor: Config.mouseCheckColor
                    backgroundEnterColor: backgroundColor
                    backgroundPressedColor: backgroundColor
                    textColor: "white"
                    textEnterColor: textColor
                    onPressed: {
                        lastButton=1;
                        rootControl.visible=false
                    }
                }
                TextButton{
                    id: closeButton
                    text: type===0?qsTr("取消"):qsTr("不保存")
                    height: 30
                    backgroundColor: Config.mouseCheckColor
                    backgroundEnterColor: backgroundColor
                    backgroundPressedColor: backgroundColor
                    textColor: "white"
                    textEnterColor: textColor
                    onPressed: {
                        lastButton=0;
                        rootControl.visible=false
                    }
                }
                TextButton{
                    id: closeAllButton
                    text: qsTr("退出")
                    visible: type!=0
                    height: 30
                    backgroundColor: Config.mouseCheckColor
                    backgroundEnterColor: backgroundColor
                    backgroundPressedColor: backgroundColor
                    textColor: "white"
                    textEnterColor: textColor
                    onPressed: {
                        lastButton=2;
                        rootControl.visible=false
                    }
                }
            }

            QCheckBox{
                id: notShowCheckBox
                visible: type===1
                buttonText: qsTr("不再提示")
                anchors{
                    left: parent.left
                    bottom: parent.bottom
                    bottomMargin: 22
                    margins: 15
                }
            }
        }
    }
}
