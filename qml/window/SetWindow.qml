import QtQuick 2.11
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import "../config"
import "../style"

Window {
    signal dataSend(var dataJson)
    property var screen
    property var dataJson
    property var root
    id: window
    visible: true
    modality: Qt.WindowModal
    flags: Qt.FramelessWindowHint
    color: "#00000000"
    width: dropShadow.width
    height: dropShadow.height

    Connections{
        target: Signal
        function onEscKeyPress()
        {
            window.close();
        }
    }

    Component.onCompleted: {
        Config.isSetModel=true;
        //常规
        set.collectZoom=Setting.collectZoom;
        set.jumpZoom=Setting.jumpZoom;
        set.closeHint=Setting.closeHint;
        set.viewToolPopup=Setting.viewToolPopup;
        set.componentWheelChanged=Setting.componentWheelChanged;
        set.showFPS=Setting.showFPS;
        set.isMouseMeasure=Setting.isMouseMeasure;
        set.isLinuxMemoryLimit=Setting.isLinuxMemoryLimit;

        //样式
        set.mouseMeasureBackColor=Setting.mouseMeasureBackColor;
        set.mouseMeasureTextColor=Setting.mouseMeasureTextColor;

        //快捷键
        keyModel.append({"showText":qsTr("开始采集"),"id":Config.ShortcutKey.StartCollecting,"key":Setting.startCollecting,"recodeKey":Setting.startCollecting,"isClash":false,"isOK":true});
        keyModel.append({"showText":qsTr("停止采集"),"id":Config.ShortcutKey.StopCollecting,"key":Setting.stopCollecting,"recodeKey":Setting.stopCollecting,"isClash":false,"isOK":true});
        keyModel.append({"showText":qsTr("上一标签"),"id":Config.ShortcutKey.SwitchVernierUp,"key":Setting.switchVernierUp,"recodeKey":Setting.switchVernierUp,"isClash":false,"isOK":true});
        keyModel.append({"showText":qsTr("下一标签"),"id":Config.ShortcutKey.SwitchVernierDown,"key":Setting.switchVernierDown,"recodeKey":Setting.switchVernierDown,"isClash":false,"isOK":true});
        keyModel.append({"showText":qsTr("参数测量"),"id":Config.ShortcutKey.ParameterMeasure,"key":Setting.parameterMeasure,"recodeKey":Setting.parameterMeasure,"isClash":false,"isOK":true});
        keyModel.append({"showText":qsTr("添加标签"),"id":Config.ShortcutKey.VernierCreate,"key":Setting.vernierCreate,"recodeKey":Setting.vernierCreate,"isClash":false,"isOK":true});
        keyModel.append({"showText":qsTr("上一页"),"id":Config.ShortcutKey.SwitchPageUp,"key":Setting.switchPageUp,"recodeKey":Setting.switchPageUp,"isClash":false,"isOK":true});
        keyModel.append({"showText":qsTr("下一页"),"id":Config.ShortcutKey.SwitchPageDown,"key":Setting.switchPageDown,"recodeKey":Setting.switchPageDown,"isClash":false,"isOK":true});
        keyModel.append({"showText":qsTr("关闭会话"),"id":Config.ShortcutKey.CloseSession,"key":Setting.closeSession,"recodeKey":Setting.closeSession,"isClash":false,"isOK":true});
        keyModel.append({"showText":qsTr("跳转到零点"),"id":Config.ShortcutKey.JumpZero,"key":Setting.jumpZero,"recodeKey":Setting.jumpZero,"isClash":false,"isOK":true});
        keyModel.append({"showText":qsTr("放大"),"id":Config.ShortcutKey.ZoomIn,"key":Setting.zoomIn,"recodeKey":Setting.zoomIn,"isClash":false,"isOK":true});
        keyModel.append({"showText":qsTr("缩小"),"id":Config.ShortcutKey.ZoomOut,"key":Setting.zoomOut,"recodeKey":Setting.zoomOut,"isClash":false,"isOK":true});
        keyModel.append({"showText":qsTr("缩放至整屏"),"id":Config.ShortcutKey.ZoomFull,"key":Setting.zoomFull,"recodeKey":Setting.zoomFull,"isClash":false,"isOK":true});
        keyModel.append({"showText":qsTr("保存"),"id":Config.ShortcutKey.Save,"key":Setting.saveFile,"recodeKey":Setting.saveFile,"isClash":false,"isOK":true});
        keyModel.append({"showText":qsTr("另存为"),"id":Config.ShortcutKey.SaveAs,"key":Setting.saveAs,"recodeKey":Setting.saveAs,"isClash":false,"isOK":true});
        keyModel.append({"showText":qsTr("导出"),"id":Config.ShortcutKey.ExportFile,"key":Setting.exportFile,"recodeKey":Setting.exportFile,"isClash":false,"isOK":true});
        keyModel.append({"showText":qsTr("设备配置"),"id":Config.ShortcutKey.DeviceConfig,"key":Setting.deviceConfig,"recodeKey":Setting.deviceConfig,"isClash":false,"isOK":true});
        keyModel.append({"showText":qsTr("协议解码"),"id":Config.ShortcutKey.ProtocolDecode,"key":Setting.protocolDecode,"recodeKey":Setting.protocolDecode,"isClash":false,"isOK":true});
        keyModel.append({"showText":qsTr("标签测量"),"id":Config.ShortcutKey.LabelMeasurement,"key":Setting.labelMeasurement,"recodeKey":Setting.labelMeasurement,"isClash":false,"isOK":true});
        keyModel.append({"showText":qsTr("数据搜索"),"id":Config.ShortcutKey.DataSearch,"key":Setting.dataSearch,"recodeKey":Setting.dataSearch,"isClash":false,"isOK":true});

        //decode
        decodeLogLevel.currentIndex=Setting.decodeLogLevel;

        checkClash();
        mouseMeasureBackColorButton.setColor(set.mouseMeasureBackColor);
        mouseMeasureTextColorButton.setColor(set.mouseMeasureTextColor);
    }

    function checkClash(){
        keysListView.isClash=false;
        for(let i=0;i<keyModel.count;i++){
            let item=keyModel.get(i);
            let isClash=false;
            if(!item.isOK)
            {
                item.isClash=false;
                item.isOK=true;
                keysListView.changedData(i,"");
                continue;
            }
            for(let ii=0;ii<keyModel.count;ii++){
                let item2=keyModel.get(ii);
                if(i!==ii && item.key===item2.key && item.key!==""){
                    item.isClash=true;
                    item2.isClash=true;
                    isClash=true;
                    keysListView.isClash=true;
                    break;
                }
            }
            if(!isClash)
                item.isClash=false;
        }
    }

    function getInitialValue(id){
        switch(id){
        case Config.ShortcutKey.StartCollecting:
            return "F1";
        case Config.ShortcutKey.StopCollecting:
            return "F2";
        case Config.ShortcutKey.SwitchVernierUp:
            return "F3";
        case Config.ShortcutKey.SwitchVernierDown:
            return "F4";
        case Config.ShortcutKey.ParameterMeasure:
            return "Ctrl+G";
        case Config.ShortcutKey.VernierCreate:
            return "Ctrl+H";
        case Config.ShortcutKey.SwitchPageUp:
            return "F5";
        case Config.ShortcutKey.SwitchPageDown:
            return "F6";
        case Config.ShortcutKey.JumpZero:
            return "F7";
        case Config.ShortcutKey.ZoomIn:
            return "F8";
        case Config.ShortcutKey.ZoomOut:
            return "F9";
        case Config.ShortcutKey.ZoomFull:
            return "F10";
        case Config.ShortcutKey.Save:
            return "Ctrl+S";
        case Config.ShortcutKey.SaveAs:
            return "Ctrl+Shift+S";
        case Config.ShortcutKey.ExportFile:
            return "Ctrl+E";
        case Config.ShortcutKey.DeviceConfig:
            return "Ctrl+1";
        case Config.ShortcutKey.ProtocolDecode:
            return "Ctrl+2";
        case Config.ShortcutKey.LabelMeasurement:
            return "Ctrl+3";
        case Config.ShortcutKey.DataSearch:
            return "Ctrl+F";
        case Config.ShortcutKey.CloseSession:
            return "Ctrl+W";
        }
        return "";
    }

    QtObject{
        id: set
        //选项
        property bool collectZoom: false            //采集完成后是否缩放至全屏
        property bool jumpZoom: false               //跳转功能是否自适应缩放界面
        property bool closeHint: false              //关闭软件是否提示保存数据
        property bool viewToolPopup: false          //视图工具悬浮窗
        property bool componentWheelChanged: false  //组件滚动修改属性
        property bool showFPS: false                //显示FPS
        property bool isMouseMeasure: true          //是否使用鼠标测量
        property bool isLinuxMemoryLimit: true      //是否限制ubuntu内存使用

        //样式
        property color mouseMeasureBackColor: "#ddbcedff"//鼠标悬浮窗背景色
        property color mouseMeasureTextColor: "#383838"//鼠标悬浮窗文字色
    }

    //    //@----disable-check M16
    //    onClosing: function(closeevent){
    //        dataSend("123456");
    //    }

    function setShowEditText(json){
        var showText='<style type="text/css">body{line-height:1.2}</style><body>';
        for(var i=0;i<json["count"];i++){
            showText+='<p><font color="#808080">v'+json["version"][i]+"&nbsp;&nbsp;"+json["date"][i]+'</font><br/>';
            showText+='<U>'+json["text"][i].toString().replace(new RegExp("\r\n", "g"),"<br/>")+'</U>';
            if(i+1<json["count"])
                showText+="<br/>";
        }
        showText+="</body>";
        showEdit.showText=showText;
        downloadProgressColumn.visible=false;
    }

    function resetSet(){
        set.collectZoom=true;
        set.jumpZoom=true;
        set.closeHint=true;
        set.viewToolPopup=true;
        set.componentWheelChanged=true;
        set.showFPS=false;
        set.isMouseMeasure=true;
        set.isLinuxMemoryLimit=true;
    }

    function confirm(){
        //选项
        Setting.collectZoom=set.collectZoom;
        Setting.jumpZoom=set.jumpZoom;
        Setting.closeHint=set.closeHint;
        Setting.viewToolPopup=set.viewToolPopup;
        Setting.componentWheelChanged=set.componentWheelChanged;
        Setting.showFPS=set.showFPS;
        Setting.isMouseMeasure=set.isMouseMeasure;
        Setting.isLinuxMemoryLimit=set.isLinuxMemoryLimit;

        //样式
        Setting.mouseMeasureBackColor=set.mouseMeasureBackColor;
        Setting.mouseMeasureTextColor=set.mouseMeasureTextColor;

        //快捷键
        for(let i=0;i<keyModel.count;i++){
            let item=keyModel.get(i);
            switch(item.id){
            case Config.ShortcutKey.StartCollecting:
                Setting.startCollecting=(item.isClash||!item.isOK)?"":item.key;
                break;
            case Config.ShortcutKey.StopCollecting:
                Setting.stopCollecting=(item.isClash||!item.isOK)?"":item.key;
                break;
            case Config.ShortcutKey.SwitchVernierUp:
                Setting.switchVernierUp=(item.isClash||!item.isOK)?"":item.key;
                break;
            case Config.ShortcutKey.SwitchVernierDown:
                Setting.switchVernierDown=(item.isClash||!item.isOK)?"":item.key;
                break;
            case Config.ShortcutKey.ParameterMeasure:
                Setting.parameterMeasure=(item.isClash||!item.isOK)?"":item.key;
                break;
            case Config.ShortcutKey.VernierCreate:
                Setting.vernierCreate=(item.isClash||!item.isOK)?"":item.key;
                break;
            case Config.ShortcutKey.SwitchPageUp:
                Setting.switchPageUp=(item.isClash||!item.isOK)?"":item.key;
                break;
            case Config.ShortcutKey.SwitchPageDown:
                Setting.switchPageDown=(item.isClash||!item.isOK)?"":item.key;
                break;
            case Config.ShortcutKey.JumpZero:
                Setting.jumpZero=(item.isClash||!item.isOK)?"":item.key;
                break;
            case Config.ShortcutKey.ZoomIn:
                Setting.zoomIn=(item.isClash||!item.isOK)?"":item.key;
                break;
            case Config.ShortcutKey.ZoomOut:
                Setting.zoomOut=(item.isClash||!item.isOK)?"":item.key;
                break;
            case Config.ShortcutKey.ZoomFull:
                Setting.zoomFull=(item.isClash||!item.isOK)?"":item.key;
                break;
            case Config.ShortcutKey.Save:
                Setting.saveFile=(item.isClash||!item.isOK)?"":item.key;
                break;
            case Config.ShortcutKey.SaveAs:
                Setting.saveAs=(item.isClash||!item.isOK)?"":item.key;
                break;
            case Config.ShortcutKey.ExportFile:
                Setting.exportFile=(item.isClash||!item.isOK)?"":item.key;
                break;
            case Config.ShortcutKey.DeviceConfig:
                Setting.deviceConfig=(item.isClash||!item.isOK)?"":item.key;
                break;
            case Config.ShortcutKey.ProtocolDecode:
                Setting.protocolDecode=(item.isClash||!item.isOK)?"":item.key;
                break;
            case Config.ShortcutKey.LabelMeasurement:
                Setting.labelMeasurement=(item.isClash||!item.isOK)?"":item.key;
                break;
            case Config.ShortcutKey.DataSearch:
                Setting.dataSearch=(item.isClash||!item.isOK)?"":item.key;
                break;
            case Config.ShortcutKey.CloseSession:
                Setting.closeSession=(item.isClash||!item.isOK)?"":item.key;
                break;
            }
        }

        //decode
        Setting.decodeLogLevel=decodeLogLevel.currentIndex;
        root.setDecodeLogLevel(Setting.decodeLogLevel);

        Setting.save();
    }

    Connections{
        target: root

        function onSendDownloadSchedule(schedule, type, index){
            downloadProgressBar.value=schedule;
            downloadProgressColumn.visible=true;
        }
    }

    Image {
        id: dropShadow
        width: 520
        height: 466
        source: "../../resource/image/SetWindowDropShadow.png"
        Rectangle{
            id: rootRectangle
            width: 504
            height: 450
            color: Config.backgroundColor
            radius: 10
            anchors{
                top: parent.top
                topMargin: 6
                horizontalCenter: parent.horizontalCenter
            }
            Item{
                id: titleItem
                width: parent.width
                height: 40
                Text {
                    text: qsTr("设置")
                    font.pixelSize: 14
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
                    onPressed: close_();
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

            Item{
                id: contentColumn
                width: parent.width-20
                anchors{
                    top: titleSplit.bottom
                    topMargin: 10
                    bottom: footerItem.top
                    bottomMargin: 10
                    left: parent.left
                    leftMargin: 10
                }

                Rectangle{
                    id: selectListRectangle
                    width: 86
                    height: parent.height
                    color: rootRectangle.color
                    border{
                        width: 1
                        color: Config.lineColor
                    }
                    ListView{
                        id: selectList
                        anchors{
                            left: parent.left
                            right: parent.right
                            top: parent.top
                            bottom: parent.bottom
                            margins: 1
                        }
                        flickableDirection: Flickable.AutoFlickDirection
                        boundsBehavior: Flickable.StopAtBounds
                        clip: true
                        model: ListModel{
                            ListElement { showText: qsTr("常规"); }
                            ListElement { showText: qsTr("样式"); }
                            ListElement { showText: qsTr("快捷键"); }
                            ListElement { showText: qsTr("协议解码"); }
                        }
                        delegate: Rectangle{
                            width: selectList.width
                            height: 24
                            color: index===selectList.currentIndex?Config.mouseCheckColor:"transparent"
                            Row{
                                height: parent.height
                                width: parent.width
                                spacing: 3
                                anchors{
                                    left: parent.left
                                    leftMargin: 10
                                }
                                Text {
                                    text: showText
                                    font.pixelSize: 14
                                    color: index===selectList.currentIndex?"white":Config.textColor
                                    anchors.verticalCenter: parent.verticalCenter
                                }
                            }

                            MouseArea{
                                id: mouseArea
                                anchors.fill: parent
                                hoverEnabled: true
                                onPressed: selectList.currentIndex=index
                            }
                        }
                    }
                }

                StackLayout {
                    id: stackLayout
                    currentIndex: selectList.currentIndex
                    anchors{
                        left: selectListRectangle.right
                        leftMargin: 10
                        top: parent.top
                        right: parent.right
                        bottom: parent.bottom
                    }
                    clip: true
                    //常规
                    Column{
                        width: parent.width
                        height: parent.height
                        spacing: 10
                        Item {
                            height: 19
                            width: parent.width
                            Text {
                                text: qsTr("选项")
                                color: Config.textColor
                                font.pixelSize: 12
                                anchors.bottom: parent.bottom
                                verticalAlignment: Text.AlignBottom
                            }
                        }
                        Rectangle{
                            width: parent.width
                            height: 24*(windowType===2?8:7)
                            color: rootRectangle.color
                            border{
                                width: 1
                                color: Config.lineColor
                            }
                            Column{
                                width: parent.width
                                height: parent.height
                                spacing: 8
                                anchors{
                                    top: parent.top
                                    topMargin: 8
                                    left: parent.left
                                    leftMargin: 10
                                }
                                QCheckBox{
                                    buttonText: qsTr("采集完成后是否缩放至全屏")
                                    fontPixelSize: 14
                                    textVerticalCenterOffset: -2
                                    autoCheck: false
                                    isCheck: set.collectZoom
                                    onClicked_: set.collectZoom=!isCheck
                                }
                                QCheckBox{
                                    buttonText: qsTr("跳转功能是否自适应缩放界面")
                                    fontPixelSize: 14
                                    textVerticalCenterOffset: -2
                                    autoCheck: false
                                    isCheck: set.jumpZoom
                                    onClicked_: set.jumpZoom=!isCheck
                                }
                                QCheckBox{
                                    buttonText: qsTr("关闭软件是否提示保存数据")
                                    fontPixelSize: 14
                                    textVerticalCenterOffset: -2
                                    autoCheck: false
                                    isCheck: set.closeHint
                                    onClicked_: set.closeHint=!isCheck
                                }
                                QCheckBox{
                                    buttonText: qsTr("视图悬浮窗")
                                    fontPixelSize: 14
                                    textVerticalCenterOffset: -2
                                    autoCheck: false
                                    isCheck: set.viewToolPopup
                                    onClicked_: set.viewToolPopup=!isCheck
                                }
                                QCheckBox{
                                    buttonText: qsTr("组件滚轮修改属性")
                                    fontPixelSize: 14
                                    textVerticalCenterOffset: -2
                                    autoCheck: false
                                    isCheck: set.componentWheelChanged
                                    onClicked_: set.componentWheelChanged=!isCheck
                                }
                                QCheckBox{
                                    buttonText: qsTr("显示帧率")
                                    fontPixelSize: 14
                                    textVerticalCenterOffset: -2
                                    autoCheck: false
                                    isCheck: set.showFPS
                                    onClicked_: set.showFPS=!isCheck
                                }
                                QCheckBox{
                                    buttonText: qsTr("显示鼠标测量浮窗")
                                    fontPixelSize: 14
                                    textVerticalCenterOffset: -2
                                    autoCheck: false
                                    isCheck: set.isMouseMeasure
                                    onClicked_: set.isMouseMeasure=!isCheck
                                }
                                QCheckBox{
                                    buttonText: qsTr("Ubuntu内存限制")
                                    fontPixelSize: 14
                                    textVerticalCenterOffset: -2
                                    autoCheck: false
                                    visible: windowType===2
                                    isCheck: set.isLinuxMemoryLimit
                                    onClicked_: set.isLinuxMemoryLimit=!isCheck
                                    QToolTip{
                                        parent: parent
                                        isShow: parent.containsMouse
                                        direction: 2
                                        showText: qsTr("如果关闭内存限制，则不检查内存余量\n如果内存不足则会导致系统卡死等问题。")
                                    }
                                }
                            }
                        }
                        TextButton{
                            text: qsTr("复位初始设置")
                            height: 22
                            textColor: "white"
                            textEnterColor: textColor
                            textDisableColor: Config.setKeysButtonDisableText
                            backgroundColor: Config.borderLineColor
                            backgroundEnterColor: Config.mouseCheckColor
                            backgroundPressedColor: backgroundEnterColor
                            backgroundDisableColor: Config.setKeysInputBackground
                            onPressed: resetSet();
                        }
                    }
                    //样式
                    Column{
                        width: parent.width
                        height: parent.height
                        spacing: 10
                        Item {
                            height: 19
                            width: parent.width
                            Text {
                                text: qsTr("鼠标测量浮窗")
                                color: Config.textColor
                                font.pixelSize: 12
                                anchors.bottom: parent.bottom
                                verticalAlignment: Text.AlignBottom
                            }
                        }
                        Rectangle{
                            width: parent.width
                            height: 1
                            color: Config.lineColor
                        }
                        Row{
                            width: parent.width
                            height: 35
                            spacing: 10
                            Row{
                                spacing: 4
                                Text {
                                    text: qsTr("背景颜色：")
                                    color: Config.textColor
                                    font.pixelSize: 12
                                    anchors.verticalCenter: parent.verticalCenter
                                }
                                ColorPickerButton{
                                    id: mouseMeasureBackColorButton
                                    isAlpha: true
                                    onPopupClose: set.mouseMeasureBackColor=currentColor;
                                    popupX: -20
                                    popupY: 38
                                }
                            }
                            Row{
                                spacing: 4
                                Text {
                                    text: qsTr("文字颜色：")
                                    color: Config.textColor
                                    font.pixelSize: 12
                                    anchors.verticalCenter: parent.verticalCenter
                                }
                                ColorPickerButton{
                                    id: mouseMeasureTextColorButton
                                    isAlpha: true
                                    onPopupClose: set.mouseMeasureTextColor=currentColor;
                                    popupX: -119
                                    popupY: 38
                                }
                            }
                        }
                    }
                    //快捷键
                    Item{
                        width: parent.width
                        height: parent.height
                        MouseArea{
                            anchors.fill: parent
                            acceptedButtons: Qt.LeftButton //只处理鼠标左键
                            onPressed: parent.focus=true;
                            z:1
                        }
                        GridLayout{
                            width: parent.width
                            height: parent.height
                            rows: 3
                            columns: 3
                            rowSpacing: 10
                            columnSpacing: 5
                            z:5
                            Item{
                                height: 16
                                width: 150
                                Text {
                                    text: qsTr("操作命令")
                                    font.pixelSize: 14
                                    color: Config.textColor
                                }
                            }
                            Item{
                                height: 16
                                Layout.column: 1
                                Layout.fillWidth: true
                                Text {
                                    text: qsTr("快捷键")
                                    font.pixelSize: 14
                                    color: Config.textColor
                                }
                            }
                            Rectangle{
                                Layout.row: 1
                                Layout.columnSpan: 2
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                border{
                                    width: 1
                                    color: Config.lineColor
                                }

                                ListView{
                                    property bool isClash: false
                                    property bool isEdit: false
                                    property bool isRestore: false
                                    property bool isInit: false
                                    property bool isDelete: false
                                    property int select: -1
                                    signal changedData(var index_, var value)
                                    id: keysListView
                                    width: parent.width-2
                                    height: parent.height-2
                                    flickableDirection: Flickable.AutoFlickDirection
                                    boundsBehavior: Flickable.StopAtBounds
                                    clip: true
                                    ScrollBar.vertical: QScrollBar{
                                        anchors{
                                            top: keysListView.top
                                            right: keysListView.right
                                        }
                                    }
                                    anchors{
                                        top: parent.top
                                        left: parent.left
                                        margins: 1
                                    }
                                    model: ListModel{
                                        id: keyModel
                                    }
                                    delegate: Rectangle{
                                        Connections{
                                            target: keysListView
                                            function onChangedData(index_, value)
                                            {
                                                if(index_===index)
                                                {
                                                    inputText.isSet=true;
                                                    inputText.showText=value;
                                                    key=value;
                                                    keysListView.isRestore=key!==recodeKey;
                                                    keysListView.isDelete=key!=="";
                                                    keysListView.isInit=key!==getInitialValue(id);
                                                    checkClash();
                                                }
                                            }
                                        }
                                        width: parent.width
                                        height: 22
                                        color: index===keysListView.select?Config.mouseCheckColor:Config.backgroundColor
                                        Rectangle{
                                            id: splicRectangle
                                            width: parent.width
                                            height: 1
                                            color: Config.lineColor
                                            visible: index!==0
                                        }

                                        Row{
                                            z:5
                                            width: parent.width-10
                                            height: parent.height
                                            anchors{
                                                left: parent.left
                                                leftMargin: 5
                                            }
                                            spacing: 0
                                            Text {
                                                id: textShow
                                                width: 146
                                                text: showText
                                                font.pixelSize: 13
                                                color: index===keysListView.select?"white":Config.textColor
                                                anchors.verticalCenter: parent.verticalCenter
                                            }
                                            QTextInput{
                                                property int count: 0
                                                property bool isSet: true
                                                property string recode
                                                id: inputText
                                                showText: key
                                                showBorder: false
                                                implicitHeight: 17
                                                implicitWidth: (nShowText.contentWidth<50?50:nShowText.contentWidth)+10
                                                keysIntercept: true
                                                radius: 0
                                                anchors{
                                                    verticalCenter: parent.verticalCenter
                                                    verticalCenterOffset: 1
                                                }
                                                backgroundColor: isFocus?Config.setKeysInputBackground:"transparent"
                                                textColor: index===keysListView.select&&!isFocus?"white":Config.subheadColor
                                                textInput.onTextChanged: {
                                                    if(isSet)
                                                    {
                                                        recode=showText;
                                                        isSet=false;
                                                    }else
                                                    {
                                                        isSet=true;
                                                        showText=recode;
                                                    }
                                                }
                                                onKeyPressed: {
                                                    count++;
                                                    let str="";
                                                    let btn=event.text;
                                                    if(event.modifiers&Qt.ControlModifier)
                                                        str+="Ctrl+";
                                                    if(event.modifiers&Qt.ShiftModifier)
                                                        str+="Shift+";
                                                    if(event.modifiers&Qt.AltModifier&&str!=="")
                                                        str+="Alt+";
                                                    if(event.modifiers&Qt.MetaModifier&&str!=="")
                                                        str+="Meta+";

                                                    if(event.key>64 && event.key<91)
                                                        btn=String.fromCharCode(event.key);
                                                    else{
                                                        let isSet=false;
                                                        switch(event.key){
                                                        case Qt.Key_Space:
                                                            btn="Space"; isSet=true; break;
                                                        case Qt.Key_Backspace:
                                                            btn=""; str=""; isSet=true; break;
                                                        case Qt.Key_ScrollLock:
                                                            btn="ScrollLock"; isSet=true; break;
                                                        case Qt.Key_Pause:
                                                            btn="Pause"; isSet=true; break;
                                                        case Qt.Key_Insert:
                                                            btn="Insert"; isSet=true; break;
                                                        case Qt.Key_Home:
                                                            btn="Home"; isSet=true; break;
                                                        case Qt.Key_PageUp:
                                                            btn="PageUp"; isSet=true; break;
                                                        case Qt.Key_Delete:
                                                            btn="Delete"; isSet=true; break;
                                                        case Qt.Key_End:
                                                            btn="End"; isSet=true; break;
                                                        case Qt.Key_PageDown:
                                                            btn="PageDown"; isSet=true; break;
                                                        case Qt.Key_Tab:
                                                        case Qt.Key_Backtab:
                                                            btn="Tab"; isSet=true; break;
                                                        case Qt.Key_BracketLeft:
                                                            btn="["; isSet=true; break;
                                                        case Qt.Key_BracketRight:
                                                            btn="]"; isSet=true; break;
                                                        case Qt.Key_Escape:
                                                            btn="Esc"; isSet=true; break;
                                                        }
                                                        if(!isSet){
                                                            for(let i=16777264;i<16777276;i++){
                                                                if(event.key===i)
                                                                {
                                                                    btn="F"+(13-(16777276-i));
                                                                    break;
                                                                }
                                                            }
                                                        }
                                                    }
                                                    if(showText!=str+btn){
                                                        isSet=true;
                                                        showText=str+btn;
                                                        if(event.key===Qt.Key_Control||event.key===Qt.Key_Alt||
                                                                event.key===Qt.Key_Shift||event.key===Qt.Key_Meta||btn==="")
                                                            isOK=false;
                                                        else
                                                        {
                                                            isOK=true;
                                                            key=showText;
                                                            keysListView.isRestore=key!==recodeKey;
                                                            keysListView.isDelete=key!=="";
                                                            keysListView.isInit=key!==getInitialValue(id);
                                                            checkClash();
                                                        }
                                                    }
                                                }
                                                onKeyReleased: {
                                                    count--;
                                                    if(count===0)
                                                        checkClash();
                                                }

                                                onIsFocusChanged: {
                                                    if(isFocus)
                                                    {
                                                        keysListView.select=index;
                                                        keysListView.isEdit=true;
                                                        keysListView.isRestore=key!==recodeKey;
                                                        keysListView.isDelete=key!=="";
                                                        keysListView.isInit=key!==getInitialValue(id);
                                                    }else if(keysListView.select===index){
                                                        count=0;
                                                        keysListView.isEdit=false;
                                                        keysListView.isRestore=key!==recodeKey;
                                                        keysListView.isDelete=key!=="";
                                                        keysListView.isInit=key!==getInitialValue(id);
                                                        checkClash();
                                                    }
                                                }
                                                Text {
                                                    id: nShowText
                                                    text: parent.showText
                                                    visible: false
                                                    font.pixelSize: parent.fontSize
                                                }
                                            }
                                            Item{
                                                width: 16
                                                height: parent.height
                                                Image{
                                                    width: 14
                                                    height: 12
                                                    fillMode: Image.PreserveAspectFit
                                                    source: "../../resource/icon/Warning_triangle_small.png"
                                                    visible: isClash
                                                    anchors{
                                                        right: parent.right
                                                        verticalCenter: parent.verticalCenter
                                                    }
                                                }
                                            }
                                        }
                                        MouseArea{
                                            anchors.fill: parent
                                            z:1
                                            onPressed: {
                                                keysListView.select=index
                                                textShow.focus=true
                                                keysListView.isEdit=false;
                                                keysListView.isRestore=key!==recodeKey;
                                                keysListView.isDelete=key!=="";
                                                keysListView.isInit=key!==getInitialValue(id);
                                            }
                                        }
                                    }
                                }
                            }
                            Column{
                                Layout.row: 1
                                Layout.column: 2
                                Layout.alignment: Qt.AlignTop
                                width: 82
                                spacing: 5
                                TextButton{
                                    text: qsTr("接受")
                                    textColor: "white"
                                    textEnterColor: textColor
                                    textDisableColor: Config.setKeysButtonDisableText
                                    enabled: keysListView.select!=-1 && keysListView.isEdit
                                    backgroundColor: Config.borderLineColor
                                    backgroundEnterColor: backgroundColor
                                    backgroundPressedColor: backgroundColor
                                    backgroundDisableColor: Config.setKeysInputBackground
                                    backgroundRectangle.radius: 0
                                    width: parent.width
                                    height: 16
                                }
                                TextButton{
                                    text: qsTr("还原")
                                    textColor: "white"
                                    textEnterColor: textColor
                                    textDisableColor: Config.setKeysButtonDisableText
                                    enabled: keysListView.select!=-1 && keysListView.isRestore
                                    backgroundColor: Config.borderLineColor
                                    backgroundEnterColor: backgroundColor
                                    backgroundPressedColor: backgroundColor
                                    backgroundDisableColor: Config.setKeysInputBackground
                                    backgroundRectangle.radius: 0
                                    width: parent.width
                                    height: 16
                                    onPressed: keysListView.changedData(keysListView.select, keyModel.get(keysListView.select).recodeKey)
                                }
                                TextButton{
                                    text: qsTr("恢复初始化")
                                    textColor: "white"
                                    textEnterColor: textColor
                                    textDisableColor: Config.setKeysButtonDisableText
                                    enabled: keysListView.select!=-1 && keysListView.isInit
                                    backgroundColor: Config.borderLineColor
                                    backgroundEnterColor: backgroundColor
                                    backgroundPressedColor: backgroundColor
                                    backgroundDisableColor: Config.setKeysInputBackground
                                    backgroundRectangle.radius: 0
                                    width: parent.width
                                    height: 16
                                    onPressed: keysListView.changedData(keysListView.select, getInitialValue(keyModel.get(keysListView.select).id))
                                }
                                TextButton{
                                    text: qsTr("删除快捷键")
                                    textColor: "white"
                                    textEnterColor: textColor
                                    textDisableColor: Config.setKeysButtonDisableText
                                    enabled: keysListView.select!=-1 && keysListView.isDelete
                                    backgroundColor: Config.borderLineColor
                                    backgroundEnterColor: backgroundColor
                                    backgroundPressedColor: backgroundColor
                                    backgroundDisableColor: Config.setKeysInputBackground
                                    backgroundRectangle.radius: 0
                                    width: parent.width
                                    height: 16
                                    onPressed: keysListView.changedData(keysListView.select,"")
                                }
                                Item{
                                    width: parent.width
                                    height: 11
                                    Rectangle{
                                        width: parent.width
                                        height: 1
                                        clip: true
                                        color: Config.lineColor
                                        anchors.centerIn: parent
                                    }
                                }
                                TextButton{
                                    text: qsTr("重置所有")
                                    textColor: "white"
                                    textEnterColor: textColor
                                    textDisableColor: Config.setKeysButtonDisableText
                                    enabled: true
                                    backgroundColor: Config.borderLineColor
                                    backgroundEnterColor: backgroundColor
                                    backgroundPressedColor: backgroundColor
                                    backgroundDisableColor: Config.setKeysInputBackground
                                    backgroundRectangle.radius: 0
                                    width: parent.width
                                    height: 16
                                    onPressed: {
                                        for(let i=0;i<keyModel.count;i++){
                                            let tmp=keyModel.get(i);
                                            keysListView.changedData(i, getInitialValue(keyModel.get(i).id))
                                        }
                                    }
                                }
                            }
                            Rectangle{
                                Layout.fillWidth: true
                                height: 36
                                Layout.row: 2
                                Layout.columnSpan: 3
                                color: keysListView.isClash?Config.background2Color:"transparent"
                                Row{
                                    visible: keysListView.isClash
                                    width: parent.width-5
                                    height: parent.height
                                    anchors{
                                        left: parent.left
                                        leftMargin: 5
                                    }

                                    Item {
                                        width: 22
                                        height: parent.height
                                        Image {
                                            width: 20
                                            height: 18
                                            fillMode: Image.PreserveAspectFit
                                            source: "../../resource/icon/Warning_triangle.png"
                                            anchors.verticalCenter: parent.verticalCenter
                                        }
                                    }
                                    Text {
                                        text: qsTr("冲突快捷键都将被忽略。")
                                        font.pixelSize: 12
                                        color: Config.textDisabledColor
                                        anchors.verticalCenter: parent.verticalCenter
                                    }
                                }
                            }
                        }
                    }
                    //协议解码
                    Item{
                        width: parent.width
                        height: parent.height
                        Column{
                            width: parent.width
                            height: parent.height
                            spacing: 10
                            Item {
                                height: 19
                                width: parent.width
                                Text {
                                    text: qsTr("选项")
                                    color: Config.textColor
                                    font.pixelSize: 12
                                    anchors.bottom: parent.bottom
                                    verticalAlignment: Text.AlignBottom
                                }
                            }
                            Rectangle{
                                width: parent.width
                                height: 1
                                color: Config.lineColor
                            }
                            Row{
                                spacing: 10
                                Row{
                                    spacing: 5
                                    Text {
                                        text: qsTr("解码器日志等级")
                                        font.pixelSize: 12
                                        color: Config.textColor
                                        anchors.verticalCenter: parent.verticalCenter
                                    }
                                    QComboBox{
                                        id: decodeLogLevel
                                        width: 90
                                        model: ListModel{
                                            ListElement { showText: "None"; cost: 0 }
                                            ListElement { showText: "Error"; cost: 1 }
                                            ListElement { showText: "Warning"; cost: 2 }
                                            ListElement { showText: "Info"; cost: 3 }
                                            ListElement { showText: "Debug"; cost: 4 }
                                            ListElement { showText: "Spew"; cost: 5 }
                                        }
                                    }
                                }
                            }
                            Item {
                                height: 19
                                width: parent.width
                                Text {
                                    text: qsTr("功能")
                                    color: Config.textColor
                                    font.pixelSize: 12
                                    anchors.bottom: parent.bottom
                                    verticalAlignment: Text.AlignBottom
                                }
                            }
                            Rectangle{
                                width: parent.width
                                height: 1
                                color: Config.lineColor
                            }
                            Row{
                                spacing: 10
                                TextButton{
                                    text: qsTr("重新加载协议库")
                                    height: 30
                                    textColor: "white"
                                    textEnterColor: textColor
                                    textDisableColor: Config.setKeysButtonDisableText
                                    backgroundColor: Config.borderLineColor
                                    backgroundEnterColor: Config.mouseCheckColor
                                    backgroundPressedColor: backgroundEnterColor
                                    backgroundDisableColor: Config.setKeysInputBackground
                                    onPressed: root.reloadDecoder(Setting.decodeLogLevel);
                                }
                                TextButton{
                                    text: qsTr("调试日志")
                                    height: 30
                                    textColor: "white"
                                    textEnterColor: textColor
                                    textDisableColor: Config.setKeysButtonDisableText
                                    backgroundColor: Config.borderLineColor
                                    backgroundEnterColor: Config.mouseCheckColor
                                    backgroundPressedColor: backgroundEnterColor
                                    backgroundDisableColor: Config.setKeysInputBackground
                                    onPressed: {
                                        root.showDecodeLogWindow();
                                        close_();
                                    }
                                }
                            }
                        }
                    }
                }
            }

            Item{
                id: footerItem
                width: parent.width
                height: 51
                anchors{
                    bottom: parent.bottom
                    left: parent.left
                }
                Rectangle{
                    width: parent.width
                    height: 1
                    color: Config.lineColor
                }
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
                        onClicked: close_();
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
                            confirm();
                            sendSetData();
                            close_();
                        }
                    }
                }
            }
        }
    }

    function sendSetData(){
        dataSend(dataJson)
    }

    function close_(){
        window.close();
    }

}
