import QtQuick 2.15
import atk.qml.Controls 1.0

QSetting {
    fileName: tempDir+"/set.ini"
    property bool isFirst: false//是否首次运行
    property bool isInstantly: false//是否立即采集
    property bool isOne: true//是否单次采集
    property bool isSimpleTrigger: true//是否简单触发
    property var channelsSet:[]//简单触发所有通道json数据
    property var settingData:{"triggerPosition":1}//设置指令数据
    property var pwmData:[];//pwm波设置
    property var favoritesList: []//收藏协议
    property var channelsDataColor: []//通道默认颜色
    property var channelsHeight: []
    property int channelHeightMultiple: 1//通道显示高度倍率
    property var decodeJson: []//已经打开的协议
    property var glitchRemoval: []
}
