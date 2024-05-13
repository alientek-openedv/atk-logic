import QtQuick 2.11
import QtQuick.Window 2.15
import "../config"
import "../style"

Window {
    property var screen
    id: window
    visible: true
    modality: Qt.WindowModal
    flags: Qt.Window | Qt.FramelessWindowHint
    color: "#00000000"
    width: dropShadow.width
    height: dropShadow.height

    Component.onCompleted: {
        window.y=screen.bottom-screen.height/2-window.height/2
        window.x=screen.right-screen.width/2-window.width/2
    }

    Connections{
        target: Signal
        function onShowInitText(state, text)
        {
            if(state===1)
                showText.text=text;
        }
    }

    Image {
        id: dropShadow
        width: 807
        height: 427
        mipmap: true
        source: "../../resource/image/Loading_"+Config.tp+".png"
    }
    Text{
        id: showText
        text: qsTr("加载程序资源")
        font.pixelSize: 14
        color: Config.textColor
        anchors{
            top: parent.top
            topMargin: 320
            left: parent.left
            leftMargin: 40
        }
    }
    Column{
        spacing: 5
        anchors{
            left: showText.left
            top: showText.bottom
            topMargin: 10
        }
        Text {
            text: "© 2013-2023 Alientek Systems incorporated. All rights reserved."
            color: Config.textDisabledColor
            font.pixelSize: 11
        }
        Text {
            text: qsTr('请参见帮助中的版权信息， "关于" ...')
            color: Config.textDisabledColor
            font.pixelSize: 11
        }
    }


}
