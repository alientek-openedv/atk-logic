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
import "../config"

Rectangle {
    property bool isOpen: false
    property int schedule: 0
    property int type
    id: rootContent

    width: 280
    height: showRow.height+30
    z: 1000

    color: Config.lineColor
    radius: 8

    onIsOpenChanged: {
        if(isOpen)
            openAnimator.start();
        else
            closeAnimator.start();
    }

    onScheduleChanged: {
        if(schedule===100&&type===0)
        {
            closeTimer.start();
            set.complete=true;
        }
        else
            set.complete=false;
    }

    QtObject{
        id: set
        property var showText: [qsTr("连接设备中 - "),qsTr("设备状态异常，请重新插拔尝试。"),qsTr("连接设备失败，请重新插拔尝试。"),qsTr("正在升级固件 - "),
            qsTr("升级固件失败"), qsTr("保存文件完成。"), qsTr("FPGA版本过低，请升级固件。"), qsTr("软件版本过低，请升级软件。")]
        property var completeText: qsTr("连接设备成功")
        property bool complete: false
        property string updateText: "<a href=\"update\"><font color=\"#3587fe\">"+qsTr("立即升级")+"</font></a>"
    }

    Timer{
        id: closeTimer
        interval: 2000
        onTriggered: closeButton.clicked();
    }

    Row{
        id: showRow
        anchors{
            left: parent.left
            verticalCenter: parent.verticalCenter
            margins: 20
        }
        height: showText.height
        width: parent.width-40
        spacing: 10

        Image {
            width: 16
            height: 16
            source: "../../resource/icon/"+(type===0?set.complete?"Complete.png":"Info.png":(type===3||type===5)?"Info.png":"Error.png")
            anchors{
                top: parent.top
                topMargin: 2
            }
        }

        Text {
            id: showText
            text: set.complete?set.completeText:set.showText[type] + (type===0||type===3?schedule+"%":"") + (type===6?set.updateText:"")
            width: parent.width-30
            height: contentHeight
            font.pixelSize: 14
            color: Config.textColor
            wrapMode: Text.Wrap
            anchors{
                verticalCenter: parent.verticalCenter
                verticalCenterOffset: -1
            }
            onLinkActivated: {
                if(link==="update")
                    root.enterBootloader();
            }
        }
    }

    ImageButton{
        id: closeButton
        width: 10
        height: 10
        imageSource: "resource/icon/"+Config.tp+"/Close_small.png"
        imageEnterSource: imageSource
        anchors{
            top: parent.top
            right: parent.right
            margins: 10
        }
        onClicked: isOpen=false;
    }

    XAnimator {
        target: rootContent;
        id: openAnimator
        from: rootContent.parent.x - rootContent.width - 5
        to: rootContent.parent.x + 15
        duration: 100
        alwaysRunToEnd: false
        onStarted: rootContent.visible=true;
        onFinished: {
            if(type===5)
                closeTimer.start();
        }
    }

    XAnimator {
        target: rootContent;
        id: closeAnimator
        from: openAnimator.to
        to: openAnimator.from
        duration: 100
        alwaysRunToEnd: false
        onFinished: rootContent.visible=false;
    }
}
