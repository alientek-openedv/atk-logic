import QtQuick 2.13
import QtQuick.Controls 2.5
import "../config"
import "../style"

Popup{
    property bool showShortcutKey: false
    property bool isTwoModel: false
    property int maxWidth: list_view.textWidth+list_view.shortcutKeyTextWidth
    property int rowHeight: 30
    property var data: []
    property int lastCurrentIndex
    property bool isShowTop: false

    signal select(var index);
    signal containsTwo(var index, var containsMouse);

    id: mainPopup
    y: isShowTop ? -contentHeight : 0
    x: -contentWidth-2
    width: contentWidth
    height: contentHeight
    contentWidth: maxWidth+20;
    padding: 1
    clip: true;
    onOpened: refreshShow();
    function refreshShow(){
        model.clear();
        var temp=0;
        for(var i in data){
            if(data[i]["showText"]==="-")
                temp+=11;
            else
                temp+=rowHeight;
            model.append(data[i]);
        }
        contentHeight=temp+14;
    }

    background: Item{}
    Rectangle{
        width: parent.width
        height: parent.height
        radius: 4
        clip: true;
        color: Config.backgroundColor
        border{
            width: 1
            color: Config.lineColor
        }

        ListView {
            id: list_view
            property int textWidth
            property int shortcutKeyTextWidth
            model: ListModel{
                id: model
            }
            anchors{
                left: parent.left
                leftMargin: 1
                top: parent.top
                topMargin: 6
            }
            clip: true
            width: parent.width-2
            height: parent.height-2
            snapMode: ListView.NoSnap
            flickableDirection: Flickable.AutoFlickDirection
            boundsBehavior: Flickable.StopAtBounds
            ScrollIndicator.vertical: ScrollIndicator { }
            delegate: Rectangle{
                width: list_view.width
                height: showText==="-"?11:rowHeight
                color: showText==="-"?"transparent":(itemMouseArea.containsMouse&&!isTwoModel)||(isTwoModel&&lastCurrentIndex===buttonIndex)?Config.mouseCheckColor:"transparent"
                Rectangle{
                    visible: showText==="-"
                    width: parent.width
                    height: 1
                    color: Config.lineColor
                    anchors.verticalCenter: parent.verticalCenter
                }
                MouseArea{
                    id: itemMouseArea
                    anchors.fill: parent
                    hoverEnabled: true
                    enabled: showText!=="-"&&seleteType!==-1
                    onPressed: {
                        select(buttonIndex);
                        if(!isTwoModel||(isTwoModel&&shortcutKey!=="->"))
                            mainPopup.close();
                    }
                    onEntered: {
                        if(isTwoModel){
                            if(shortcutKey==="->")
                                containsTwo(buttonIndex, true);
                            else if(shortcutKey!=="->")
                                containsTwo(buttonIndex, false);
                        }
                        lastCurrentIndex=buttonIndex;
                    }
                }
                Item{
                    visible: showText!=="-"
                    width: parent.width-20
                    height: parent.height-20
                    anchors{
                        left: parent.left
                        top: parent.top
                        margins: 10
                    }
                    Text{
                        id: showTextElement
                        text: showText
                        font{
                            pixelSize: 12
                            bold: seleteType===2
                        }
                        anchors.verticalCenter: parent.verticalCenter
                        color: seleteType===-1||seleteType===1?Config.textDisabledColor:(itemMouseArea.containsMouse&&!isTwoModel)||(isTwoModel&&lastCurrentIndex===buttonIndex)?"white":Config.textColor
                        Component.onCompleted: {
                            if(showTextElement.contentWidth>list_view.textWidth)
                                list_view.textWidth=showTextElement.contentWidth;
                        }
                    }
                    Text{
                        id: shortcutKeyTextElement
                        text: shortcutKey
                        font.pixelSize: 12
                        color: Config.textDisabledColor
                        visible: showShortcutKey
                        anchors{
                            right: parent.right
                            verticalCenter: parent.verticalCenter
                        }
                        Component.onCompleted: {
                            if(visible&&shortcutKeyTextElement.contentWidth+20>list_view.shortcutKeyTextWidth)
                                list_view.shortcutKeyTextWidth=shortcutKeyTextElement.contentWidth+20;
                        }
                    }
                }
            }
        }
    }
}
