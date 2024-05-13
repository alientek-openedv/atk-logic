pragma Singleton
import QtQuick 2.15

QtObject {
    signal closeApp();//关闭软件
    signal escKeyPress();//Esc按键被按下
    signal showInitText(var state, var text);//加载程序文本显示,state:0=关闭，1=显示
    signal saveDataEvent();//模拟保存数据
    signal saveData(var path, var savePath, var saveType, var argument);//保存数据事件，saveType:0=数据csv,1=协议表格csv,2=工程atkdl,3=Bin数据
    signal tabSelected(var sessionID);//tab按钮选中
    signal setCursor(var cursor);//全局鼠标样式设置
    signal clickMenuTabButton(var buttonIndex,var isDoubleClick)//菜单栏按钮点击
    signal menuStateChanged(var state)//菜单栏状态控制
    signal menuRunStateChanged(var isRun, var isLoop, var isData, var savePath)//菜单栏采集状态控制图标
    signal refreshMenuTabIcon(var sidebarContentType, var sessionType)//菜单栏选项卡图标刷新
    signal showZoomString(var text)//显示缩放文本
    signal setLanguage(var path)//设置语言
    signal setStateText(var text)//设置状态栏文本
    signal sessionCloseClicked(var sessionID, var state_);//会话关闭按钮被点击
    signal hotKey(var type);//启动快捷键
    signal hotkeyMeasureStart();//标记快捷键
    signal focusOut();//窗口丢失焦点
    signal openFile();//打开文件
    signal showUpdateTag(var isHardwareUpdate, var isShow);
    signal showError(var str);
    signal importProject(var path);//导入工程
    signal resetConnectDevice(var sessionID, var port, var complete);//重新连接设备
    signal closeAppSaveNext();//关闭软件保存下一个文件
    signal runScheduleMove(var sessionID, var isRun)//显示页面按钮进度条
    signal sendExportPath(var path);//导出路径被选择
    signal showExportBinPopup();//显示导出窗口
    signal setDecodeConfig(var decodeName, var attributeName, var data);//设置协议习惯
}
