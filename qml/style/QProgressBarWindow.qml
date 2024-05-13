import QtQuick 2.15
import QtQuick.Controls 2.5
import "../config"

Rectangle{
    property int type: 0//0-3=采集状态,4=保存文件,5=加载文件,6=重新计算毛刺过滤,7=等待停止解码,8解压文件中,9压缩文件中
    property color primaryColor: "white"
    property color secondaryColor: "#00af54"
    property real minimumValue: 0
    property real maximumValue: 100
    property real currentValue: 0
    property real interval: 200

    QtObject{
        id: set
        property real centerWidth: canvas.width / 2
        property real centerHeight: canvas.height / 2
        property real radius: Math.min(canvas.width-12, canvas.height-12) / 2
        property real angle: (currentValueShow - minimumValue) / (maximumValue - minimumValue) * 2 * Math.PI
        property real angleOffset: -Math.PI / 2

        onAngleChanged: canvas.requestPaint();

        property real currentValueRecode

        property bool containsMouse_: false
        property var showText: [qsTr("预采集"),qsTr("等待触发"),qsTr("已触发，等待采集！"),qsTr("已采集，等待传输！"),
            qsTr("保存文件中"),qsTr("加载文件中"),qsTr("计算数据中"),qsTr("等待停止解码"),qsTr("解压文件中"),qsTr("压缩文件中")]
        property var closeText: qsTr("停止")

        property real currentValueShow
        property bool isFinished: true

        NumberAnimation on currentValueShow{
            id: currentValueAnimation
            duration: interval
            onFinished: {
                if(currentValue>=maximumValue&&type>2)
                    rootRectangle.visible=false;
                set.isFinished=true;
            }
        }

        onCurrentValueShowChanged: {
            if(currentValueAnimation.duration!=50)
                txtProgress.text=Math.trunc(currentValueShow).toString();
        }
    }

    signal closeButtonClick

    id: rootRectangle
    visible: false
    color: "#70000000"
    z: 1000

    onPrimaryColorChanged: canvas.requestPaint()
    onSecondaryColorChanged: canvas.requestPaint()

    onTypeChanged: {
        if(type==0||type==1){
            currentValueAnimation.stop();
            set.isFinished=true;
        }
    }

    onCurrentValueChanged: {
        if(currentValue===minimumValue)
        {
            set.currentValueRecode=minimumValue;
            set.currentValueShow=minimumValue;
            set.isFinished=true;
            txtProgress.text=minimumValue.toString();
        }

        var temp=currentValue;
        currentValueAnimation.from=set.currentValueRecode;
        currentValueAnimation.to=temp;

        if(currentValue===maximumValue)
        {
//            currentValueAnimation.stop();
//            currentValueAnimation.duration=10;
//            txtProgress.text=Math.trunc(currentValue).toString();
//            set.isFinished=true;
            rootRectangle.visible=false;
            return;
        }
        else if(currentValueAnimation.duration!=interval)
            currentValueAnimation.duration=interval;
        if(set.isFinished)
        {
            set.isFinished=false;
            currentValueAnimation.start();
            set.currentValueRecode=temp;
        }
    }

    MouseArea{
        anchors.fill: parent
        preventStealing: true
        hoverEnabled: true
        acceptedButtons: Qt.AllButtons
        onPressed: mouse.accepted=true
        onWheel: wheel.accepted=true
    }

    Item{
        anchors.centerIn: parent
        width: canvas.width
        height: canvas.height+stopButton.height+40
        Canvas {
            id: canvas
            antialiasing: true
            width: 210
            height: 210
            onPaint: {
                var ctx = getContext("2d");
                ctx.save();
                ctx.clearRect(0, 0, canvas.width, canvas.height);
                ctx.fillStyle = primaryColor;
                ctx.lineWidth = 6;

                // First, thinner arc
                // From angle to 2*PI
                ctx.beginPath();
                ctx.strokeStyle = primaryColor;
                ctx.arc(set.centerWidth,
                        set.centerHeight,
                        set.radius,
                        set.angleOffset + set.angle,
                        set.angleOffset + 2*Math.PI);
                ctx.stroke();

                // Second, thicker arc
                // From 0 to angle
                ctx.beginPath();
                ctx.strokeStyle = secondaryColor;
                ctx.arc(set.centerWidth,
                        set.centerHeight,
                        set.radius,
                        set.angleOffset,
                        set.angleOffset + set.angle);
                ctx.stroke();
                ctx.restore();
            }

            Item{
                id: progressItem
                width: progressItemRow.width
                height: progressItemRow.height
                anchors{
                    horizontalCenter: parent.horizontalCenter
                    top: parent.top
                    topMargin: 57
                }
                Row{
                    id: progressItemRow
                    width: txtProgress.contentWidth+20
                    height: txtProgress.contentHeight
                    spacing: 5
                    Text {
                        id: txtProgress
                        text: "0"
                        font.pixelSize: 54
                        color: primaryColor
                    }
                    Text {
                        font.pixelSize: 24
                        text: "%"
                        color: primaryColor
                        anchors{
                            bottom: parent.bottom
                            bottomMargin: 8
                        }
                    }
                }
            }

            Text {
                font.pixelSize: 16
                text: set.showText[type]
                color: primaryColor
                anchors{
                    horizontalCenter: parent.horizontalCenter
                    top: progressItem.bottom
                }
            }

        }
        TextButton{
            id: stopButton
            width: 100
            height: 30
            visible: type!=6&&type!=7&&type!=8&&type!=9
            text: set.closeText
            textFontSize: 16
            anchors{
                top: canvas.bottom
                topMargin: 40
                horizontalCenter: parent.horizontalCenter
            }
            textColor: "#383838"
            backgroundColor: primaryColor
            backgroundEnterColor: backgroundColor
            backgroundPressedColor: backgroundColor
            backgroundRectangle.radius: 4
            onPressed: closeButtonClick();
        }
    }
}
