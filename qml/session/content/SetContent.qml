import QtQuick 2.15
import QtQuick.Controls 2.5
import "../../config"
import "../../style"

Rectangle {
    color: Config.background2Color
    clip: true
    Component.onCompleted: {
        //默认全开所有通道
        if(sSettings.isFirst)
            setNumChannelState(sessionChannelCount_,true);

        con.recode=JSON.stringify(controller.getGlitchRemoval());

        switch(sSettings.channelHeightMultiple){
        case 1:
            channelSelectMenuPopup.selectIndex=11;
            break;
        case 2:
            channelSelectMenuPopup.selectIndex=12;
            break;
        case 4:
            channelSelectMenuPopup.selectIndex=13;
            break;
        case 8:
            channelSelectMenuPopup.selectIndex=14;
            break;
        }
        delayInit.start()
    }

    Timer{
        id: delayInit
        interval: 400
        onTriggered: {
            if(sConfig.isCloseNotDataChannels)
                sSignal.closeNotDataChannels();
            if(sSettings.glitchRemoval.length!==0){
                controller.setGlitchRemoval(sSettings.glitchRemoval);
                con.recode=JSON.stringify(sSettings.glitchRemoval);
            }
        }
    }

    Connections{
        target: sConfig
        function onSidebarContentTypeChanged(){
            if(sConfig.sidebarContentType===Config.SidebarContentType.Set)
                methodsRadioGroup.selectButton= sSettings.isInstantly?methodsRadioGroup.button3:sSettings.isOne?methodsRadioGroup.button1:methodsRadioGroup.button2;
        }

        function onIsLoadSettingChanged(){
            if(!sConfig.isLoadSetting){
                for(let i=0;i<sessionChannelCount_;i++)
                    channelRow.colorChanged(i);

                switch(sSettings.channelHeightMultiple){
                case 1:
                    channelSelectMenuPopup.selectIndex=11;
                    break;
                case 2:
                    channelSelectMenuPopup.selectIndex=12;
                    break;
                case 4:
                    channelSelectMenuPopup.selectIndex=13;
                    break;
                case 8:
                    channelSelectMenuPopup.selectIndex=14;
                    break;
                }
                timeComboBox.currentIndex=sSettings.settingData["selectTimeIndex"];
                hzComboBox.currentIndex=sSettings.settingData["selectHzIndex"];
                spinBox.value=sSettings.settingData["thresholdLevel"].toString();
                thresholdLevelComboBox.currentIndex=sSettings.settingData["selectThresholdLevelIndex"];
                triggerPositionSlider.value=sSettings.settingData["triggerPosition"];
                modelRadioGroup.selectButton=sSettings.settingData["isBuffer"]?modelRadioGroup.button1:modelRadioGroup.button2
                methodsRadioGroup.selectButton=sSettings.isInstantly?methodsRadioGroup.button3:sSettings.isOne?methodsRadioGroup.button1:methodsRadioGroup.button2
                intervalSlider.value=sSettings.settingData["intervalTime"];
                pwm0HzComboBox.currentIndex=sSettings.pwmData[0]["unit"];
                pwm0SpinBox.value=sSettings.pwmData[0]["hz"].toString();
                pwm0dutySpinBox.value=sSettings.pwmData[0]["duty"].toString();
                pwm1HzComboBox.currentIndex=sSettings.pwmData[1]["unit"];
                pwm1SpinBox.value=sSettings.pwmData[1]["hz"].toString();
                pwm1dutySpinBox.value=sSettings.pwmData[1]["duty"].toString();
                sSignal.channelRefresh(-1);
                resetHzComboBox();
            }
        }
    }
    Connections{
        target: sSignal

        function onCloseAllPopup(){
            channelSelectMenuPopup.close();
        }

        function onSessionSelect(isSelect){
            if(isSelect)
                setStateText();
        }

        function onCloseNotDataChannels(){
            setNumChannelState(16,true);
            for(var i=0;i<sessionChannelCount_;i++){
                if(controller.getChannelMaxSample(i)===0){
                    setChannelState(i,false);
                }
            }
        }

        function onChannelColorChanged(){
            for(let i=0;i<sessionChannelCount_;i++)
                channelRow.colorChanged(i);
        }

        function onRefreshMethodsRadioGroup(){
            methodsRadioGroup.selectButton=sSettings.isInstantly?methodsRadioGroup.button3:sSettings.isOne?methodsRadioGroup.button1:methodsRadioGroup.button2
        }
    }

    Connections{
        target: controller
        function onSendGlitchString(str){
            if(str===""){
                resetGlitchRemoval(false);
            }else{
                let js=JSON.parse(con.recode);
                let split=str.split(',');
                for(let i in split){
                    js[i]["num"]=parseInt(split[i]);
                    js[i]["enable"]=js[i]["num"]>0;
                }
                con.recode=JSON.stringify(js);
                controller.setGlitchRemoval(js);
            }
        }
    }

    Connections{
        target: root
        function onLanguageChanged(){
            setStateText();
            thresholdLevelComboBox.refreshText();
            methodsRadioGroup.selectButton=sSettings.isInstantly?methodsRadioGroup.button3:sSettings.isOne?methodsRadioGroup.button1:methodsRadioGroup.button2;
        }
    }

    Connections{
        property string recode
        id: con
        ignoreUnknownSignals: true
        function onDataSend(dataJson)
        {
            if(JSON.stringify(dataJson)!==con.recode)
            {
                controller.setGlitchRemoval(dataJson);
                con.recode=JSON.stringify(dataJson);
                sSettings.glitchRemoval=JSON.stringify(dataJson);
                sSettings.save();
            }
        }

        function onColorPickerDataSend(dataJson){
            for(let i=0;i<dataJson.length;i++)
                sSettings.channelsDataColor[i]=dataJson[i]["color"].toString();
            sSignal.channelColorChanged();
        }

        function onClosing(){
            Config.isSetModel=false;
        }
    }

    //重设毛刺过滤
    function resetGlitchRemoval(isSet){
        con.recode='[{"enable":false,"num":1},{"enable":false,"num":1},{"enable":false,"num":1},{"enable":false,"num":1},{"enable":false,"num":1},{"enable":false,"num":1},{"enable":false,"num":1},{"enable":false,"num":1},{"enable":false,"num":1},{"enable":false,"num":1},{"enable":false,"num":1},{"enable":false,"num":1},{"enable":false,"num":1},{"enable":false,"num":1},{"enable":false,"num":1},{"enable":false,"num":1}]'
        if(isSet)
            controller.setGlitchRemoval(JSON.parse(con.recode));
    }

    //设置通道状态
    function setNumChannelState(num,isOpen){
        for(var i in channelRow.children){
            if(typeof(channelRow.children[i].button)!="undefined")
            {
                if(parseInt(channelRow.children[i].button.text)===0)
                    channelRow.children[i].button.checked=true;
                else if(parseInt(channelRow.children[i].button.text)<num)
                    channelRow.children[i].button.checked=isOpen;
                else
                    channelRow.children[i].button.checked=!isOpen;
            }
        }
    }

    //设置单个通道状态
    function setChannelState(channelID_,isOpen){
        for(var i in channelRow.children){
            if(typeof(channelRow.children[i].button)!=="undefined")
            {
                if(parseInt(channelRow.children[i].button.text)===channelID_)
                {
                    channelRow.children[i].button.checked=isOpen;
                    return;
                }
            }
        }
    }

    Text {
        id: titleText
        text: qsTr("设备配置")
        font.pixelSize: 18
        color: Config.mouseCheckColor
        anchors{
            top: parent.top
            left: parent.left
        }
    }
    Text {
        id: channelText
        text: qsTr("通道选择")
        font.pixelSize: 16
        color: moreButton.enabled?Config.textColor:Config.textDisabledColor
        height: 20
        width: 240
        anchors{
            top: titleText.bottom
            left: parent.left
            topMargin: 20
        }
        ImageButton{
            id: moreButton
            anchors{
                right: parent.right
                verticalCenter: parent.verticalCenter
            }
            imageWidth: 2
            imageHeight: 12
            imageSource: "resource/icon/"+Config.tp+"/More.png"
            imageEnterSource: "resource/icon/"+Config.tp+"/MoreEnter.png"
            imageDisableSource: "resource/icon/"+Config.tp+"/MoreDisable.png"
            width: 12
            height: 16
            enabled: sConfig.loopState===-1 && !sConfig.isRun
            onPressed: channelSelectMenuPopup.open();
        }
        QMenuPopup{
            property int selectIndex: 11
            id: channelSelectMenuPopup
            parent: moreButton
            showShortcutKey: false
            data: [{"showText":qsTr("3ch")+(sConfig.isBuffer?sessionLevel_===1?"(1GHz)":"(250MHz)":"(100MHz)"),"shortcutKey":"","seleteType":0,"buttonIndex":0},
                {"showText":qsTr("8ch")+(sConfig.isBuffer?sessionLevel_===1?"(1GHz)":"(250MHz)":"(25MHz)"),"shortcutKey":"","seleteType":0,"buttonIndex":1},
                {"showText":qsTr("12ch")+(sConfig.isBuffer?sessionLevel_===1?"(500MHz)":"(250MHz)":"(25MHz)"),"shortcutKey":"","seleteType":0,"buttonIndex":2},
                {"showText":qsTr("16ch")+(sConfig.isBuffer?sessionLevel_===1?"(500MHz)":"(250MHz)":"(20MHz)"),"shortcutKey":"","seleteType":0,"buttonIndex":3},
                {"showText":"-","shortcutKey":"","seleteType":0,"buttonIndex":-1},
                {"showText":qsTr("1X通道高度"),"shortcutKey":"","seleteType":selectIndex==11?2:1,"buttonIndex":11},
                {"showText":qsTr("2X通道高度"),"shortcutKey":"","seleteType":selectIndex==12?2:1,"buttonIndex":12},
                {"showText":qsTr("4X通道高度"),"shortcutKey":"","seleteType":selectIndex==13?2:1,"buttonIndex":13},
                {"showText":qsTr("8X通道高度"),"shortcutKey":"","seleteType":selectIndex==14?2:1,"buttonIndex":14},
                {"showText":"-","shortcutKey":"","seleteType":0,"buttonIndex":-1},
                {"showText":qsTr("毛刺过滤设置"),"shortcutKey":"","seleteType":0,"buttonIndex":4},
                {"showText":qsTr("通道颜色修改"),"shortcutKey":"","seleteType":0,"buttonIndex":5},
                {"showText":qsTr("重置所有通道名"),"shortcutKey":"","seleteType":0,"buttonIndex":6}]
            onSelect: {
                var component;
                var dataJson;
                var i;
                switch(index){
                case 0:
                    setNumChannelState(3,true);
                    break;
                case 1:
                    setNumChannelState(8,true);
                    break;
                case 2:
                    setNumChannelState(12,true);
                    break;
                case 3:
                    setNumChannelState(16,true);
                    break;
                case 4:
                    component = Qt.createComponent("qrc:/qml/window/GlitchRemovalSetWindow.qml");
                    if (component.status === Component.Ready){
                        dataJson=[];
                        let js=JSON.parse(con.recode);
                        for(i=0;i<js.length;i++){
                            let temp={}
                            temp["enable"]=js[i]["enable"];
                            temp["num"]=js[i]["num"];
                            dataJson.push(temp);
                        }
                        let openChannels=[];
                        for(let i in sSettings.channelsSet){
                            if(sSettings.channelsSet[i].enable)
                                openChannels.push(parseInt(i));
                        }
                        let subParams = {
                            "dataJson":dataJson,
                            "screen": root.getScreenQRect(),
                            "openChannels": openChannels
                        }
                        let showWindow = component.createObject(root, subParams);
                        showWindow.show();
                        con.target=showWindow;
                    }
                    break;
                case 5:
                    component = Qt.createComponent("qrc:/qml/window/ColorPickerWindow.qml");
                    if (component.status === Component.Ready){
                        dataJson=[];
                        for(i=0;i<sessionChannelCount_;i++){
                            let temp={}
                            temp["color"]=sSettings.channelsDataColor[i];
                            dataJson.push(temp);
                        }
                        let subParams = {
                            "dataJson":dataJson,
                            "screen": root.getScreenQRect()
                        }
                        let showWindow = component.createObject(root, subParams);
                        showWindow.show();
                        con.target=showWindow;
                    }
                    break;
                case 6:
                    for(let i=0;i<sessionChannelCount_;i++)
                        sSignal.channelNameSignalChanged(i,"Channel "+i)
                    break;
                case 11:
                    if(selectIndex==11)
                        sSettings.channelHeightMultiple=2;
                    selectIndex=11;
                    sSettings.channelHeightMultiple=1;
                    break;
                case 12:
                    if(selectIndex==12)
                        sSettings.channelHeightMultiple=1;
                    selectIndex=12;
                    sSettings.channelHeightMultiple=2;
                    break;
                case 13:
                    if(selectIndex==13)
                        sSettings.channelHeightMultiple=2;
                    selectIndex=13;
                    sSettings.channelHeightMultiple=4;
                    break;
                case 14:
                    if(selectIndex==14)
                        sSettings.channelHeightMultiple=4;
                    selectIndex=14;
                    sSettings.channelHeightMultiple=8;
                    break;
                }
            }
        }

        TextButton{
            id: clearChannelButton
            text: "Clear"
            anchors{
                verticalCenter: parent.verticalCenter
                right: moreButton.left
                rightMargin: 5
            }
            width: 44
            height: 20
            textFontSize: 11
            textColor: enabled?Config.subheadColor:Config.textDisabledColor
            textEnterColor: Config.subheadColor
            backgroundRectangle{
                border{
                    width: 1
                    color: enabled?Config.borderLineColor:Config.textDisabledColor
                }
                radius: 10
            }
            enabled: moreButton.enabled
            onPressed: setNumChannelState(sessionChannelCount_,false);
        }
        TextButton{
            id: allChannelButton
            text: "All"
            anchors{
                verticalCenter: parent.verticalCenter
                right: clearChannelButton.left
                rightMargin: 8
            }
            width: 33
            height: 20
            textFontSize: 11
            textColor: enabled?Config.subheadColor:Config.textDisabledColor
            textEnterColor: Config.subheadColor
            backgroundRectangle{
                border{
                    width: 1
                    color: enabled?Config.borderLineColor:Config.textDisabledColor
                }
                radius: 10
            }
            enabled: moreButton.enabled
            onPressed: setNumChannelState(sessionChannelCount_,true);
        }
    }
    Flow{
        signal colorChanged(var index);
        id: channelRow
        spacing: 5
        width: 240
        anchors{
            left: parent.left
            top: channelText.bottom
            topMargin: 14
        }
        Repeater{
            model: sessionChannelCount_
            Item{
                property color showColor: sSettings.channelsDataColor[index]
                property alias button: channelButton
                id: rootItem
                width: 26-(index===0||index===8?3:0)
                height: 20
                Connections{
                    target: channelRow
                    function onColorChanged(index_)
                    {
                        if(index_===index && rootItem.showColor!==sSettings.channelsDataColor[index]){
                            rootItem.showColor=sSettings.channelsDataColor[index];
                            channelButton.checked=sSettings.channelsSet[index].enable;
                        }
                    }
                }
                TextButton{
                    id: channelButton
                    text: index
                    checkable: true
                    width: 23
                    height: 20
                    anchors.right: parent.right
                    enabled: moreButton.enabled
                    textColor: showColor
                    textEnterColor: "white"
                    backgroundPressedColor: textColor
                    backgroundDisableColor: Config.lineDisableColor
                    backgroundRectangle{
                        border{
                            width: 1
                            color: enabled?showColor:Config.lineDisableColor
                        }
                        radius: 4
                    }
                    checked: sSettings.channelsSet[index].enable
                    Component.onCompleted: sSignal.channelRefresh(index);
                    onCheckedChanged: {
                        if(sSettings.channelsSet.length!==0)
                        {
                            if(!checked){
                                var isChecked=false;
                                for(var i in sSettings.channelsSet){
                                    if(parseInt(i)!==index && sSettings.channelsSet[i].enable){
                                        isChecked=true;
                                        break;
                                    }
                                }
                                if(!isChecked)
                                {
                                    checked=true;
                                    return;
                                }
                            }
                            sSettings.channelsSet[index].enable=checked;
                            sSignal.channelRefresh(-1);
                            resetHzComboBox();
                        }
                    }
                }
            }
        }
    }

    Column{
        id: collectColumn
        spacing: 10
        width: parent.width
        anchors{
            left: parent.left
            top: channelRow.bottom
            topMargin: 20
        }

        Column{
            spacing: 20
            width: parent.width
            Rectangle{
                height: 1
                width: parent.width
                color: Config.lineColor
            }

            Item{
                height: acquisitionParameterText.implicitHeight
                width: 240
                Text {
                    id: acquisitionParameterText
                    text: qsTr("采集参数")
                    font.pixelSize: 16
                    color: channelText.color
                    anchors.left: parent.left
                }
                ImageButton{
                    anchors{
                        right: parent.right
                        rightMargin: 2
                        verticalCenter: parent.verticalCenter
                    }
                    width: 15
                    height: 15
                    imageSource: "resource/icon/"+Config.tp+"/Tip.png"
                    imageEnterSource: "resource/icon/"+Config.tp+"/TipEnter.png"
                    imagePressedSource: imageEnterSource
                    onContainsMouseChanged: sSignal.showSetTips(1,containsMouse);
                }
            }
        }

        Row{
            spacing: 10
            Text {
                width: 55
                text: qsTr("采样参数")
                font.pixelSize: 14
                color: channelText.color
                anchors.verticalCenter: parent.verticalCenter
            }
            Row{
                spacing: 5
                QComboBox{
                    id: timeComboBox
                    width: 90
                    model: ListModel{
                        id: timeComboBoxModel
                        ListElement { showText: "1.00 ms"; cost: 1 }
                        ListElement { showText: "2.00 ms"; cost: 2 }
                        ListElement { showText: "5.00 ms"; cost: 5 }
                        ListElement { showText: "10.00 ms"; cost: 10 }
                        ListElement { showText: "20.00 ms"; cost: 20 }
                        ListElement { showText: "50.00 ms"; cost: 50 }
                        ListElement { showText: "100.00 ms"; cost: 100 }
                        ListElement { showText: "200.00 ms"; cost: 200 }
                        ListElement { showText: "500.00 ms"; cost: 500 }
                        ListElement { showText: "1.00 s"; cost: 1000 }
                        ListElement { showText: "2.00 s"; cost: 2000 }
                        ListElement { showText: "5.00 s"; cost: 5000 }
                        ListElement { showText: "10.00 s"; cost: 10000 }
                        ListElement { showText: "20.00 s"; cost: 20000 }
                        ListElement { showText: "50.00 s"; cost: 50000 }
                        ListElement { showText: "1.00 ms"; cost: 1 }
                        ListElement { showText: "2.00 ms"; cost: 2 }
                        ListElement { showText: "5.00 ms"; cost: 5 }
                        ListElement { showText: "10.00 ms"; cost: 10 }
                        ListElement { showText: "20.00 ms"; cost: 20 }
                        ListElement { showText: "50.00 ms"; cost: 50 }
                        ListElement { showText: "100.00 ms"; cost: 100 }
                        ListElement { showText: "200.00 ms"; cost: 200 }
                        ListElement { showText: "500.00 ms"; cost: 500 }
                        ListElement { showText: "1.00 s"; cost: 1000 }
                        ListElement { showText: "2.00 s"; cost: 2000 }
                        ListElement { showText: "5.00 s"; cost: 5000 }
                        ListElement { showText: "10.00 s"; cost: 10000 }
                        ListElement { showText: "20.00 s"; cost: 20000 }
                        ListElement { showText: "50.00 s"; cost: 50000 }
                    }
                    disable: !moreButton.enabled
                    currentIndex: sSettings.settingData["selectTimeIndex"];
                    onCurrentIndexChanged: {
                        if(currentIndex>=0){
                            if(sConfig.isLoadSetting)
                                currentIndex=sSettings.settingData["selectTimeIndex"];
                            else
                                sSettings.settingData["selectTimeIndex"]=currentIndex;
                        }
                    }
                    onCurrentModelChildrenChanged: {
                        if(currentModelChildren){
                            if(typeof(currentModelChildren.rle)!="undefined")
                                sConfig.isRLE=currentModelChildren.rle;
                            sSettings.settingData["setTime"]=currentModelChildren.cost;
                            samplingDepthText.text=sa2Unit(sSettings.settingData["setHz"]*sSettings.settingData["setTime"]/1000)+"/ch"+(sConfig.isRLE?" (RLE)":"");
                            setStateText();
                        }
                    }
                }
                QComboBox{
                    id: hzComboBox
                    width: 80
                    model: ListModel{
                        id: hzComboBoxModel
                        ListElement { showText: "1 MHz"; cost: 1000000 }
                        ListElement { showText: "2 MHz"; cost: 2000000 }
                        ListElement { showText: "4 MHz"; cost: 4000000 }
                        ListElement { showText: "5 MHz"; cost: 5000000 }
                        ListElement { showText: "10 MHz"; cost: 10000000 }
                        ListElement { showText: "20 MHz"; cost: 20000000 }
                        ListElement { showText: "25 MHz"; cost: 25000000 }
                        ListElement { showText: "40 MHz"; cost: 40000000 }
                        ListElement { showText: "50 MHz"; cost: 50000000 }
                        ListElement { showText: "100 MHz"; cost: 100000000 }
                        ListElement { showText: "200 MHz"; cost: 200000000 }
                        ListElement { showText: "250 MHz"; cost: 250000000 }
                        ListElement { showText: "500 MHz"; cost: 500000000 }
                        ListElement { showText: "1 GHz"; cost: 1000000000 }
                    }
                    disable: !moreButton.enabled
                    currentIndex: sSettings.settingData["selectHzIndex"];
                    onCurrentIndexChanged: {
                        if(currentIndex>=0){
                            if(sConfig.isLoadSetting)
                                currentIndex=sSettings.settingData["selectHzIndex"];
                            else
                                sSettings.settingData["selectHzIndex"]=currentIndex;
                        }
                    }
                    onCurrentModelChildrenChanged: {
                        if(currentModelChildren){
                            sSettings.settingData["setHz"]=currentModelChildren.cost;
                            samplingDepthText.text=sa2Unit(sSettings.settingData["setHz"]*sSettings.settingData["setTime"]/1000)+"/ch"+(sConfig.isRLE?" (RLE)":"");
                            resetTimeComboBox();
                        }
                    }
                }
            }
        }

        Row{
            spacing: 10
            Text {
                width: 55
                text: qsTr("触发阈值")
                font.pixelSize: 14
                color: channelText.color
                anchors.verticalCenter: parent.verticalCenter
            }
            Row{
                spacing: 5
                QComboBox{
                    id: thresholdLevelComboBox
                    width: 90
                    model: ListModel{
                        ListElement { showText: qsTr("自定义阈值"); cost: -1 }
                        ListElement { showText: "5.0V CMOS"; cost: 2.5 }
                        ListElement { showText: "3.3V CMOS"; cost: 1.6 }
                        ListElement { showText: "3.0V CMOS"; cost: 1.5 }
                        ListElement { showText: "2.5V CMOS"; cost: 1.2 }
                        ListElement { showText: "1.8V CMOS"; cost: 0.9 }
                        ListElement { showText: "1.5V CMOS"; cost: 0.7 }
                        ListElement { showText: "1.2V CMOS"; cost: 0.6 }
                    }
                    disable: !moreButton.enabled
                    currentIndex: sSettings.settingData["selectThresholdLevelIndex"];
                    onCurrentIndexChanged: sSettings.settingData["selectThresholdLevelIndex"]=currentIndex;
                    onCurrentModelChildrenChanged: {
                        if(currentModelChildren.cost!==-1)
                        {
                            spinBox.value=currentModelChildren.cost
                            spinBox.disable=true;
                        }else
                            spinBox.disable=false;
                    }
                    onDisableChanged: {
                        if(disable)
                            spinBox.disable=true;
                        else
                            currentModelChildrenChanged();
                    }
                }
                QSpinBox{
                    id: spinBox
                    width: 80
                    from: -5
                    to: 5
                    value: sSettings.settingData["thresholdLevel"].toString()
                    decimalDigits: 1
                    postfixText: "V"
                    onValueChanged: {
                        sSettings.settingData["thresholdLevel"]=parseFloat(value);
                        setStateText();
                    }
                    onContainsMouseChanged: {
                        if(containsMouse&&inputActiveFocus)
                            tooltipPopup.open()
                        else
                            tooltipPopup.close()
                    }
                    onInputActiveFocusChanged: {
                        if(containsMouse&&inputActiveFocus)
                            tooltipPopup.open()
                        else
                            tooltipPopup.close()
                    }
                }
                Popup{
                    id: tooltipPopup
                    parent: thresholdLevelComboBox
                    closePolicy: Popup.NoAutoClose
                    y: spinBox.y-5
                    height: 143+10
                    width: 216+10
                    padding: 0
                    background: Rectangle{
                        radius: 8
                        color: "transparent"
                        Image {
                            fillMode: Image.PreserveAspectFit
                            anchors{
                                fill: parent
                                margins: 5
                            }
                            width: 216
                            height: 143
                            source: "qrc:resource/icon/"+Config.tp+"/thresholdLevelTooltip.png"
                        }
                    }
                    Component.onCompleted: x=spinBox.x-width
                }
            }
        }

        Row{
            spacing: 10
            width: parent.width
            height: 35
            Text {
                width: 55
                text: qsTr("采样深度")
                font.pixelSize: 14
                color: channelText.color
                anchors.verticalCenter: parent.verticalCenter
            }
            Text {
                id: samplingDepthText
                font{
                    pixelSize: 14
                }
                color: moreButton.enabled?Config.subheadColor:Config.textDisabledColor
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        Item{
            id: triggerPositionItem
            width: parent.width
            height: 50
            QTextInput{
                id: triggerPositionInput
                width: 26
                height: 16
                disable: !triggerPositionSlider.enabled
                onIsFocusChanged: {
                    if(!isFocus)
                    {
                        if(parseInt(showText)<1)
                            triggerPositionSlider.value=1;
                        else if(parseInt(showText)>90)
                            triggerPositionSlider.value=90;
                        else
                            triggerPositionSlider.value=parseInt(showText);
                        showText=triggerPositionSlider.value;
                    }
                }
            }
            Text {
                text: "%"
                font.pixelSize: 12
                color: triggerPositionSlider.enabled?Config.textColor:Config.textDisabledColor
                anchors{
                    verticalCenter: triggerPositionInput.verticalCenter
                    left: triggerPositionInput.right
                    leftMargin: 5
                }
            }

            Row{
                id: triggerPositionRow
                spacing: 10
                anchors{
                    left: parent.left
                    right: parent.right
                    bottom: parent.bottom
                    bottomMargin: 10
                }
                height: 28
                Text {
                    width: 55
                    text: qsTr("触发位置")
                    font.pixelSize: 14
                    color: triggerPositionSlider.enabled?Config.textColor:Config.textDisabledColor
                    anchors.verticalCenter: parent.verticalCenter
                }
                QSlider{
                    id: triggerPositionSlider
                    width: 175
                    anchors{
                        bottom: parent.bottom
                        bottomMargin: 7
                    }
                    from: 1
                    to: 90
                    splice: 9
                    postfixText: "%"
                    isShowText: false
                    enabled: sConfig.loopState===-1 && !sConfig.isRun && sConfig.isBuffer
                    value: sSettings.settingData["triggerPosition"]
                    onHandleXChanged: {
                        if(handleX>135)
                            triggerPositionInput.x=135+triggerPositionSlider.x
                        else
                            triggerPositionInput.x=handleX+triggerPositionSlider.x
                    }
                    onValueChanged: {
                        sSettings.settingData["triggerPosition"]=value
                        triggerPositionInput.showText=value
                    }
                    Component.onCompleted: triggerPositionInput.x=triggerPositionSlider.x+5
                }
            }
        }
    }

    Column{
        id: titleColumn
        spacing: 10
        anchors{
            top: collectColumn.bottom
            topMargin: 20
            left: parent.left
            right: parent.right
        }
        Column{
            spacing: 20
            width: parent.width
            Rectangle{
                height: 1
                width: parent.width
                color: Config.lineColor
            }

            Item{
                height: ingestOptionsText.implicitHeight
                width: 240
                Text {
                    id: ingestOptionsText
                    text: qsTr("采集选项")
                    font.pixelSize: 16
                    color: channelText.color
                    anchors.left: parent.left
                }
                ImageButton{
                    anchors{
                        right: parent.right
                        rightMargin: 2
                        verticalCenter: parent.verticalCenter
                    }
                    width: 15
                    height: 15
                    imageSource: "resource/icon/"+Config.tp+"/Tip.png"
                    imageEnterSource: "resource/icon/"+Config.tp+"/TipEnter.png"
                    imagePressedSource: imageEnterSource
                    onContainsMouseChanged: sSignal.showSetTips(2,containsMouse);
                }
            }
        }

        Row{
            spacing: 10
            Text {
                width: 55
                text: qsTr("运行模式")
                font.pixelSize: 14
                color: channelText.color
                anchors.verticalCenter: parent.verticalCenter
            }
            RadioGroup{
                property string button1: "Buffer"
                property string button2: "Stream"
                id: modelRadioGroup
                width: 177
                height: 28
                disable: !moreButton.enabled
                modelData: ListModel {
                    Component.onCompleted: {
                        this.append({buttonText: modelRadioGroup.button1});
                        this.append({buttonText: modelRadioGroup.button2});
                    }
                }
                selectButton: sSettings.settingData["isBuffer"]?button1:button2
                onSelectButtonChanged: {
                    sSettings.settingData["isBuffer"]=selectButton===button1;
                    sConfig.isBuffer=selectButton===button1;
                    resetHzComboBox();
                }
            }
        }

        Row{
            spacing: 10
            Text {
                width: 55
                text: qsTr("采集方式")
                font.pixelSize: 14
                color: channelText.color
                anchors.verticalCenter: parent.verticalCenter
            }
            RadioGroup{
                property string button1: qsTr("单次")
                property string button2: qsTr("循环")
                property string button3: qsTr("立即")
                id: methodsRadioGroup
                width: 177
                height: 28
                disable: !moreButton.enabled
                modelData: ListModel {
                    ListElement{
                        buttonText: qsTr("单次")
                    }
                    ListElement{
                        buttonText: qsTr("循环")
                    }
                    ListElement{
                        buttonText: qsTr("立即")
                    }
                }
                selectButton: sSettings.isInstantly?button3:sSettings.isOne?button1:button2
                onSelectButtonChanged: {
                    sSettings.isInstantly=selectButton===button3;
                    if(!sSettings.isInstantly)
                        sSettings.isOne=selectButton===button1;
                    else
                        sSettings.isSimpleTrigger=true;
                }
            }
        }

        Row{
            spacing: 10
            height: 28
            Text{
                width: 55
                text: qsTr("采样间隔")
                font.pixelSize: 14
                color: methodsRadioGroup.selectButton===methodsRadioGroup.button2&&moreButton.enabled?Config.textColor:Config.textDisabledColor
                anchors.verticalCenter: parent.verticalCenter
            }
            QSlider{
                id: intervalSlider
                width: 175
                anchors{
                    top: parent.top
                    topMargin: 8
                }
                from: 0
                to: 10
                postfixText: "s"
                enabled: methodsRadioGroup.selectButton===methodsRadioGroup.button2&&moreButton.enabled
                value: sSettings.settingData["intervalTime"]
                onValueChanged: {
                    sSettings.settingData["intervalTime"]=parseInt(value)
                }
            }
        }
    }

    Column{
        id: waveformOutputTitleColumn
        spacing: 20
        anchors{
            top: titleColumn.bottom
            topMargin: 20
            left: parent.left
            right: parent.right
        }
        Rectangle{
            height: 1
            width: parent.width
            color: Config.lineColor
        }

        Text {
            text: qsTr("波形输出")
            font.pixelSize: 16
            color: Config.textColor
        }

        Column{
            spacing: 10
            width: parent.width
            Item{
                width: 240
                height: 25
                Text {
                    text: "PWM0"
                    font.pixelSize: 14
                    color: Config.textColor
                    anchors.verticalCenter: parent.verticalCenter
                }
                ImageButton{
                    property bool isOpen: false
                    id: pwm0Button
                    width: 36
                    height: 20
                    enabled: moreButton.enabled
                    imageSource: sConfig.pwm0Start?"resource/icon/"+Config.tp+"/SwitchOpen.png":"resource/icon/"+Config.tp+"/SwitchClose.png"
                    imageEnterSource: imageSource
                    imageDisableSource: sConfig.pwm0Start?"resource/icon/"+Config.tp+"/SwitchOpenDisable.png":"resource/icon/"+Config.tp+"/SwitchCloseDisable.png"
                    onImageSourceChanged: isOpen=sConfig.pwm0Start;
                    anchors{
                        right: parent.right
                        verticalCenter: parent.verticalCenter
                    }
                    onPressed: {
                        if(sessionType_!==Config.SessionType.Device)
                            return;
                        isOpen=!isOpen;
                        if(isOpen)
                        {
                            controller.fpgaActive();
                            pwmSend(0);
                        }
                        else if(!controller.pwmStop(0))
                            isOpen=true;
                        sConfig.pwm0Start=isOpen;
                        if(!sConfig.pwm0Start&&!sConfig.pwm1Start)
                            controller.fpgaDormancy();
                    }
                }
            }
            Row{
                spacing: 7
                Row{
                    spacing: 7
                    Text {
                        text: qsTr("频率")
                        font.pixelSize: 14
                        color: pwm0Button.isOpen?Config.textColor:Config.textDisabledColor
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    Row{
                        spacing: 3
                        QSpinBox{
                            id: pwm0SpinBox
                            width: 45
                            from: 1
                            to: 1
                            value: sSettings.pwmData[0]["hz"].toString()
                            disable: !pwm0Button.isOpen || !pwm0Button.enabled
                            onValueChanged: {
                                sSettings.pwmData[0]["hz"]=parseFloat(value);
                                pwmSend(0);
                            }
                            Component.onCompleted: value=sSettings.pwmData[0]["hz"];
                        }
                        QComboBox{
                            id: pwm0HzComboBox
                            width: 52
                            model: ListModel{
                                ListElement { showText: "Hz"; cost: 0 }
                                ListElement { showText: "KHz"; cost: 1 }
                                ListElement { showText: "MHz"; cost: 2 }
                            }
                            disable: !pwm0Button.isOpen || !pwm0Button.enabled
                            currentIndex: sSettings.pwmData[0]["unit"];
                            onCurrentIndexChanged: {
                                sSettings.pwmData[0]["unit"]=currentIndex;
                                pwm0SpinBox.to=currentIndex===2?20:999;
                                pwmSend(0);
                            }
                        }
                    }
                }
                Row{
                    spacing: 7
                    Text {
                        text: qsTr("占空比")
                        font.pixelSize: 14
                        color: pwm0Button.isOpen?Config.textColor:Config.textDisabledColor
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    QSpinBox{
                        id: pwm0dutySpinBox
                        width: 50
                        from: 1
                        to: 99
                        value: sSettings.pwmData[0]["duty"].toString()
                        disable: !pwm0Button.isOpen || !pwm0Button.enabled
                        postfixText: "%"
                        onValueChanged: {
                            sSettings.pwmData[0]["duty"]=parseFloat(value);
                            pwmSend(0);
                        }
                    }
                }
            }
        }

        Column{
            spacing: 10
            width: parent.width
            Item{
                width: 240
                height: 25
                Text {
                    text: "PWM1"
                    font.pixelSize: 14
                    color: Config.textColor
                    anchors.verticalCenter: parent.verticalCenter
                }
                ImageButton{
                    property bool isOpen: false
                    id: pwm1Button
                    width: 36
                    height: 20
                    enabled: moreButton.enabled
                    imageSource: sConfig.pwm1Start?"resource/icon/"+Config.tp+"/SwitchOpen.png":"resource/icon/"+Config.tp+"/SwitchClose.png"
                    imageEnterSource: imageSource
                    imageDisableSource: sConfig.pwm1Start?"resource/icon/"+Config.tp+"/SwitchOpenDisable.png":"resource/icon/"+Config.tp+"/SwitchCloseDisable.png"
                    onImageSourceChanged: isOpen=sConfig.pwm1Start;
                    anchors{
                        right: parent.right
                        verticalCenter: parent.verticalCenter
                    }
                    onPressed: {
                        if(sessionType_!==Config.SessionType.Device)
                            return;
                        isOpen=!isOpen;
                        if(isOpen)
                        {
                            controller.fpgaActive();
                            pwmSend(1);
                        }
                        else if(!controller.pwmStop(1))
                            isOpen=true;
                        sConfig.pwm1Start=isOpen;
                        if(!sConfig.pwm0Start&&!sConfig.pwm1Start)
                            controller.fpgaDormancy();
                    }
                }
            }
            Row{
                spacing: 7
                Row{
                    spacing: 7
                    Text {
                        text: qsTr("频率")
                        font.pixelSize: 14
                        color: pwm1Button.isOpen?Config.textColor:Config.textDisabledColor
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    Row{
                        spacing: 3
                        QSpinBox{
                            id: pwm1SpinBox
                            width: 45
                            from: 1
                            to: 1
                            value: sSettings.pwmData[1]["hz"].toString()
                            disable: !pwm1Button.isOpen || !pwm1Button.enabled
                            onValueChanged: {
                                sSettings.pwmData[1]["hz"]=parseFloat(value);
                                pwmSend(1);
                            }
                            Component.onCompleted: value=sSettings.pwmData[1]["hz"];
                        }
                        QComboBox{
                            id: pwm1HzComboBox
                            width: 52
                            model: ListModel{
                                ListElement { showText: "Hz"; cost: 0 }
                                ListElement { showText: "KHz"; cost: 1 }
                                ListElement { showText: "MHz"; cost: 2 }
                            }
                            disable: !pwm1Button.isOpen || !pwm1Button.enabled
                            currentIndex: sSettings.pwmData[1]["unit"];
                            onCurrentIndexChanged: {
                                sSettings.pwmData[1]["unit"]=currentIndex;
                                pwm1SpinBox.to=currentIndex===2?20:999;
                                pwmSend(1);
                            }
                        }
                    }
                }
                Row{
                    spacing: 7
                    Text {
                        text: qsTr("占空比")
                        font.pixelSize: 14
                        color: pwm1Button.isOpen?Config.textColor:Config.textDisabledColor
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    QSpinBox{
                        id: pwm1dutySpinBox
                        width: 50
                        from: 1
                        to: 99
                        value: sSettings.pwmData[1]["duty"].toString()
                        disable: !pwm1Button.isOpen || !pwm1Button.enabled
                        postfixText: "%"
                        onValueChanged: {
                            sSettings.pwmData[1]["duty"]=parseFloat(value);
                            pwmSend(1);
                        }
                    }
                }
            }
        }
    }

    function resetHzComboBox(){
        let count=0;
        for(var i in sSettings.channelsSet){
            if(sSettings.channelsSet[i].enable)
                count++;
        }
        let current=hzComboBox.currentIndex;
        let maxDepth=320000000
        if(sConfig.isBuffer)
            maxDepth=Math.pow(2,40)*1000-1

        hzComboBoxModel.clear();
        appendHzConboBox(count, maxDepth, [1,2,4,5],     1000000);
        appendHzConboBox(count, maxDepth, [1,2,2.5,4,5], 10000000);
        if(sConfig.isBuffer){
            if(sessionLevel_===1)
                appendHzConboBox(count, maxDepth, [1,2,2.5,5], 100000000);
            else
                appendHzConboBox(count, maxDepth, [1,2,2.5], 100000000);
        }
        else
            appendHzConboBox(count, maxDepth, [1], 100000000);

        if(sessionLevel_===1){
            if(count<=8&&sConfig.isBuffer)
                hzComboBoxModel.append({ showText: "1 GHz", cost: 1000000000 });
        }
        if(current+1>=hzComboBoxModel.count)
            hzComboBox.currentIndex=hzComboBoxModel.count-1;
        else
            hzComboBox.currentIndex=current;

        setStateText();
    }

    function appendHzConboBox(channelNum, maxDepth, list, power){
        for(let i=0;i<list.length;i++){
            let hz=list[i]*power
            let depth=hz*channelNum
            if(depth<=maxDepth)
                hzComboBoxModel.append({ "showText": (power/1000000*list[i]).toString()+" MHz", "cost": hz})
            else
                return false
        }
        return true
    }

    function resetTimeComboBox(){
        let count=0;
        for(var i in sSettings.channelsSet){
            if(sSettings.channelsSet[i].enable)
                count++;
        }
        let current=timeComboBox.currentIndex;
        let maxDepth=1000000000*1000
        if(sessionLevel_===1)
            maxDepth=3500000000*1000
        let rleMaxDepth=maxDepth*10

        if(!sConfig.isBuffer){
            rleMaxDepth=Math.pow(2,40)*1000-1
            maxDepth=rleMaxDepth;
        }

        timeComboBoxModel.clear();
        if(appendTimeConboBox(count, maxDepth, rleMaxDepth,"ms", 1, 3))
            if(appendTimeConboBox(count, maxDepth, rleMaxDepth,"s", 1000, 2))
                if(appendTimeConboBox(count, maxDepth, rleMaxDepth,"m", 60000, 2))
                    appendTimeConboBox(count, maxDepth, rleMaxDepth,"h", 3600000, 2);
        let maxTime=rleMaxDepth/count/sSettings.settingData["setHz"];
        if(maxTime>(typeof timeComboBoxModel.get(timeComboBoxModel.count-1) != "undefined" ? timeComboBoxModel.get(timeComboBoxModel.count-1).cost : 0))
            timeComboBoxModel.append({ "showText": time2Unit(maxTime,2)+(sConfig.isBuffer?" *":""), "cost": maxTime, "rle": sConfig.isBuffer?true:false });
        if(current+1>=timeComboBoxModel.count)
            timeComboBox.currentIndex=timeComboBoxModel.count-1;
        else
            timeComboBox.currentIndex=current;
    }

    function appendTimeConboBox(channelNum, maxDepth, rleMaxDepth, unit, power, powerMaxIndex){
        let arr=[1,2,5]
        let arrPower=[1,10,100]
        let hz=sSettings.settingData["setHz"]
        for(let i=0;i<powerMaxIndex;i++){
            for(let j=0;j<arr.length;j++){
                let time=arr[j]*arrPower[i]*power
                let depth=hz*time*channelNum
                if(depth<=rleMaxDepth)
                    timeComboBoxModel.append({ "showText": time2Unit(time,2)+(depth>maxDepth?" *":""), "cost": time, "rle": depth>maxDepth })
                else
                    return false
            }
        }
        return true;
    }

    function setStateText(){
        if(sessionType_===Config.SessionType.Device)
        {
            var t="";
            if(typeof(sSettings.settingData)!=="undefined"){
                t+=(qsTr("时间: ")+time2Unit(sSettings.settingData["setTime"],2)+"   ");
                t+=(qsTr("频率: ")+hz2Unit(sSettings.settingData["setHz"],6)+"   ");
                t+=(qsTr("阈值: ")+sSettings.settingData["thresholdLevel"].toString()+" V   ");
                t+=(qsTr("深度: ")+samplingDepthText.text);
            }
            //            t+=("USB: "+sessionUSB);
            stateText=t;
            Signal.setStateText(t);
        }else
        {
            stateText="";
            Signal.setStateText("");
        }
    }

    function pwmSend(index){
        //防止初始化的时候自动启动pwm
        if(index===0&&!pwm0Button.isOpen||index===1&&!pwm1Button.isOpen)
            return;
        var hz,duty;
        if(index===0)
        {
            hz=parseInt(pwm0SpinBox.value);
            duty=parseInt(pwm0dutySpinBox.value);
            if(pwm0HzComboBox.currentIndex===1)
                hz*=1000;
            else if(pwm0HzComboBox.currentIndex===2)
                hz*=1000000;
        }else{
            hz=parseInt(pwm1SpinBox.value);
            duty=parseInt(pwm1dutySpinBox.value);
            if(pwm1HzComboBox.currentIndex===1)
                hz*=1000;
            else if(pwm1HzComboBox.currentIndex===2)
                hz*=1000000;
        }
        var isOpen=controller.pwmStart(index, hz, duty);
        if(index===0)
            pwm0Button.isOpen=isOpen;
        else
            pwm1Button.isOpen=isOpen;
    }
}
