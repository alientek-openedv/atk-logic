import QtQuick 2.15
import QtQuick.Controls 2.5
import "../config"

ComboBox {
    property alias backgroundRectangle: backRect
    property string currentTextValue: ""
    property var currentModelChildren
    property int popupMaxHeight: 30*8
    property bool showArrows: true
    property bool autoSetCurrentIndex: true
    property bool wheelChanged: Setting.componentWheelChanged
    property bool disable: false
    property var modelCount: model.count
    property bool isError: false
    property int maxTextWidth: 0
    property int elide_: Text.ElideMiddle

    onDisableChanged: {
        if(disable)
            popup.close();
    }

    onModelCountChanged: {
        if(autoSetCurrentIndex){
            if(currentIndex>=modelCount)
                currentIndex=modelCount===0?-1:0;
            refreshText();
        }else if(currentIndex>=0){
            let tmp=model.get(currentIndex);
            if(tmp.showText!==currentModelChildren.showText || tmp.cost!==currentModelChildren.cost){
                let isSet=true;
                if(currentIndex>0){
                    tmp=model.get(currentIndex-1);
                    if(tmp.showText===currentModelChildren.showText && tmp.cost===currentModelChildren.cost){
                        isSet=false;
                        currentIndex--;
                    }
                }
                if(isSet)
                    currentIndex=-1;
            }

        }
    }

    id: rootComboBox
    implicitHeight: 30
    focusPolicy: Qt.NoFocus
    clip: true
    delegate: ItemDelegate
    {
        clip: true
        height: 30
        width: rootComboBox.width
        contentItem: Text {
            id: contentItemText
            text: showText?showText:""
            elide: elide_
            color: rootComboBox.highlightedIndex === index ? "white" : Config.textColor
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: 12
            onContentWidthChanged: {
                if(contentWidth+30>maxTextWidth)
                    maxTextWidth=contentWidth+30;
            }
        }
        background: Rectangle{
            anchors.fill: parent
            color: rootComboBox.highlightedIndex === index ? Config.mouseCheckColor : "transparent"
        }
        highlighted: rootComboBox.highlightedIndex === index
    }

    indicator: Image{
        visible: showArrows
        width: 9
        height: 5
        fillMode: Image.PreserveAspectFit
        anchors{
            verticalCenter: parent.verticalCenter
            right: parent.right
            rightMargin: 6
        }
        source: disable?"qrc:resource/icon/"+Config.tp+"/PullDownDisable.png":"qrc:resource/icon/"+Config.tp+"/PullDown.png"
    }

    contentItem: Text {
        leftPadding: 6
        rightPadding: rootComboBox.indicator.width + rootComboBox.spacing
        text: currentTextValue
        horizontalAlignment: showArrows?Text.AlignLeft:Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: elide_
        color: disable ? Config.textDisabledColor:Config.textColor
        font.pixelSize: 12
        onContentWidthChanged: {
            if(contentWidth+30>maxTextWidth)
                maxTextWidth=contentWidth+30;
        }
    }

    onCurrentIndexChanged: refreshText();

    onPressedChanged: {
        if(disable)
            pressed=false
    }

    popup: Popup {
        id: popup
        y: rootComboBox.height;
        width: rootComboBox.width;
        height: contentItem.implicitHeight>popupMaxHeight?popupMaxHeight:contentItem.implicitHeight;
        onHeightChanged: {
            if(height!==(contentItem.implicitHeight>popupMaxHeight?popupMaxHeight:contentItem.implicitHeight) && height===0)
                height=contentItem.implicitHeight>popupMaxHeight?popupMaxHeight:contentItem.implicitHeight;
        }

        clip: true
        contentItem: ListView {
            clip: true;
            implicitHeight: contentHeight;
            onImplicitHeightChanged: showAnimation.to=implicitHeight>popupMaxHeight?popupMaxHeight:implicitHeight;
            flickableDirection: Flickable.AutoFlickDirection
            boundsBehavior: Flickable.StopAtBounds
            model: rootComboBox.delegateModel;
            currentIndex: rootComboBox.highlightedIndex;
            highlightMoveDuration: 0;
            anchors.fill: parent
            snapMode: ListView.NoSnap
            ScrollIndicator.vertical: ScrollIndicator {}
        }

        background: Rectangle {
            anchors.fill: parent
            color: Config.borderBackgroundColor;
            border{
                width: 1
                color: Config.borderLineColor
            }
            radius: 4;
            clip: true
        }

        onOpenedChanged: {
            if(opened)
                showAnimation.start();
        }


        PropertyAnimation {
            id: showAnimation
            target: popup;
            properties: "height";
            from: 1
            to: contentItem.implicitHeight>popupMaxHeight?popupMaxHeight:contentItem.implicitHeight;
            duration: 100
        }
    }

    background: Rectangle {
        id: backRect
        anchors.fill: parent
        color: Config.borderBackgroundColor
        border{
            width: 1
            color: isError?Config.red:rootMouseArea.containsMouse && wheelChanged && !disable?Config.mouseCheckColor:disable?Config.lineDisableColor:Config.borderLineColor
        }
        radius: 4
    }

    MouseArea{
        id: rootMouseArea
        anchors.fill: parent
        acceptedButtons: Qt.NoButton
        hoverEnabled: true
        onWheel: {
            if(wheelChanged)
                wheelChangedSelect(wheel.angleDelta.y>0)
        }
        enabled: !rootComboBox.disable
    }

    function wheelChangedSelect(isUp){
        if(isUp){
            if(currentIndex>0)
                currentIndex--;
        }else{
            if(currentIndex+1<modelCount)
                currentIndex++;
            else if(modelCount!==0 && currentIndex<0)
                currentIndex=0;
        }
    }

    function refreshText(){
        if(currentIndex<0){
            currentTextValue="";
            return;
        }
        currentModelChildren=model.get(currentIndex);
        if(currentModelChildren && currentModelChildren.showText)
            currentTextValue=currentModelChildren.showText;
        else
            currentTextValue="";
    }
}


