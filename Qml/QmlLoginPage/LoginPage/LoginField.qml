import QtQuick 2.12
import QtQuick.Templates 2.12 as T
import QtQuick.Controls 2.12
import QtQuick.Controls.impl 2.12

//自定义输入框
T.TextField{
    id: control
    color: "black"
    font{
        family: "SimSun"
        pixelSize: 15
    }

    padding: 6
    leftPadding: padding+4
    //placeholderText: "Input Password"
    placeholderTextColor: "gray"
    verticalAlignment: TextInput.AlignVCenter

    selectByMouse: true
    selectedTextColor: "white"
    selectionColor: "black"

    //作为密码框
    //显示模式：Normal普通文本，Password密码，NoEcho无显示，
    //PasswordEchoOnEdit显示在编辑时输入的字符，否则与相同TextInput.Password
    //echoMode: TextInput.Password
    //passwordCharacter: "*"
    passwordMaskDelay: 1000
    validator: RegExpValidator{
        regExp:/[a-zA-Z0-9]+/
    }
    //clip: true
    //inputMethodHints: Qt.ImhUrlCharactersOnly
    //renderType: Text.NativeRendering
    renderType: Text.QtRendering

    PlaceholderText {
        id: placeholder
        x: control.leftPadding
        y: control.topPadding
        width: control.width - (control.leftPadding + control.rightPadding)
        height: control.height - (control.topPadding + control.bottomPadding)

        text: control.placeholderText
        font: control.font
        color: control.placeholderTextColor
        verticalAlignment: control.verticalAlignment
        visible: !control.length && !control.preeditText && (!control.activeFocus || control.horizontalAlignment !== Qt.AlignHCenter)
        elide: Text.ElideRight
        renderType: control.renderType
    }
    background: Rectangle {
        radius: 6
        border.width: 1 //control.activeFocus ? 2 : 1
        color: "white"
        border.color: control.activeFocus
                      ? Qt.rgba(22/255,155/255,213/255,1)
                      : Qt.rgba(208/255,201/255,201/255,1)
    }
}
