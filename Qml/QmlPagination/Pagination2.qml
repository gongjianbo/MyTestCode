import QtQuick 2.12
import QtQuick.Controls 2.12

//自定义分页组件
//龚建波 2020-12-19
//1.负责页码展示和发送翻页请求，实际页码计算由后端or业务逻辑进行
//每次请求返回后根据计算好的页码设置值，pageCurrent、itemCount
//2.可以在返回之前ui先根据之前的参数计算用于展示，但要注意绑定关系
//实现中我对pageCurrent直接赋值，要绑定需要加Connections
Rectangle {
    id: control

    //请求某一页
    //page: 页码
    //count: 一页总数
    signal requestPage(int page,int count)

    //需求中要展示的信息：数据总条数，每页条数，当前页，页跳转
    //当前页
    property int pageCurrent: 0
    //数据总数
    property int itemCount: 0
    //总页数
    property int pageCount: itemCount>0?Math.ceil(itemCount/__itemPerPage):0
    //每页条数，这个和box关联的，暂时默认初始化
    property int __itemPerPage: 20
    //页码按钮显示个数
    property int pageButtonCount: 5
    property int __pageButtonHalf: Math.floor(pageButtonCount/2)+1
    property font font
    font{
        pixelSize: 14
        family: "Microsoft YaHei"
    }
    property color textColor: "#666666"
    property color highlightedColor: "#1989FA"
    property color elementBorderColor: "#DCDFE6"
    property int elementHeight: 30
    property alias padding: content_row.padding

    //color: "transparent"
    //border.color: "gray"
    //radius: 4
    implicitHeight: 40
    implicitWidth: content_row.width

    Row{
        id: content_row
        height: control.height
        spacing: 25
        padding: 10

        //总条数
        Text{
            anchors.verticalCenter: parent.verticalCenter
            font: control.font
            color: control.textColor
            text: "共 "+control.itemCount+" 条"
        }

        //每页多少条
        Row{
            spacing: 5
            anchors.verticalCenter: parent.verticalCenter
            Text{
                anchors.verticalCenter: parent.verticalCenter
                font: control.font
                color: control.textColor
                text: "每页"
            }
            ComboBox{
                id: item_page_box
                width: 90
                height: control.elementHeight
                model: [10,20,50,100]
                currentIndex: 1
                font: control.font
                onCurrentIndexChanged: {
                    //选择每页数后重新请求数据
                    //currentText此时还未修改
                    //设置页码后从1开始
                    control.__itemPerPage=model[currentIndex];
                    control.pageCurrent=1;
                    control.requestPage(control.pageCurrent,control.__itemPerPage);
                }
            }
            Text{
                anchors.verticalCenter: parent.verticalCenter
                font: control.font
                color: control.textColor
                text: "条"
            }
        }
        //页码按钮
        Row{
            spacing: 5
            anchors.verticalCenter: parent.verticalCenter
            //左侧前一页，应该使用图片，这里仅作演示
            PageButton{
                visible: control.pageCount>1
                enabled: control.pageCurrent>1
                anchors.verticalCenter: parent.verticalCenter
                width: 30
                height: 30
                font: control.font
                textColor: control.textColor
                highlightedColor: control.highlightedColor
                text: "<"
                onClicked: {
                    control.calcNewPage(control.pageCurrent-1);
                }
            }
            //中间显示页码
            Row{
                spacing: 5
                anchors.verticalCenter: parent.verticalCenter
                //第一页
                PageButton{
                    visible: control.pageCount>0
                    width: 30
                    height: control.elementHeight
                    font: control.font
                    textColor: control.textColor
                    highlightedColor: control.highlightedColor
                    pageNumber: 1
                    pageCurrent: control.pageCurrent
                    onClicked: {
                        control.calcNewPage(1);
                    }
                }
                Text {
                    //pageCount<btnCount不用显示
                    //当前页在前countHalf页不用显示
                    visible: (control.pageCount>control.pageButtonCount&&
                              control.pageCurrent>control.__pageButtonHalf)
                    text: "..."
                    font: control.font
                    color: control.textColor
                    height: control.elementHeight
                }
                //中间的页码由Repeater根据设置的pageButtonCount创建
                Repeater{
                    id: button_repeator
                    model: (control.pageCount<2)
                           ?0
                           :(control.pageCount>=control.pageButtonCount)
                             ?(control.pageButtonCount-2)
                             :(control.pageCount-2)
                    delegate: PageButton{
                        width: 30
                        height: control.elementHeight
                        font: control.font
                        textColor: control.textColor
                        highlightedColor: control.highlightedColor
                        //在首或者尾相连时，值要连续，避免和首尾重复
                        pageNumber: (control.pageCurrent<=control.__pageButtonHalf)
                                    ?(2+index)
                                    :(control.pageCount-control.pageCurrent<=control.pageButtonCount-control.__pageButtonHalf)
                                      ?(control.pageCount-button_repeator.count+index)
                                      :(control.pageCurrent+2+index-control.__pageButtonHalf)
                        pageCurrent: control.pageCurrent
                        onClicked: {
                            control.calcNewPage(pageNumber);
                        }
                    }
                }
                Text {
                    id: page_moreright
                    //pageCount<btnCount不用显示
                    //当前页在倒数countHalf页不用显示
                    visible: (control.pageCount>control.pageButtonCount&&
                              control.pageCount-control.pageCurrent>control.pageButtonCount-control.__pageButtonHalf)
                    text: "..."
                    font: control.font
                    color: control.textColor
                    height: control.elementHeight
                }
                //最后一页
                PageButton{
                    visible: control.pageCount>1
                    width: 30
                    height: control.elementHeight
                    font: control.font
                    textColor: control.textColor
                    highlightedColor: control.highlightedColor
                    pageNumber: control.pageCount
                    pageCurrent: control.pageCurrent
                    onClicked: {
                        control.calcNewPage(pageNumber);
                    }
                }
            }
            //右侧下一页，应该使用图片，这里仅作演示
            PageButton{
                visible: control.pageCount>1
                enabled: control.pageCurrent<control.pageCount
                anchors.verticalCenter: parent.verticalCenter
                width: 30
                height: 30
                font: control.font
                textColor: control.textColor
                highlightedColor: control.highlightedColor
                text: ">"
                onClicked: {
                    control.calcNewPage(control.pageCurrent+1);
                }
            }
        }
        //页码跳转
        Row{
            spacing: 5
            anchors.verticalCenter: parent.verticalCenter
            Text{
                anchors.verticalCenter: parent.verticalCenter
                font: control.font
                color: control.textColor
                text: "前往"
            }
            TextField{
                id: edit_goto_page
                width: 60
                height: control.elementHeight
                text: "1"
                font: control.font
                color: control.textColor
                validator: IntValidator{
                    bottom: 0
                    top: 999999
                }
                selectByMouse: true
                selectedTextColor: "white"
                selectionColor: "blue"
                background: Rectangle{
                    border.color: control.elementBorderColor
                }

                Keys.onPressed: {
                    //enter小键盘enter
                    //return主键盘enter
                    if(event.key===Qt.Key_Enter||event.key===Qt.Key_Return){
                        focus=false;
                        if(edit_goto_page.text){
                            //编辑结束就去请求该页数据
                            control.requestPage(Number(edit_goto_page.text),control.__itemPerPage);
                        }
                    }
                }
            }
            Text{
                anchors.verticalCenter: parent.verticalCenter
                font: control.font
                color: control.textColor
                text: "页"
            }
        }
    }

    //翻页计算
    function calcNewPage(page)
    {
        if(!page)
            return;
        let page_num=Number(page);
        //超出范围或者为当前页就返回
        if(page_num<1||page_num>control.pageCount||page_num===control.pageCurrent)
            return;
        control.pageCurrent=page_num;
        control.requestPage(page_num,control.__itemPerPage)
    }
}
