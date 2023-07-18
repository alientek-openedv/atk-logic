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
import QtQuick.Controls 2.5
import "../config"

Item{
    property alias modelData: repeater.model
    property int fontSize: 11
    property var selectButton
    property bool disable: false
    property int itemWidth: Math.floor(rootItem.width/repeater.model.count)
    id: rootItem

    Row {
        id: rootRow
        width: parent.width
        height: parent.height
        clip: true
        Repeater{
            id: repeater
            Button{
                width: itemWidth
                height: rootItem.height
                clip: true
                enabled: !disable
                Text {
                    id: btnText
                    text: buttonText
                    anchors.centerIn: parent
                    font.pixelSize: fontSize
                    color: disable?Config.textDisabledColor:selectButton===buttonText?"white":Config.textColor
                }

                background: CurvedRectangle{
                    width: parent.width
                    height: parent.height
                    radius: 8
                    cornersRadius: index===0?[0,1,1,0]:index===repeater.model.count-1?[1,0,0,1]:[1,1,1,1]
                    color: selectButton===buttonText?Config.mouseCheckColor:Config.borderBackgroundColor
                    Behavior on color{
                        ColorAnimation {
                            duration: 200
                        }
                    }
                    clip: true
                }
                onPressed: selectButton=buttonText
            }
        }
    }

    Rectangle{
        width: parent.width
        height: parent.height
        radius: 8
        color: "transparent"
        border{
            color: Config.borderLineColor
            width: 1
        }
    }
}



