import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtGraphicalEffects 1.15
import "../config"

Item {
    id: root
    width: pickerRect.width+10+previewItem.width
    height: pickerRect.height+10+huePicker.height

    property alias cursorWidth: pickerRect.cursorWidth
    property bool isAlpha: false
    readonly property color currentColor: pickerRect.currentColor

    function setColor(color) {
        if(typeof(color)==="undefined")
            return;
        pickerRect.transitionColor=color;
        hueSlider.x = (huePicker.width - hueSlider.width) * (Math.max(pickerRect.transitionColor.hsvHue, 0));
        colorPickerCursor.x = pickerRect.transitionColor.hsvSaturation * (pickerRect.width - cursorWidth);
        colorPickerCursor.y = (1.0 - pickerRect.transitionColor.hsvValue) * (pickerRect.height - cursorWidth);
        pickerRect.alpha = pickerRect.transitionColor.a;
    }

    Item {
        id: pickerRect
        width: 240 + cursorWidth
        height: 240 + cursorWidth

        property color transitionColor
        property real cursorWidth: 12
        property color hueColor: {
            let v = 1.0 - hueSlider.value;

            if (0.0 <= v && v < 0.16) {
                return Qt.rgba(1.0, 0.0, v / 0.16, alpha);
            } else if (0.16 <= v && v < 0.33) {
                return Qt.rgba(1.0 - (v - 0.16) / 0.17, 0.0, 1.0, alpha);
            } else if (0.33 <= v && v < 0.5) {
                return Qt.rgba(0.0, ((v - 0.33) / 0.17), 1.0, alpha);
            } else if (0.5 <= v && v < 0.76) {
                return Qt.rgba(0.0, 1.0, 1.0 - (v - 0.5) / 0.26, alpha);
            } else if (0.76 <= v && v < 0.85) {
                return Qt.rgba((v - 0.76) / 0.09, 1.0, 0.0, alpha);
            } else if (0.85 <= v && v <= 1.0) {
                return Qt.rgba(1.0, 1.0 - (v - 0.85) / 0.15, 0.0, alpha);
            } else {
                return "red";
            }
        }
        property real saturation: colorPickerCursor.x / (width - cursorWidth)
        property real brightness: 1 - colorPickerCursor.y / (height - cursorWidth)
        property color currentColor: Qt.hsva(hueSlider.value, saturation, brightness, alpha)
        property color __color: Qt.rgba(0, 0, 0, 255)
        property double alpha: 1.0

        function fromColor() {
            setColor(Qt.rgba(parseInt(redEditor.text) / 255.
                                        , parseInt(greenEditor.text) / 255.
                                        , parseInt(blueEditor.text) / 255.
                                        , isAlpha?parseInt(alphaEditor.text)/255:1));
        }

        function fromArgbColor() {
            __color = '#' + argbEditor.text;
            setColor(__color);
        }

        onCurrentColorChanged: {
            alphaEditor.text = (alpha * 255).toFixed(0);
            redEditor.text = (currentColor.r * 255).toFixed(0);
            greenEditor.text = (currentColor.g * 255).toFixed(0);
            blueEditor.text = (currentColor.b * 255).toFixed(0);
            if(isAlpha){
                if(currentColor.toString().replace("#", "").length===8)
                    argbEditor.text = currentColor.toString().replace("#", "");
                else
                    argbEditor.text = (alpha * 255).toString(16)+currentColor.toString().replace("#", "");
            }
            else
                argbEditor.text = currentColor.toString().replace("#", "");
        }

        Rectangle {
            x: pickerRect.cursorWidth * 0.5
            y: pickerRect.height - pickerRect.cursorWidth * 0.5
            width: pickerRect.height - pickerRect.cursorWidth
            height: pickerRect.width - pickerRect.cursorWidth
            rotation: -90
            transformOrigin: Item.TopLeft
            gradient: Gradient {
                GradientStop { position: 0.0; color: "white" }
                GradientStop { position: 1.0; color: pickerRect.hueColor }
            }
        }

        Rectangle {
            x: pickerRect.cursorWidth * 0.5
            y: pickerRect.cursorWidth * 0.5
            width: pickerRect.width - pickerRect.cursorWidth
            height: pickerRect.height - pickerRect.cursorWidth
            gradient: Gradient {
                GradientStop { position: 1.0; color: "#ff000000" }
                GradientStop { position: 0.0; color: "#00000000" }
            }
        }

        Rectangle {
            id: colorPickerCursor
            width: pickerRect.cursorWidth
            height: width
            border.color: "white"
            border.width: 1
            radius: width
            color: pickerRect.currentColor
        }

        MouseArea {
            x: pickerRect.cursorWidth
            y: pickerRect.cursorWidth
            anchors.fill: parent

            function handleCursorPos(x, y) {
                let halfWidth = pickerRect.cursorWidth * 0.5;
                colorPickerCursor.x = Math.max(0, Math.min(width , x + halfWidth) - pickerRect.cursorWidth);
                colorPickerCursor.y = Math.max(0, Math.min(height, y + halfWidth) - pickerRect.cursorWidth);
            }

            onPositionChanged: (mouse) => handleCursorPos(mouse.x, mouse.y);
            onPressed: {
                handleCursorPos(mouse.x, mouse.y);
            }
        }
    }

    Rectangle {
        id: previewItem
        width: 64
        height: 64
        anchors.left: pickerRect.right
        anchors.leftMargin: 10 - pickerRect.cursorWidth/2
        anchors.top: pickerRect.top
        anchors.topMargin: pickerRect.cursorWidth/2
        color: pickerRect.currentColor
        border.width: 1
        border.color: Config.borderLineColor
    }

    component ColorEditor: ColumnLayout {
        id: __layout
        width: previewItem.width
        height: label.height+20

        property alias label: label.text
        property alias text: input.text
        property alias validator: input.validator

        signal textEdited();
        signal accepted();
        signal tabClick();

        Item{
            Layout.fillWidth: true
            height: label.height
            Text {
                id: label
                color: Config.textColor
                font.pixelSize: 12
                height: font.pixelSize
                width: parent.width
            }
        }

        Rectangle {
            clip: true
            color: Config.borderBackgroundColor
            border.color: Config.lineColor
            border.width: 1
            height: 20
            Layout.fillWidth: true
            radius: 4

            TextInput {
                id: input
                leftPadding: 5
                rightPadding: 5
                selectionColor: Config.mouseCheckColor
                selectByMouse: true
                anchors.fill: parent
                font.pixelSize: 12
                color: Config.textColor
                verticalAlignment: TextInput.AlignVCenter
                onTextEdited: __layout.textEdited();
                onAccepted: __layout.accepted();
                Keys.enabled: true
                Keys.onPressed: {
                    if(event.key===Qt.Key_Tab)
                    {
                        tabClick();
                        event.accepted = true;
                    }
                }

                onActiveFocusChanged: __layout.accepted();

                MouseArea{
                    anchors.fill: parent
                    cursorShape: Qt.IBeamCursor
                    acceptedButtons: Qt.NoButton
                }
            }
        }

        function active(){
            input.focus=true;
            input.accepted();
            input.selectAll();
        }
    }

    Column {
        anchors.top: previewItem.bottom
        anchors.topMargin: 10
        anchors.left: previewItem.left
        spacing: 9

        ColorEditor {
            id: alphaEditor
            label: "透明(A)"
            visible: isAlpha
            validator: IntValidator { top: 255; bottom: 0 }
            onAccepted: pickerRect.fromColor();
            onTabClick: argbEditor.active();
        }

        ColorEditor {
            id: redEditor
            label: "红色(R)"
            validator: IntValidator { top: 255; bottom: 0 }
            onAccepted: pickerRect.fromColor();
            onTabClick: greenEditor.active();
        }

        ColorEditor {
            id: greenEditor
            label: "绿色(G)"
            validator: IntValidator { top: 255; bottom: 0 }
            onAccepted: pickerRect.fromColor();
            onTabClick: blueEditor.active();
        }

        ColorEditor {
            id: blueEditor
            label: "蓝色(B)"
            validator: IntValidator { top: 255; bottom: 0 }
            onAccepted: pickerRect.fromColor();
            onTabClick: argbEditor.active();
        }

        ColorEditor {
            id: argbEditor
            label: "十六进制(H)"
            validator: RegularExpressionValidator { regularExpression: /[0-9a-fA-F]{0,8}/ }
            onAccepted: pickerRect.fromArgbColor();
            onTabClick: redEditor.active();
        }
    }

    Rectangle {
        id: huePicker
        width: pickerRect.width - pickerRect.cursorWidth
        height: 26
        anchors.top: pickerRect.bottom
        anchors.topMargin: 10 - pickerRect.cursorWidth/2
        anchors.left: pickerRect.left
        anchors.leftMargin: pickerRect.cursorWidth/2
        clip: true

        gradient: Gradient {
            orientation: Gradient.Horizontal
            GradientStop { position: 0.0;  color: "#ff0000" }
            GradientStop { position: 0.16; color: "#ffff00" }
            GradientStop { position: 0.33; color: "#00ff00" }
            GradientStop { position: 0.5;  color: "#00ffff" }
            GradientStop { position: 0.76; color: "#0000ff" }
            GradientStop { position: 0.85; color: "#ff00ff" }
            GradientStop { position: 1.0;  color: "#ff0000" }
        }

        Rectangle {
            id: hueSlider
            width: 14
            height: parent.height
            anchors.verticalCenter: parent.verticalCenter
            border.color: "white"
            border.width: 1
            clip: true
            color: pickerRect.hueColor
            property real value: x / (parent.width - width)
        }

        MouseArea {
            anchors.fill: parent
            function handleCursorPos(x) {
                let halfWidth = hueSlider.width * 0.5;
                hueSlider.x = Math.max(0, Math.min(width, x + halfWidth) - hueSlider.width);
            }
            onPressed: handleCursorPos(mouse.x);
            onPositionChanged: handleCursorPos(mouse.x);
        }
    }

    function allEnter(){
        pickerRect.focus=true
        pickerRect.forceActiveFocus();
    }
}
