import QtQuick 2.15
import QtQuick.Controls 2.5
import "../config"

Rectangle{
    property bool isLiveFollowing: true
    id: rootContent
    width: 58
    height: 26
    radius: 3
    visible: false
    z: 100
    color: Config.backgroundColor
    border{
        width: 1
        color: Config.lineColor
    }
    anchors{
        left: parent.left
        leftMargin: 14+(sConfig.isExpandSidebar?Config.headerWidth:Config.closeHeaderWidth)
        bottom: parent.bottom
        bottomMargin: 24
    }
    Behavior on anchors.leftMargin {
        NumberAnimation {
            duration: 100
        }
    }
    Row{
        width: parent.width-5
        height: parent.height
        spacing: 5
        anchors{
            left: parent.left
            leftMargin: 5
        }
        Image {
            width: 14
            height: 9
            fillMode: Image.PreserveAspectFit
            source: "qrc:resource/icon/"+(isLiveFollowing?"SwitchOpen_small.png":"SwitchClose_small.png")
            anchors.verticalCenter: parent.verticalCenter
        }
        Text {
            text: qsTr("实况")
            color: Config.textColor
            font.pixelSize: 14
            anchors{
                verticalCenter: parent.verticalCenter
                verticalCenterOffset: -1
            }
        }
    }

    MouseArea{
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton
        preventStealing: true
        propagateComposedEvents: false
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        onPressed: setLiveFollowing(!rootContent.isLiveFollowing);
        onContainsMouseChanged: parentQToolTip.isShow=containsMouse
    }

    QToolTip{
        id: parentQToolTip
        parent: parent
        direction: 2
        z: 100
        isShow: false
        closePolicy: Popup.NoAutoClose
        showTextPixelSize: 12
        showText: rootContent.isLiveFollowing?qsTr("关闭实况跟随"):qsTr("打开实况跟随")
    }

    function setLiveFollowing(isEnable){
        rootContent.isLiveFollowing=isEnable;
        controller.setLiveFollowing(rootContent.isLiveFollowing);
    }
}
