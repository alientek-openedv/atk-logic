pragma Singleton

import QtQuick 2.15

QtObject {
    enum SessionType{Demo, Device ,File}
    enum ChannelTriggerButton {Null, RisingEdge, HighLevel, FallingEdge, LowLevel, DoubleEdge}
    enum MenuState{NoDisplay, Display, Fixed, NoFixed}
    enum SidebarContentType{Null, Set, Decode, Trigger, Measure, Search}
    enum MenuItemType{File, Property, Show, About}
    enum ShortcutKey{DelVernierMeasure, StartCollecting, StopCollecting, SwitchVernierUp, SwitchVernierDown, ParameterMeasure, VernierCreate
        , SwitchPageUp, SwitchPageDown, JumpZero, ZoomIn, ZoomOut, ZoomFull, Save, SaveAs, ExportFile, DeviceConfig, ProtocolDecode,
        LabelMeasurement, DataSearch, CloseSession}
    enum SaveType{DataCSV, TableCSV, ATKDL, BinDATA}

    //全局常量
    property var themeList: ["dark","light"]
    property string tp: themeList[Setting.theme]

    property int headerHeight: 30//通道头部高度
    property int headerWidth: 190//左侧边栏宽度
    property int closeHeaderWidth: 55//左侧边栏关闭宽度
    property int sideBarWidth: 60//右侧边栏宽度
    property int sideBarContentWidth: 268//右侧边栏弹窗宽度

    property color backgroundColor: Setting.theme===0?"#1e1e1e":"#FAFAFA"       //主色
    property color background2Color: Setting.theme===0?"#252526":"#F0F0F0"      //配色
    property color pageColor: Setting.theme===0?"#1A1A1A":"#FAFAFA"             //通道底色及标签
    property color mouseEnterColor: Setting.theme===0?"#37373b":"#EBF5FF"       //焦点颜色
    property color mouseCheckColor: "#3587fe"                                   //点击颜色
    property color lineColor: Setting.theme===0?"#37373b":"#d5d5d5"             //线条颜色
    property color lineDisableColor: Setting.theme===0?"#37373b":"#d5d5d5"      //线条禁用颜色
    property color borderLineColor: Setting.theme===0?"#1A1A1A":"#a5b3cc"       //边框颜色
    property color borderBackgroundColor: Setting.theme===0?"#1A1A1A":"#FAFAFA" //边框背景颜色

    property color textColor: Setting.theme===0?"#F0F0F0":"#000"             //文本颜色
    property color subheadColor: "#8e8e8e"                                      //副文本颜色
    property color textDisabledColor: Setting.theme===0?"#5d5d5d":"#b2b2b2"     //文本禁用颜色

    property color measurePopupBackground: Setting.theme===0?"#37373b":"#c6d8ff"//测量悬浮窗上部背景
    property color measurePopupBackground2: Setting.theme===0?"#252526":"#ebf5ff"//测量悬浮窗下部背景

    property color setKeysInputBackground: Setting.theme===0?"#1A1A1A":"#d5d5d5"//设置快捷键输入底色
    property color setKeysButtonDisableText: Setting.theme===0?"#8e8e8e":"#F0F0F0"//设置快捷键按钮禁用文本颜色


    property color red: "#e73131"                                               //特定红色
    property color green: "#00af54"                                             //特定绿色
    property color iceBlue: "#a5b3cc"                                           //触发划动、协议解码进度等

    property var channelDataColor: ["#9252e8","#3462f6","#148c1e","#d1b101",
        "#f09a37","#e2493a","#85422d","#898989","#9252e8","#3462f6","#148c1e",
        "#d1b101","#f09a37","#e2493a","#85422d","#898989"]//通道默认颜色

    //全局变量
    property bool isFixed: false
    property int initHeightState: 0 //首次初始化方法状态，0=未初始化，1=初始化步骤1，2=初始化完成
    property var menuSelectButton
    property var tabSelectButton
    property var languageList: ["zh_CN","en_US"]
    property string language
    property var updateJson
    property bool isSetModel: false//是否设置模式，禁用快捷键
    property bool isControlEnter: false//Ctrl是否被按下
    property bool isHardwareUpdate: false//是否可硬件升级
    property bool isHardwarePageConnect: false//是否打开设备页并且已连接设备
    property bool isExit: false//是否退出程序中
}
