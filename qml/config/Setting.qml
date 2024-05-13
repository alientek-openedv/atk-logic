pragma Singleton
import QtQuick 2.15
import atk.qml.Controls 1.0

QSetting {
    fileName: tempDir+"//set.ini"
    category: "system"
    property int theme: 0//0=暗黑风格，1=明亮风格

    //选项
    property bool collectZoom: true             //采集完成后是否缩放至全屏
    property bool jumpZoom: false                //跳转功能是否自适应缩放界面
    property bool closeHint: true               //关闭软件是否提示保存数据
    property bool viewToolPopup: true           //视图工具悬浮窗
    property bool componentWheelChanged: true   //组件滚动修改属性
    property bool showFPS: false                //显示FPS
    property bool isMouseMeasure: true          //是否使用鼠标测量
    property bool isLinuxMemoryLimit: true      //是否限制ubuntu内存使用

    //提示
    property string notRemindVersion: ""   //不提醒版本

    //窗体属性
    property bool isSetWindow: false
    property int width: 0
    property int height: 0
    property int x: 0
    property int y: 0
    property int windowState: 0

    //样式
    property color mouseMeasureBackColor: "#ddbcedff"//鼠标悬浮窗背景色
    property color mouseMeasureTextColor: "#383838"//鼠标悬浮窗文字色

    //协议习惯
    property var decodeConfig

    //快捷键
    property string startCollecting: "F1"       //开始采集
    property string stopCollecting: "F2"        //停止采集
    property string switchVernierUp: "F3"       //上一个标签
    property string switchVernierDown: "F4"     //下一个标签
    property string parameterMeasure: "Ctrl+G"  //参数测量
    property string vernierCreate: "Ctrl+H"     //添加标签
    property string switchPageUp: "F5"          //上一页
    property string switchPageDown: "F6"        //下一页
    property string jumpZero: "F7"              //跳转到零点
    property string zoomIn: "F8"                //放大
    property string zoomOut: "F9"               //缩小
    property string zoomFull: "F10"             //缩放至整屏
    property string saveFile: "Ctrl+S"          //保存
    property string saveAs: "Ctrl+Shift+S"      //另存为
    property string exportFile: "Ctrl+E"        //导出
    property string deviceConfig: "Ctrl+1"      //设备配置
    property string protocolDecode: "Ctrl+2"    //协议解码
    property string labelMeasurement: "Ctrl+3"  //标签测量
    property string dataSearch: "Ctrl+F"        //数据搜索
    property string closeSession: "Ctrl+W"      //关闭会话

    //decode
    property int decodeLogLevel: 2              //解码日志等级
}
