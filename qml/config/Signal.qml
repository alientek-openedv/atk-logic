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
    signal closeApp()
    signal escKeyPress()
    signal showInitText(var state, var text)
    signal saveDataEvent()
    signal saveData(var path, var savePath, var saveType)
    signal tabSelected(var sessionID)
    signal setCursor(var cursor)
    signal clickMenuTabButton(var buttonIndex,var isDoubleClick)
    signal menuStateChanged(var state)
    signal menuRunStateChanged(var isRun, var isLoop, var isData, var savePath)
    signal refreshMenuTabIcon(var sidebarContentType, var sessionType)
    signal showZoomString(var text)
    signal setLanguage(var path)
    signal setStateText(var text)
    signal sessionCloseClicked(var sessionID, var state_)
    signal hotKey(var type)
    signal hotkeyMeasureStart()
    signal focusOut()
    signal openFile()
    signal showUpdateTag(var isHardwareUpdate, var isShow)
    signal showError(var str)
    signal importProject(var path)
    signal resetConnectDevice(var sessionID, var port, var complete)
    signal closeAppSaveNext()
    signal runScheduleMove(var sessionID, var isRun)
}
