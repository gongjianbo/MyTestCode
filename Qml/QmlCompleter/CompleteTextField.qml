import QtQuick 2.12
import QtQuick.Controls 2.12
import Completer 1.0

//带提示选项的编辑框
//龚建波 2022-01-17
//参考: https://github.com/Konstilio/QML-TextField-Completer
TextField {
    id: control

    //选项高度
    property int itemHeight: 30
    //只选单个还是可以多次选，由符号分格多个词
    property bool single: true
    //提示的关键字列表
    property alias keywords: completer_proxy.keywords
    implicitHeight: 30
    implicitWidth: 200
    color: "black"
    selectByMouse: true
    selectedTextColor: "white"
    selectionColor: "blue"
    font{
        family: "Microsoft YaHei"
        pixelSize: 14
    }
    background: Rectangle{
        color: "white"
        border.color: "black"
    }

    Menu {
        id: completer_menu
        padding: 1
        width: control.width
        height: (completer_view.count > 10
                 ? 10
                 : completer_view.count) * control.itemHeight + 2
        closePolicy:Popup.CloseOnPressOutsideParent|
                    Popup.CloseOnReleaseOutsideParent|
                    Popup.CloseOnEscape
        contentItem: completer_view
        ListView {
            id: completer_view
            spacing: 0
            model: completer_proxy.filterModel
            interactive: false
            delegate: MenuItem {
                id: menu_item
                text: model.display
                width: completer_menu.width - 2
                height: control.itemHeight
                font: control.font
                property bool highlight: model.index === completer_view.currentIndex
                background: Rectangle {
                    color : (menu_item.pressed ? "green" : (menu_item.highlight ? "lightgreen" : "white"))
                }
                onTriggered: {
                    control.complete(text);
                }
            }
        }
        background: Rectangle {
            border.width: 1
            border.color: "black"
        }
    }

    CompleterProxy {
        id: completer_proxy
    }

    Keys.enabled: true
    Keys.onUpPressed: {
        if(completer_menu.visible){
            completer_view.decrementCurrentIndex();
        }
    }
    Keys.onDownPressed: {
        if(completer_menu.visible){
            completer_view.incrementCurrentIndex();
        }
    }
    Keys.onReturnPressed: {
        if(completer_menu.visible){
            control.selectItem();
        }
    }
    Keys.onEnterPressed: {
        if(completer_menu.visible){
            control.selectItem();
        }
    }

    onPressed: {
        control.textChange()
    }

    onTextEdited: {
        control.textChange()
    }

    //文本编辑后，重置sortfilter的过滤条件
    function textChange()
    {
        var filter_text = getWord(text, cursorPosition);
        completer_proxy.filterModel.setFilterWildcard(filter_text + '*');

        if(completer_menu){
            if(completer_view.count === 0 || filter_text.length < 1){
                completer_menu.close();
            }else{
                completer_menu.popup(control, 0, control.height + 1);
                control.forceActiveFocus();
            }
        }
    }

    //选中当前提示选项
    function selectItem()
    {
        var index = completer_proxy.filterModel.index(completer_view.currentIndex, 0);
        control.complete(completer_proxy.filterModel.data(index, 0));
    }

    //complete 设置当前文本
    //__startPos 多个词时记录当前词的起始位置
    property int __startPos: -1
    function complete(completeText)
    {
        if(control.single){
            control.text = completeText;
        }else{
            if(__startPos === -1 || __startPos >= text.length)
                return;
            var old_text = control.text;
            control.text = old_text.substring(0, __startPos) + completeText +
                    old_text.substring(cursorPosition, old_text.length) + ' ';
            // set cursor position
            control.cursorPosition = __startPos + completeText.length + 1;
            __startPos = -1;
        }

        if(completer_menu){
            completer_menu.close();
        }
    }

    //getWord 在编辑时找词的开始
    //endOfWord 多个词时用于判断词的分隔
    property string endOfWord: "~!#$%^&*()_+{}|:\"<>?,./;'[]\\-=\n "
    function getWord(text, position)
    {
        if(single){
            return text;
        }
        __startPos = -1;
        if (position > text.length)
            return "";
        var pos = position - 1;
        for (; pos >= 0; --pos)
        {
            if (endOfWord.indexOf(text.charAt(pos)) > -1)
            {
                break;
            }
        }
        pos += 1;
        if (pos >= position)
            return "";
        __startPos = pos;
        return text.substring(pos, position);
    }
}
