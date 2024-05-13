import QtQuick 2.15
import Qt.labs.settings 1.0

QtObject {
    property var type
    property var name
    property bool isExpandSidebar: true//是否展开侧边栏(左)
    property bool isSidebarContentShow: false//侧边栏(右)内容显示
    property int sidebarContentType:0//侧边栏(右)显示类型
    property var useChannels: []
    property int loopState: -1//循环状态，-1=不循环，0=启动循环，1=等待循环
    property bool isMeasureStart: false
    property bool isVernierStart: false
    property bool isMeasureMove: false
    property bool isVernierMove: false
    property bool isCrossChannelMeasure: false
    property bool isChannelMethodRun: isMeasureStart||isVernierStart||isMeasureMove||isVernierMove||isCrossChannelMeasure//是否在执行功能中，例如标记标尺、时间标签
    property bool isRun: false//是否采集中
    property bool isBuffer: false//是否Buffer模式
    property bool isRLE: false//是否RLE压缩
    property string filePath: ""//保存文件记录
    property string setPath: ""//载入工程的配置路径
    property bool isErrorShow: false//是否显示错误窗口
    property bool isCloseNotDataChannels: false//初始化时使用，是否关闭无数据通道
    property bool pwm0Start: false
    property bool pwm1Start: false
    property int decodeColorIndex: 0
    property bool isLiveFollowingPopupShow: false//是否显示实况显示浮窗
    property bool isLoadSetting: false//是否在加载配置
    property bool isExit: false//是否在退出状态
    property bool isStopDecode: false//是否停止解码
    property bool isGlitchRemovaling: false//是否毛刺过滤中
    property string selectDecodeID: ""//选中的协议
    property bool isBind: false//是否绑定视图中
}
