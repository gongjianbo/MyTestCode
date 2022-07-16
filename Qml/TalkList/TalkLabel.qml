import QtQuick 2.12
import QtQuick.Controls 2.12

//文本
//Text不能选中文本，所以用编辑框来做label
//龚建波 2021-3-29
TextEdit {
    id: control

    property int textWidth: private_text.implicitWidth

    padding: 14
    font{
        family: "Microsoft YaHei"
        pixelSize: 14
    }
    color: "#666666"

    verticalAlignment: TextEdit.AlignVCenter
    horizontalAlignment: TextEdit.AlignLeft
    readOnly: true

    selectByMouse: true
    selectionColor: "black"
    selectedTextColor: "white"
    wrapMode: TextEdit.WrapAnywhere
    //background: Item{ }

    //用TextEdit的implicitWidth/contentWidth来算width总是会Binding loop
    //故拿一个Text计算文本宽度用于判断
    Text{
        id: private_text
        visible: false
        font: control.font
        padding: control.padding
        text: control.text
    }
}
