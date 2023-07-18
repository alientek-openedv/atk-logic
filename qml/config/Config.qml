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

pragma Singleton

import QtQuick 2.15

QtObject {
    enum SessionType{Demo, Device ,File}
    enum ChannelTriggerButton {Null, RisingEdge, HighLevel, FallingEdge, LowLevel, DoubleEdge}
    enum MenuState{NoDisplay, Display, Fixed, NoFixed}
    enum SidebarContentType{Null, Set, Decode, Trigger, Measure, Search}
    enum MenuItemType{File, Property, Show, About}
    enum ShortcutKey{StartCollecting, StopCollecting, ParameterMeasure, VernierCreate, SwitchPageUp, SwitchPageDown,
        JumpZero, ZoomIn, ZoomOut, ZoomFull, Save, SaveAs, ExportFile, DeviceConfig, ProtocolDecode,
        LabelMeasurement, DataSearch, CloseSession}
    property var themeList: ["dark","light"]
    property string tp: themeList[Setting.theme]
    property int headerHeight: 30
    property int headerWidth: 190
    property int closeHeaderWidth: 55
    property int sideBarWidth: 60
    property int sideBarContentWidth: 268
    property color backgroundColor: Setting.theme===0?"#1e1e1e":"#ffffff"       
    property color background2Color: Setting.theme===0?"#252526":"#F6F6F6"      
    property color pageColor: Setting.theme===0?"#151515":"#ffffff"             
    property color mouseEnterColor: Setting.theme===0?"#37373b":"#EBF5FF"       
    property color mouseCheckColor: "#3587fe"                                   
    property color lineColor: Setting.theme===0?"#37373b":"#d5d5d5"             
    property color lineDisableColor: Setting.theme===0?"#37373b":"#d5d5d5"      
    property color borderLineColor: Setting.theme===0?"#151515":"#a5b3cc"       
    property color borderBackgroundColor: Setting.theme===0?"#151515":"#ffffff" 
    property color textColor: Setting.theme===0?"#ffffff":"#383838"             
    property color subheadColor: "#8e8e8e"                                      
    property color textDisabledColor: Setting.theme===0?"#5d5d5d":"#b2b2b2"     
    property color measurePopupBackground: Setting.theme===0?"#37373b":"#c6d8ff"
    property color measurePopupBackground2: Setting.theme===0?"#252526":"#ebf5ff"
    property color setKeysInputBackground: Setting.theme===0?"#151515":"#d5d5d5"
    property color setKeysButtonDisableText: Setting.theme===0?"#8e8e8e":"#ffffff"
    property color red: "#e73131"                                               
    property color green: "#00af54"                                             
    property color iceBlue: "#a5b3cc"                                           
    property var channelDataColor: ["#9252e8","#3462f6","#148c1e","#d1b101",
        "#f09a37","#e2493a","#85422d","#898989","#9252e8","#3462f6","#148c1e",
        "#d1b101","#f09a37","#e2493a","#85422d","#898989"]
    property bool isFixed: false
    property int initHeightState: 0 
    property var menuSelectButton
    property var tabSelectButton
    property var languageList: ["zh_CN","en_US"]
    property string language
    property var updateJson
    property bool isSetModel: false
    property bool isControlEnter: false
    property bool isHardwareUpdate: false
    property bool isHardwarePageConnect: false
    property bool isExit: false
}
