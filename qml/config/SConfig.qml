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
import Qt.labs.settings 1.0

QtObject {
    property var type
    property var name
    property bool isExpandSidebar: true
    property bool isSidebarContentShow: false
    property int sidebarContentType:0
    property var availableChannels: []
    property int loopState: -1
    property bool isMeasureStart: false
    property bool isVernierStart: false
    property bool isMeasureMove: false
    property bool isVernierMove: false
    property bool isChannelMethodRun: isMeasureStart||isVernierStart||isMeasureMove||isVernierMove
    property bool isRun: false
    property bool isBuffer: false
    property bool isRLE: false
    property var decodeJson: []
    property string filePath: ""
    property string setPath: ""
    property bool isErrorShow: false
    property bool isCloseNotDataChannels: false
    property bool pwm0Start: false
    property bool pwm1Start: false
    property int decodeColorIndex: 0
    property bool isLiveFollowingPopupShow: false
    property bool isLoadSetting: false
    property bool isExit: false
    property bool isStopDecode: false
    property bool isGlitchRemovaling: false
    property string selectDecodeID: ""
}
