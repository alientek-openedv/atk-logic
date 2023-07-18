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

import QtQuick 2.15

QtObject {
    signal channelNameSignalChanged(var channelID,var channelName)
    signal channelHeightChanged(var channelID,var cHeight)
    signal channelRefresh(var channelID)
    signal channelColorChanged()
    signal setChannelDisable(var channelID, var isDisable)
    signal setChannelCursor(var cursor)
    signal channelWheel(var isUp)
    signal start(var state)
    signal stop(var state)
    signal collectComplete()
    signal setSidebarContentType(var sidebarContentType)
    signal sessionSelect()
    signal sideBarCanLoader()
    signal refreshZoom(var state)
    signal decode(var decodeID,var decodeJson,var channelList)
    signal resetAllDecode()
    signal resetDecode(var decodeID,var decodeJson,var channelList, var isDelModel)
    signal resetDecodeComplete(var decodeID)
    signal resetDecodeJson(var decodeJson, var decodeID)
    signal removeDecode(var decodeID,var state_)
    signal removeAllDecode()
    signal editDecode(var decodeID)
    signal refreshChannelHeight()
    signal measureStart()
    signal measureEnd()
    signal measurePopupAppend(var measureID)
    signal measureRefreshData(var measureID)
    signal measureRefreshY()
    signal measureRefreshShow()
    signal measureRemove(var measureID)
    signal measureRemoveAll()
    signal vernierCreate(var state)
    signal getChannelY(var channelID,var type,var y)
    signal sendChannelY(var channelID,var type, var y)
    signal vernierMoveComplete(var id)
    signal showVernierPopup(var vernierID, var mouse_x)
    signal closeVernierPopup()
    signal showMouseMeasurePopup(var x_,var channelID,var period,var freq,var duty)
    signal closeMouseMeasurePopup()
    signal setMouseMeasure(var isCheck)
    signal setLiveFollowing(var isEnable,var visible_)
    signal showCollectorSchedule(var schedule,var visible_)
    signal openTestFileSelectPopup()
    signal closeNotDataChannels()
    signal decodeDocPopupShow(var isShow, var note)
    signal showMouseZoom(var show, var x, var y, var width, var height)
    signal refreshEdgeButton()
    signal showSetTips(var type, var visible_)
    signal vernierDataChanged(var vernierID)
    signal refreshMethodsRadioGroup()
    signal searchModelChanged()
    signal exit(var sessionID_)
    signal resetSearch()
}
