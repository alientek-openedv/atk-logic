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
import atk.qml.Controls 1.0

QSetting {
    fileName: tempDir+"/set.ini"
    property bool isFirst: false
    property bool isInstantly: false
    property bool isOne: true
    property bool isSimpleTrigger: true
    property var channelsSet:[]
    property var settingData:{"triggerPosition":1}
    property var pwmData:[]
    property var favoritesList: []
    property var channelsDataColor: []
    property int channelHeightMultiple: 1
    property bool isMouseMeasure: true
}
