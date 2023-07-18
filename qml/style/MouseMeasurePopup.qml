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
    id: rootContent
    visible: false
    width: 175
    height: 80
    radius: 6
    color: "#ddbcedff"
    z: 1000

    QtObject{
        id: set
        property string periodText
        property string freqText
        property string dutyText
        property var channelID
    }

    onXChanged: {
        if(x+width+5>parent.width)
            x=parent.width-width-5
    }

    Connections{
        target: sSignal
        function onShowMouseMeasurePopup(x_,channelID,period,freq,duty){
            set.channelID=channelID;
            rootContent.x=x_+5;
            set.periodText=period;
            set.freqText=freq;
            set.dutyText=duty;
        }

        function onCloseMouseMeasurePopup(){
            rootContent.visible=false;
        }

        function onSendChannelY(channelID,type,y){
            if(typeof(channelID)==="number"){
                if(channelID===set.channelID && type===1){
                    rootContent.y=y;
                    rootContent.visible=true;
                }
            }
        }
    }
    Item {
        width: parent.width-40
        height: parent.height-40
        anchors{
            left: parent.left
            top: parent.top
            margins: 10
        }

        Column{
            id: column1
            width: 70
            spacing: 5
            Text {
                text: "周期(Period)"
                font.pixelSize: 12
                color: "#383838"
            }
            Text {
                text: "频率(Freq)"
                font.pixelSize: 12
                color: "#383838"
            }
            Text {
                text: "占空比(Duty)"
                font.pixelSize: 12
                color: "#383838"
            }
        }
        Column{
            width: parent.width-column1.width-10
            spacing: 5
            anchors{
                left: column1.right
                leftMargin: 10
            }
            Text {
                text: ": "+set.periodText
                font.pixelSize: 12
                color: "#383838"
            }
            Text {
                text: ": "+set.freqText
                font.pixelSize: 12
                color: "#383838"
            }
            Text {
                text: ": "+set.dutyText
                font.pixelSize: 12
                color: "#383838"
            }
        }
    }
}
