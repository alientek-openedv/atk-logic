import QtQuick 2.15

QtObject {
    signal channelNameSignalChanged(var channelID,var channelName)//通道名称更改
    signal channelHeightChanged(var channelID,var cHeight);//通道高度更改
    signal channelRefresh(var channelID)//通道刷新，-1代表全部刷新，-2代表全部刷新，除了滚动条，-3代表全部刷新，除了协议表格滚动条
    signal channelColorChanged();//通道颜色修改
    signal setChannelDisable(var channelID, var isDisable);//协议专用，隐藏或显示通道
    signal setChannelCursor(var cursor);//通道鼠标样式设置
    signal channelWheel(var isUp)//通道滚轮
    signal channelXWhell(var isUp)//横向移动
    signal start(var state)//启动采集
    signal stop(var state)//停止采集
    signal collectComplete();//采集完成
    signal setSidebarContentType(var sidebarContentType)//右侧边栏弹窗
    signal sessionSelect(var isSelect)//会话被选中
    signal sideBarCanLoader()//侧边栏允许初始化
    signal refreshZoom(var state)//刷新放大缩小
    signal decode(var decodeID,var decodeJson,var channelList)//解码事件
    signal resetAllDecode();//重设所有Decode
    signal resetDecode(var decodeID,var decodeJson,var channelList, var isDelModel)//编辑重设解码行
    signal resetDecodeComplete(var decodeID);//重设解码完成
    signal resetDecodeJson(var decodeJson, var decodeID);//刷新解码json，仅供显示使用
    signal removeDecode(var decodeID,var state_);//删除解码
    signal removeAllDecode();//删除所有解码
    signal editDecode(var decodeID);//编辑解码
    signal refreshChannelHeight()//刷新高度
    signal measureStart()//进入通道标记模式
    signal measureEnd()//结束通道标记模式
    signal measurePopupAppend(var measureID)//添加测量弹窗
    signal measureRefreshData(var measureID)//刷新数据
    signal measureRefreshY()//刷新窗口Y轴
    signal measureRefreshShow()//刷新弹窗显示属性
    signal measureRemove(var measureID)//删除测量
    signal measureRemoveAll()//删除所有测量
    signal vernierCreate(var state)//进入标签标记模式，state=0 进入模式，1=已经创建标签完成，2=完成
    signal vernierCancelMove();//取消标签移动
    signal getChannelY(var channelID,var type,var y)//通知需求通道Y轴，y只有在type=3才有用，type=0测量浮窗，1鼠标浮窗，2协助计算右击放大区域，3协助标签吸附，4-5协助跨通道测量
    signal sendChannelY(var channelID,var type, var y)//发送通道Y轴
    signal vernierMoveComplete(var id);//标签移动完成
    signal showVernierPopup(var vernierID, var mouse_x);//显示弹窗
    signal closeVernierPopup();//关闭弹窗
    signal showMouseMeasurePopup(var x_,var channelID,var period,var freq,var duty);//显示鼠标弹窗
    signal closeMouseMeasurePopup();//关闭鼠标弹窗
    signal setLiveFollowing(var isEnable,var visible_);//设置实况跟随,0=不设置，1=开，2=关
    signal showCollectorSchedule(var schedule,var visible_);//采集按钮显示进度
    signal openTestFileSelectPopup();//显示选择测试文件选择浮窗
    signal closeNotDataChannels();//关闭没有数据的通道
    signal decodeDocPopupShow(var isShow, var note);//协议文档弹窗显示或关闭
    signal showMouseZoom(var show, var x, var y, var width, var height);//绘制鼠标右键矩形
    signal refreshEdgeButton();//刷新触发按钮
    signal showSetTips(var type, var visible_);//显示问号提示
    signal vernierDataChanged(var vernierID);//刷新游标数据
    signal refreshMethodsRadioGroup();//刷新运行模式，主要用在浮窗点击不同状态运行采集时，set窗口按钮状态不同步
    signal searchModelChanged();//搜索界面标签修改
    signal exit(var sessionID_);//退出会话
    signal resetSearch();
    signal closeAllPopup();//关闭所有弹窗;
    signal stopXWheel();//停止XWheel移动
    signal crossChannelMeasureState(var isStop);//跨通道测量模式
    signal sendCrossChannelMeasurePosition(var type, var x, var y, var mouseY, var position, var isHit);//发送跨通道测量模式数据
}
