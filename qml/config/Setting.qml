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
import atk.qml.Controls 1.0

QSetting {
    fileName: tempDir+"//set.ini"
    category: "system"
    property int theme: 0
    property bool collectZoom: true             
    property bool jumpZoom: false                
    property bool closeHint: true               
    property bool viewToolPopup: true           
    property bool componentWheelChanged: true   
    property bool showFPS: false                
    property string startCollecting: "F1"       
    property string stopCollecting: "F2"        
    property string parameterMeasure: "Ctrl+G"  
    property string vernierCreate: "Ctrl+H"     
    property string switchPageUp: "F5"          
    property string switchPageDown: "F6"        
    property string jumpZero: "F7"              
    property string zoomIn: "F8"                
    property string zoomOut: "F9"               
    property string zoomFull: "F10"             
    property string saveFile: "Ctrl+S"          
    property string saveAs: "Ctrl+Shift+S"      
    property string exportFile: "Ctrl+E"        
    property string deviceConfig: "Ctrl+1"      
    property string protocolDecode: "Ctrl+2"    
    property string labelMeasurement: "Ctrl+3"  
    property string dataSearch: "Ctrl+F"        
    property string closeSession: "Ctrl+W"      
}
