import QtQuick 2.15
import "../config"

Rectangle {
    id: rootContent
    visible: false
    width: 175
    height: 70
    radius: 6
    color: Setting.mouseMeasureBackColor
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
                color: Setting.mouseMeasureTextColor
            }
            Text {
                text: "频率(Freq)"
                font.pixelSize: 12
                color: Setting.mouseMeasureTextColor
            }
            Text {
                text: "占空比(Duty)"
                font.pixelSize: 12
                color: Setting.mouseMeasureTextColor
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
                color: Setting.mouseMeasureTextColor
            }
            Text {
                text: ": "+set.freqText
                font.pixelSize: 12
                color: Setting.mouseMeasureTextColor
            }
            Text {
                text: ": "+set.dutyText
                font.pixelSize: 12
                color: Setting.mouseMeasureTextColor
            }
        }
    }
}
