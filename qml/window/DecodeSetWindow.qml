import QtQuick 2.11
import QtQuick.Window 2.15
import "../config"
import "../style"

Window {
    signal dataSend(var decodeJson)
    property var channels
    property var decodeJson
    property var vernierJson
    property string initDecode: ""
    property var initJson
    property var screen
    property bool isChanged: false
    property bool showRequiredTips: false
    property bool showClashTips: false
    property bool showSelectChannel: false
    property string titleName: ""
    id: window
    visible: true
    modality: Qt.WindowModal
    flags: Qt.FramelessWindowHint
    color: "#00000000"
    width: rootRectangle.width+12
    height: rootRectangle.height+15

    Timer{
        id: changedTimer
        interval: 300
        onTriggered: isChanged=false;
    }

    Connections{
        target: Signal
        function onEscKeyPress()
        {
            window.close();
        }
    }

    Component.onCompleted: {
        Config.isSetModel=true;
        vernierStartComboBox.currentIndex=0;
        vernierEndComboBox.currentIndex=1;
        var i=0;
        for(i=0;i<vernierJson.length;i++){
            vernierStartListModel.append({"showText":vernierJson[i]["name"],"cost":vernierJson[i]["position"],"id_":vernierJson[i]["id"]});
            vernierEndListModel.append({"showText":vernierJson[i]["name"],"cost":vernierJson[i]["position"],"id_":vernierJson[i]["id"]});
        }
        if(initJson){
            for(i=0;i<vernierStartListModel.count;i++){
                var item=vernierStartListModel.get(i);
                if(item.showText===initJson["main"]["startName"])
                    vernierStartComboBox.currentIndex=i
                if(item.showText===initJson["main"]["endName"])
                    vernierEndComboBox.currentIndex=i
            }
            selectDecode(initJson["main"]["first"],true)
            decodeListView.outJson=initJson;
            decodeViewModel.clear();
            selectInitDecode(initJson["main"]["first"],initJson);
            for(var j in initJson["main"]["second"])
                selectInitDecode(initJson["main"]["second"][j],initJson);
            positionChangedTimer.isInit=true;
            titleName=initJson["main"]["showText"];
        }else{
            positionChangedTimer.isInit=initParent(initDecode);
            titleName=initDecode;
        }
        x=-15000;
        positionChangedTimer.start()
        changedTimer.start()
    }

    function initParent(decodeName){
        for(let k in decodeJson){
            if(decodeJson[k]["name"].toString().toLowerCase()===decodeName.toLowerCase())
            {
                let ret=true;
                if(!decodeJson[k]["first"])
                    ret=initParent(decodeJson[k]["parent"]);
                if(ret)
                    ret=selectDecode(decodeJson[k]["name"],decodeJson[k]["first"]);
                return ret;
            }
        }
        return false;
    }

    Timer{
        property bool isInit: false
        id: positionChangedTimer
        interval: 50
        onTriggered: {
            window.y=screen.bottom-screen.height/2-window.height/2
            window.x=screen.right-screen.width/2-window.width/2
            if(!isInit)
                close();
        }
    }

    //    //@----disable-check M16
    //    onClosing: function(closeevent){
    //        dataSend("123456");
    //    }

    //阴影组件
    Item{
        anchors.fill: parent

        Image {
            height: 15
            source: "../../resource/DropShadow/Top.png"
            fillMode: Image.Stretch
            anchors{
                left: dropShadowTopLeft.right
                right: dropShadowTopRight.left
                top: parent.top
            }
        }

        Image {
            id: dropShadowTopLeft
            width: 24
            height: 15
            source: "../../resource/DropShadow/TopLeft.png"
            anchors{
                left: parent.left
                top: parent.top
            }
        }

        Image {
            id: dropShadowTopRight
            width: 24
            height: 15
            source: "../../resource/DropShadow/TopRight.png"
            anchors{
                right: parent.right
                top: parent.top
            }
        }

        Image {
            width: 24
            source: "../../resource/DropShadow/Left.png"
            fillMode: Image.Stretch
            anchors{
                left: parent.left
                top: dropShadowTopLeft.bottom
                bottom: dropShadowLeftBottom.top
            }
        }

        Image {
            width: 24
            source: "../../resource/DropShadow/Right.png"
            fillMode: Image.Stretch
            anchors{
                right: parent.right
                top: dropShadowTopRight.bottom
                bottom: dropShadowRightBottom.top
            }
        }

        Image {
            height: 15
            source: "../../resource/DropShadow/Bottom.png"
            fillMode: Image.Stretch
            anchors{
                left: dropShadowLeftBottom.right
                right: dropShadowRightBottom.left
                bottom: parent.bottom
            }
        }

        Image {
            id: dropShadowLeftBottom
            width: 24
            height: 15
            source: "../../resource/DropShadow/LeftBottom.png"
            anchors{
                left: parent.left
                bottom: parent.bottom
            }
        }

        Image {
            id: dropShadowRightBottom
            width: 24
            height: 15
            source: "../../resource/DropShadow/RightBottom.png"
            anchors{
                right: parent.right
                bottom: parent.bottom
            }
        }
    }

    Rectangle{
        id: rootRectangle
        width: contentColumn.width
        height: contentColumn.height+150
        color: Config.backgroundColor
        radius: 10
        anchors{
            top: parent.top
            topMargin: 6
            left: parent.left
            leftMargin: 6
        }
        Item{
            id: titleItem
            width: parent.width
            height: 38
            Text {
                text: qsTr("协议设置")+(titleName===""?"":" （"+titleName+"）")
                font.pixelSize: 18
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
                onPressed: window.close()
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

        Column{
            id: contentColumn
            width: decodeListView.width+20
            height: decodeListView.height+1
            spacing: 10
            anchors{
                top: titleSplit.bottom
                topMargin: 10
                left: parent.left
                leftMargin: 20
            }
            Column{
                height: contentColumn.height
                width: decodeListView.width
                spacing: 0

                DecodeListView{
                    id: decodeListView
                    height: contentHeight>400?400:contentHeight
                    width: contentWidth
                    model: ListModel{
                        id: decodeViewModel
                    }
                    channels_: channels
                    onDataChanged: isChanged=true;
                }

                Row{
                    height: 1
                    width: parent.width-20
                    spacing: 4
                    clip: true
                    Repeater{
                        model: parent.width/10+1
                        Rectangle{
                            width: 6
                            height: 1
                            color: Config.lineColor
                        }
                    }
                }
            }
        }

        Item{
            id: secondDecodeItem
            width: parent.width-40
            height: 30
            anchors{
                top: contentColumn.bottom
                topMargin: 10
                left: parent.left
                leftMargin: 20
            }
            Row{
                width: parent.width
                spacing: 0
                Row{
                    width: 200
                    spacing: 10
                    QComboBox{
                        id: vernierStartComboBox
                        width: 80
                        model: ListModel{
                            id: vernierStartListModel
                            ListElement { showText: "Start"; cost: -2; id_: -2 }
                            ListElement { showText: "End"; cost: -1; id_: -1 }
                        }
                        onCurrentIndexChanged: isChanged=true;
                    }
                    Text {
                        text: "-"
                        anchors.verticalCenter: parent.verticalCenter
                        color: Config.textColor
                    }
                    QComboBox{
                        id: vernierEndComboBox
                        width: 80
                        model: ListModel{
                            id: vernierEndListModel
                            ListElement { showText: "Start"; cost: -2 }
                            ListElement { showText: "End"; cost: -1 }
                        }
                        onCurrentIndexChanged: isChanged=true;
                    }
                }
                Text {
                    text: qsTr("通道存在冲突")
                    font.pixelSize: 12
                    color: Config.red
                    anchors.verticalCenter: parent.verticalCenter
                    visible: showClashTips
                }
                Text {
                    text: qsTr("请选择通道后再试")
                    font.pixelSize: 12
                    color: Config.red
                    anchors.verticalCenter: parent.verticalCenter
                    visible: showSelectChannel
                }
            }
        }

        Rectangle{
            id: contentSplit
            height: 1
            width: parent.width
            anchors{
                top: secondDecodeItem.bottom
                topMargin: 10
            }
            color: Config.lineColor
        }

        Item{
            width: parent.width
            height: 40
            anchors.top: contentSplit.bottom
            Row{
                spacing: 10
                anchors{
                    right: parent.right
                    rightMargin: 20
                    verticalCenter: parent.verticalCenter
                }
                TextButton{
                    text: qsTr("取消")
                    width: 46
                    height: 30
                    onClicked: window.close();
                }

                TextButton{
                    text: qsTr("确定")
                    width: 46
                    height: 30
                    backgroundColor: Config.mouseCheckColor
                    backgroundEnterColor: backgroundColor
                    backgroundPressedColor: backgroundColor
                    textColor: "white"
                    textEnterColor: textColor
                    onPressed: {
                        let ret=true;
                        if((initJson&&isChanged)||!initJson)
                            ret=sendDecodeData();
                        if(ret)
                            window.close();
                    }
                }
            }
        }
    }

    function sendDecodeData(){
        var decodeData=decodeListView.outJson;
        let isSelect=false;
        showSelectChannel=false;
        showRequiredTips=false;
        for(let i in decodeData){
            if(decodeData[i]["channels"]){
                for(let ii in decodeData[i]["channels"]){
                    if(decodeData[i]["channels"][ii]["value"]==="-"){
                        showRequiredTips=true;
                        return false;
                    }else
                        isSelect=true;
                }
            }
            if(decodeData[i]["opt_channels"]){
                for(let ii in decodeData[i]["opt_channels"]){
                    if(decodeData[i]["opt_channels"][ii]["value"]!=="-")
                        isSelect=true;
                }
            }
        }
        if(!isSelect){
            showSelectChannel=true;
            return;
        }
        let decodeName=decodeData["main"]["first"];
        if(decodeData["main"]["second"] && decodeData["main"]["second"].length>0)
            decodeName=decodeData["main"]["second"][decodeData["main"]["second"].length-1];

        if(!decodeData["main"]["height"] && Setting.decodeConfig[decodeName] && Setting.decodeConfig[decodeName]["height"])
            decodeData["main"]["height"]=Setting.decodeConfig[decodeName]["height"];

        if(!decodeData["main"]["isLockRow"] && Setting.decodeConfig[decodeName] && Setting.decodeConfig[decodeName]["isLockRow"])
            decodeData["main"]["isLockRow"]=Setting.decodeConfig[decodeName]["isLockRow"];

        decodeData["main"]["start"]=vernierStartComboBox.currentModelChildren["cost"].toString();
        decodeData["main"]["end"]=vernierEndComboBox.currentModelChildren["cost"].toString();
        decodeData["main"]["startName"]=vernierStartComboBox.currentModelChildren["showText"];
        decodeData["main"]["endName"]=vernierEndComboBox.currentModelChildren["showText"];
        decodeData["main"]["startID"]=vernierStartComboBox.currentModelChildren["id_"];
        decodeData["main"]["endID"]=vernierEndComboBox.currentModelChildren["id_"];
        dataSend(decodeData)
        return true;
    }

    function selectDecode(name,isFirst){
        var selectJson;
        for(var i in decodeJson){
            if(decodeJson[i]["name"]===name)
            {
                selectJson=decodeJson[i];
                break;
            }
        }
        if(selectJson){
            if(isFirst)
            {
                decodeViewModel.clear();
                decodeListView.firstDecode=selectJson["name"];
            }
            else{
                //过滤重复协议
                for(var j=0;j<decodeViewModel.count;j++){
                    if(decodeViewModel.get(j)["name"]===name)
                        return
                }
            }
            selectJson["main"]={"isSingle":false};
            decodeViewModel.append(selectJson);
            return true;
        }
        return false;
    }

    function selectInitDecode(decodeName,dataJson){
        var selectJson;
        for(var i in decodeJson){
            if(decodeJson[i]["name"]===decodeName)
            {
                selectJson=decodeJson[i];
                break;
            }
        }
        if(selectJson){
            var arr,arr2;
            var row="annotation_rows";
            var jDara=dataJson[decodeName][row];
            selectJson["main"]={"isSingle":dataJson[decodeName]["main"]["isSingle"]};
            for(arr in jDara){
                if(selectJson[row].length===0){
                    let js={"id":jDara[arr]["id"],"value":jDara[arr]["isShow"]}
                    selectJson[row].push(js);
                }else{
                    for(arr2 in selectJson[row]){
                        if(selectJson[row][arr2]["id"]===jDara[arr]["id"]){
                            selectJson[row][arr2]["value"]=jDara[arr]["isShow"];
                            break;
                        }
                    }
                }
            }
            row="channels";
            jDara=dataJson[decodeName][row];
            for(arr in jDara){
                for(arr2 in selectJson[row]){
                    if(selectJson[row][arr2]["id"]===jDara[arr]["id"]){
                        selectJson[row][arr2]["def"]=jDara[arr]["value"];
                        break;
                    }
                }
            }

            row="opt_channels";
            jDara=dataJson[decodeName][row];
            for(arr in jDara){
                for(arr2 in selectJson[row]){
                    if(selectJson[row][arr2]["id"]===jDara[arr]["id"]){
                        selectJson[row][arr2]["def"]=jDara[arr]["value"];
                        break;
                    }
                }
            }

            row="options";
            jDara=dataJson[decodeName][row];
            for(arr in jDara){
                for(arr2 in selectJson[row]){
                    if(selectJson[row][arr2]["id"]===jDara[arr]["id"]){
                        selectJson[row][arr2]["def"]=jDara[arr]["value"];
                        break;
                    }
                }
            }
            decodeViewModel.append(selectJson);
        }
    }
}
