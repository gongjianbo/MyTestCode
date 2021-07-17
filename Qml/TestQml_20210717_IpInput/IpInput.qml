import QtQuick 2.12
import QtQuick.Controls 2.12

//参照win ip地址输入框：
//ok:填完三个数字自动到下一段
//--也可以扩展下，如输入28明显不会有第三位，就可以去下一段了
//--win自带的效果是超出范围就弹提示框
//ok:删除完再按删除自动删除上一段末尾字符
//ok:按左右可以跨段移动光标
//ok:任意段ctrl+v，粘贴ip，粘贴完整格式的ip（格式带小数点）或者单个段
//--粘贴单个段时是覆盖原内容，不是在光标处插入
//ok:任意段ctrl+c，复制ip，单个段选中内容则复制选中，否则复制完整格式的ip
//ok:按小数点切换到下一段并全选，（可以加一个tab快捷键）
//--原效果左右按不会全选，小数点主动切换会全选该段
//--没有上下加减数字功能，原效果同按左右
//todo没有处理和其他组件的焦点切换
Rectangle {
    id: control

    width: 300
    height: 40
    color: "lightGray"
    border.color: "black"
    property int _itemWidth: (width-3*style.width)/4

    Text{
        id: style
        visible: false
        text: "·"
        font{
            pixelSize: 16
            family: "Microsoft Yahei"
        }
        color: "red"
    }

    Row {
        spacing: 0
        anchors.verticalCenter: parent.verticalCenter
        Repeater{
            id: repeater
            model: 7
            IpItem {
                //height: control.height
                width: (index%2==1)?style.width:control._itemWidth
                enabled: (index%2==0)
                text: (index%2==1)?style.text:""
                font: style.font
                color: style.color
                //到上一段
                onGotoPrev: {
                    if(index%2==0 && index-2>=0){
                        let item=repeater.itemAt(index-2);
                        item.forceActiveFocus();
                        item.cursorPosition=cursorLeft?0:item.text.length;
                        if(checkAll)
                            item.selectAll();
                    }
                }
                //到下一段
                onGotoNext: {
                    if(index%2==0 && index+2<7){
                        let item=repeater.itemAt(index+2);
                        item.forceActiveFocus();
                        item.cursorPosition=cursorLeft?0:item.text.length;
                        if(checkAll)
                            item.selectAll();
                    }
                }
                //复制
                onDoCopy: {
                    let ip_list;
                    if(copyOne){
                        ip_list=[selectedText];
                    }else{
                        ip_list=[repeater.itemAt(0).text,
                                 repeater.itemAt(2).text,
                                 repeater.itemAt(4).text,
                                 repeater.itemAt(6).text]
                    }
                    clipboard.copyIp(ip_list);
                }
                //粘贴
                onDoPaste: {
                    let ip_list=clipboard.pasteIp();
                    if(ip_list.length===4){
                        repeater.itemAt(0).text=ip_list[0];
                        repeater.itemAt(2).text=ip_list[1];
                        repeater.itemAt(4).text=ip_list[2];
                        repeater.itemAt(6).text=ip_list[3];
                    }else if(ip_list.length===1){
                        text=ip_list[0];
                    }else{
                        tip.show("The pasted IP format is wrong.",2000)
                    }
                }
            }
        }
    }

    ToolTip{
        id: tip
    }
}
