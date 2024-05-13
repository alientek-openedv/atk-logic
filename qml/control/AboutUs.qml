import QtQuick 2.15
import QtQuick.Controls 2.5
import "../config"
import "../style"

Rectangle {
    id: rootRectangle
    visible: false
    color: "#7d000000"
    z: 1000

    Connections{
        target: Signal
        function onEscKeyPress()
        {
            rootRectangle.visible=false
        }
    }

    MouseArea{
        anchors.fill: parent
        preventStealing: true
        propagateComposedEvents: false
        hoverEnabled: true
        acceptedButtons: Qt.AllButtons
    }

    Image {
        anchors.centerIn: parent
        source: "../../resource/image/AboutUsDropShadow.png"
        width: 506
        height: mainRectangle.height+16
        Rectangle{
            id: mainRectangle
            width: 490
            height: titleText_.height+companyText.height+explainText.height+phoneText.height+180
            radius: 8
            color: Config.backgroundColor
            anchors.centerIn: parent
            Text {
                id: titleText_
                text: qsTr("关于我们")
                font.pixelSize: 18
                anchors{
                    top: parent.top
                    horizontalCenter: parent.horizontalCenter
                    margins: 10
                }
                color: Config.textColor
            }

            ImageButton{
                width: 15
                height: 15
                imageWidth: 10
                imageHeight: 10
                imageSource: "resource/icon/MessageClose.png"
                imageEnterSource: "resource/icon/MessageClose.png"
                anchors{
                    right: parent.right
                    rightMargin: 10
                    top: parent.top
                    topMargin: 15
                }
                onPressed: rootRectangle.visible=false
            }

            Rectangle{
                id: splitRectangle
                height: 1
                anchors{
                    top: titleText_.bottom
                    topMargin: 10
                    left: parent.left
                    right: parent.right
                }
                color: Config.lineColor
            }

            Text{
                id: companyText
                text: qsTr("广州市星翼电子科技有限公司")
                font{
                    pixelSize: 14
                    bold: true
                }
                anchors{
                    top: splitRectangle.bottom
                    topMargin: 20
                    left: parent.left
                    leftMargin: 15
                }
                color: Config.textColor
            }
            Text {
                id: explainText
                text: qsTr("我们是一家从事嵌入式开发工具、仪表仪器、物联网、工业控制和企业服务等软硬件研发、销售一体化的国家高新技术企业。欢迎来电合作洽谈。")
                font.pixelSize: 14
                width: parent.width-30
                lineHeight: 1.3
                wrapMode: Text.WordWrap
                anchors{
                    top: companyText.bottom
                    topMargin: 5
                    left: parent.left
                    leftMargin: companyText.anchors.leftMargin
                }
                color: Config.textColor
            }
            Text{
                id: phoneText
                text: qsTr("电话")+"：020-38271790"
                font.pixelSize: 14
                anchors{
                    top: explainText.bottom
                    topMargin: 25
                    left: parent.left
                    leftMargin: companyText.anchors.leftMargin
                }
                color: Config.textColor
            }
            Row{
                id: emailRow
                anchors{
                    top: phoneText.bottom
                    topMargin: 5
                    left: parent.left
                    leftMargin: companyText.anchors.leftMargin
                }
                Text{
                    text: qsTr("邮箱")+"："
                    font.pixelSize: 14
                    color: Config.textColor
                }
                QLineText{
                    text: "zhangyang@alientek.com"
                    openUrl: "mailto: zhangyang@alientek.com"
                }
            }

            Text {
                id: statementText
                text: "Copyright © 2022. ALIENTEK All Rights Reserved."
                font.pixelSize: 10
                color: Config.subheadColor
                anchors{
                    bottom: parent.bottom
                    bottomMargin: 15
                    horizontalCenter: parent.horizontalCenter
                }
            }
            Row{
                spacing: 10
                anchors{
                    bottom: statementText.top
                    bottomMargin: 7
                    horizontalCenter: parent.horizontalCenter
                }
                QLineText{
                    text: qsTr("资料链接")
                    openUrl: "http://www.openedv.com/docs/tool/luojifenxiyi/DL16.html"
                }
                QLineText{
                    text: qsTr("软件仓库")
                    openUrl: "http://www.openedv.com/thread-336889-1-1.html"
                }
                QLineText{
                    text: qsTr("技术论坛")
                    openUrl: "http://www.openedv.com/forum.php"
                }
                QLineText{
                    text: qsTr("官方商城")
                    openUrl: "http://zhengdianyuanzi.tmall.com"
                }
                QLineText{
                    text: qsTr("官方网站")
                    openUrl: "http://www.alientek.com"
                }
                QLineText{
                    id: tencentText
                    text: qsTr("公众号")
                    onMouseEnter: tencentPopup.open()
                    onMouseExit: tencentPopup.close()
                }
                Popup{
                    id: tencentPopup
                    parent: tencentText
                    closePolicy: Popup.CloseOnPressOutsideParent
                    y: tencentText.y-height-5
                    height: 150
                    width: 150
                    padding: 0
                    background: Rectangle{
                        radius: 8
                        border{
                            width: 1
                            color: Config.lineColor
                        }
                        Image {
                            anchors{
                                fill: parent
                                margins: 5
                            }
                            source: "../../resource/image/QRcode.png"
                        }
                    }
                    Component.onCompleted: x=x-width/2+parent.width/2
                }
            }
        }
    }
}
