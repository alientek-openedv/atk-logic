import QtQuick 2.13
import QtQuick.Controls 2.5
import "../config"
import "../style"

Popup{
    property int maxWidth: 160
    property int rowHeight: 30
    property int maxBottom
    property var headers
    property var checks: []
    signal exportClicked();
    id: mainPopup
    y: height>maxBottom?maxBottom-height:0
    x: -contentWidth
    width: contentWidth
    height: contentHeight
    contentWidth: maxWidth+30;
    contentHeight: mainRectangle.height+2
    padding: 1
    clip: true

    onHeadersChanged: {
        checks.splice(0,checks.length);
        for(let i in headers)
            checks.push(true);
    }

    background: Item{}
    Rectangle{
        id: mainRectangle
        width: parent.width
        height: showColumn.height
        radius: 4
        clip: true;
        color: Config.backgroundColor
        border{
            width: 1
            color: Config.lineDisableColor
        }

        Column{
            id: showColumn
            width: parent.width-2
            height: decode_header.height+decode_listview.height+11
            anchors{
                top: parent.top
                topMargin: 5
                left: parent.left
                leftMargin: 1
            }

            spacing: 0
            Item{
                id: decode_header
                width: parent.width
                height: rowHeight
                Text {
                    text: qsTr("导出列：")
                    color: Config.textColor
                    anchors{
                        verticalCenter: parent.verticalCenter
                        left: parent.left
                        leftMargin: 24
                    }
                }
                TextButton{
                    id: exportButton
                    text: qsTr("导出")
                    anchors{
                        verticalCenter: parent.verticalCenter
                        right: parent.right
                        rightMargin: 12
                    }
                    height: 16
                    textFontSize: 12
                    textColor: Config.iceBlue
                    textEnterColor: Config.subheadColor
                    backgroundRectangle{
                        border{
                            width: 1
                            color: Config.iceBlue
                        }
                        radius: 8
                    }
                    onPressed: exportClicked();
                }
            }
            ListView {
                id: decode_listview
                model: headers
                clip: true
                width: parent.width
                height: contentHeight
                snapMode: ListView.NoSnap
                flickableDirection: Flickable.AutoFlickDirection
                boundsBehavior: Flickable.StopAtBounds
                ScrollIndicator.vertical: ScrollIndicator { }
                delegate: Rectangle{
                    width: decode_listview.width
                    height: rowHeight
                    color: itemMouseArea.containsMouse?Config.mouseCheckColor:"transparent"
                    MouseArea{
                        property bool isSelect: checks[index]
                        id: itemMouseArea
                        anchors.fill: parent
                        hoverEnabled: true
                        onPressed: {
                            checks[index]=!checks[index]
                            isSelect=checks[index]
                        }
                    }
                    Item{
                        id: checkedItem
                        width: 24
                        height: parent.height
                        Image {
                            anchors.centerIn: parent
                            visible: itemMouseArea.isSelect
                            width: 10
                            height: 8
                            fillMode: Image.PreserveAspectFit
                            source: "../../resource/icon/Checked.png"
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.horizontalCenter: parent.horizontalCenter
                        }
                    }
                    Text{
                        id: showTextElement
                        text: modelData
                        font.pixelSize: 12
                        anchors{
                            verticalCenter: parent.verticalCenter
                            left: checkedItem.right
                        }
                        color: itemMouseArea.containsMouse?"white":Config.textColor
                        Component.onCompleted: {
                            var temp=showTextElement.contentWidth;
                            if(temp>maxWidth)
                                maxWidth=temp;
                        }
                    }
                }
            }
        }
    }
}
