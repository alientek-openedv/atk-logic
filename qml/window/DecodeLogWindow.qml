import QtQuick 2.11
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import "../config"
import "../style"

Window {
    property var screen
    property var root
    property var showText
    id: window
    visible: true
    modality: Qt.NonModal
    transientParent: null
    flags: Qt.FramelessWindowHint
    color: "#00000000"
    width: dropShadow.width
    height: dropShadow.height

    Component.onCompleted: {
        root.getDecodeLogList();
    }

    //    //@----disable-check M16
    //    onClosing: function(closeevent){
    //        dataSend("123456");
    //    }

    Timer{
        interval: 100
        repeat: true
        running: true
        onTriggered: {
            let decode_log_list=root.getDecodeLogList();
            if(decode_log_list.length>0){
                while(decode_log_list.length>0)
                    showText+=decode_log_list.shift();
                noteEdit.showText=showText;
                noteEdit.contentY=noteEdit.contentHeight-noteEdit.height;
                noteEdit.returnToBounds();
            }
        }
    }

    Image {
        id: dropShadow
        width: 520
        height: 466
        source: "../../resource/image/SetWindowDropShadow.png"
        Rectangle{
            id: rootRectangle
            width: 504
            height: 450
            color: Config.backgroundColor
            radius: 10
            anchors{
                top: parent.top
                topMargin: 6
                horizontalCenter: parent.horizontalCenter
            }
            Item{
                id: titleItem
                width: parent.width
                height: 40
                Text {
                    text: qsTr("协议解析日志")
                    font.pixelSize: 14
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
                    onPressed: close_();
                }
                MouseArea{
                    anchors.fill: parent
                    acceptedButtons: Qt.LeftButton //只处理鼠标左键
                    property point clickPos: "0,0"
                    onPressed: clickPos=Qt.point(mouse.x,mouse.y)
                    onPositionChanged: {
                        var delta = Qt.point(mouse.x-clickPos.x, mouse.y-clickPos.y)
                        if(window.x+delta.x<Screen.virtualX)
                            window.setX(Screen.virtualX);
                        else if(window.x+window.width+delta.x>Screen.virtualX+screen.width)
                            window.setX(Screen.virtualX+screen.width-window.width);
                        else
                            window.setX(window.x+delta.x)

                        if(window.y+delta.y<Screen.virtualY)
                            window.setY(Screen.virtualY);
                        else if(window.y+window.height+delta.y>Screen.virtualY+screen.height)
                            window.setY(Screen.virtualY+screen.height-window.height);
                        else
                            window.setY(window.y+delta.y)
                    }
                }
            }

            Rectangle{
                id: titleSplit
                height: 1
                width: parent.width
                anchors.top: titleItem.bottom
                color: Config.lineColor
            }

            Item{
                id: contentColumn
                width: parent.width-20
                height: parent.height-112
                anchors{
                    top: titleSplit.bottom
                    topMargin: 10
                    left: parent.left
                    leftMargin: 10
                }

                QTextEdit{
                    id: noteEdit
                    width: parent.width
                    height: parent.height
                    textColor: Config.subheadColor
                    fontSize: 12
                    note: ""
                    canSelect: true
                    ScrollBar.vertical: QScrollBar {}
                }

            }

            Item{
                id: footerItem
                width: parent.width
                height: 51
                anchors{
                    bottom: parent.bottom
                    left: parent.left
                }
                Rectangle{
                    width: parent.width
                    height: 1
                    color: Config.lineColor
                }
                Row{
                    spacing: 10
                    anchors{
                        right: parent.right
                        rightMargin: 20
                        verticalCenter: parent.verticalCenter
                    }

                    TextButton{
                        text: qsTr("清除")
                        width: 46
                        height: 30
                        onClicked: {
                            noteEdit.showText="";
                            showText="";
                        }
                    }

                    TextButton{
                        text: qsTr("关闭")
                        width: 46
                        height: 30
                        backgroundColor: Config.mouseCheckColor
                        backgroundEnterColor: backgroundColor
                        backgroundPressedColor: backgroundColor
                        textColor: "white"
                        textEnterColor: textColor
                        onPressed: close_();
                    }
                }
            }
        }
    }

    function close_(){
        window.close();
    }
}
