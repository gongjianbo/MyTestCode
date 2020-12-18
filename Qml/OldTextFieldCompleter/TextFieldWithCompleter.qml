import QtQuick 2.0
import QtQuick.Controls 2.3

//带提示的编辑框
//龚建波 2020-12-18
//原作者: OLEKSANDR KONSTANTINOV (Konstilio)
//原地址: https://github.com/Konstilio/QML-TextField-Completer
TextField {
    id: control

    signal editEnd(string text);

    property var completerModel // model of completer, works with Display Role so far
    property int __startPos: -1
    property bool dynamic: false

    height: 32
    width: (contentWidth<60?60:contentWidth)+leftPadding+rightPadding
    leftPadding: 8
    rightPadding: 10
    clip: true
    color: "#3D3E40"
    selectByMouse: true
    selectedTextColor: "white"
    selectionColor: "blue"
    placeholderTextColor: "#9FA1A5"
    autoScroll: true
    verticalAlignment: TextInput.AlignVCenter
    font{
        family: "Microsoft YaHei"
        pixelSize: 14
    }
    background: Rectangle{
        border.width: 2
        border.color: control.focus?"blue":"gray"
        radius: 4
    }
    onFocusChanged: {
        if(control.dynamic&&
                !completer_menu.visible&&
                !control.focus&&
                !control.readOnly){
            control.visible=false;
            control.destroy();
        }
    }

    Keys.onUpPressed: {
        if (completer_menu.visible)
            completer_menu.decreaseIndex();
    }

    Keys.onDownPressed: {
        if (completer_menu.visible)
            completer_menu.increaseIndex();
    }

    Keys.onReturnPressed: {
        if (completer_menu.visible)
            completer_menu.selectIndex();
    }

    MouseArea{
        anchors.fill: parent
        visible: control.readOnly
        onDoubleClicked: {
            control.readOnly=false;
            control.selectAll();
        }
    }

    Menu {
        id: completer_menu
        padding: 1
        width: control.width
        height: __contentItem.count*32+2
        closePolicy:Popup.CloseOnPressOutsideParent|
                    Popup.CloseOnReleaseOutsideParent|
                    Popup.CloseOnEscape
        contentItem: __contentItem
        ListView {
            id: __contentItem
            model: control.completerModel
            width: contentWidth
            height: contentHeight
            spacing: 0
            delegate: MenuItem {
                id: menu_item
                property bool isCurrent: model.index===__contentItem.currentIndex
                text: model.display
                width: completer_menu.width-2
                height: 32
                background: Rectangle {
                    color : (pressed ? "green" : (menu_item.isCurrent ? "lightblue" : "white"))
                }
                onTriggered: {
                    control.complete(text);
                }
            }
        }

        background: Rectangle {
            border.width: 1
            color: "white"
            border.color: "#D8DCE6"
            radius: 2
        }

        function increaseIndex() {
            __contentItem.incrementCurrentIndex();
        }

        function decreaseIndex() {
            __contentItem.decrementCurrentIndex();
        }

        function selectIndex() {
            //control.complete(menuModel.data(menuModel.index(_contentItem.currentIndex, 0), 0));
            control.complete(control.completerModel.data(control.completerModel.index(__contentItem.currentIndex, 0), 0));
        }

    }

    onTextEdited: {
        let filterText = getWord(text, cursorPosition);

        completerModel.setFilterWildcard(filterText + '*');

        if (completer_menu)
        {
            if (completerModel.rowCount() === 0 || filterText.length < 2)
            {
                completer_menu.close();
            }
            else
            {
                completer_menu.popup(control,0,control.height+1);
                control.forceActiveFocus();
            }
        }
    }

    function complete(completeText)
    {
        if (control.__startPos == -1 || control.__startPos >= text.length)
            return;

        let oldText = control.text;
        control.text = oldText.substring(0, control.__startPos) + completeText + oldText.substring(cursorPosition, oldText.length) //+ ' '
        //control.cursorPosition = _private.startPos + completeText.length + 1 // set cursor position
        control.__startPos = -1;
        //control.textModified();

        if (completer_menu)
            completer_menu.close();

        //选中后编辑结束
        let cur_text=control.text;
        if(control.dynamic){
            control.readOnly=true;
            control.focus=false;
        }else{
            control.text="";
        }

        control.editEnd(cur_text);
    }

    function getWord(text, position)
    {
        control.__startPos = -1;
        if (position > text.length)
            return "";
        let iPos=0;
        control.__startPos = iPos;
        return text.substring(iPos, position);
    }
}
