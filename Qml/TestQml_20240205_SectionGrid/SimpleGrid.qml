import QtQuick 2.15
import QtQuick.Controls 2.15

// ListView 实现 GridView 效果
Rectangle {
    id: control

    border.color: "black"

    // 边距
    property int padding: 10
    // Item 间隔
    property int spacing: 10
    // Item 宽
    property int itemWidth: 300
    // Item 高
    property int itemHeight: 100
    // Delegate 宽
    property int delegateWidth: itemWidth + spacing
    // Delegate 高
    property int delegateHeight: itemHeight + spacing
    // 列数根据可视宽度和 Item 宽度计算
    property int columns: (list_view.width + spacing - padding) / delegateWidth < 1
                          ? 1
                          : (list_view.width + spacing - padding) / delegateWidth

    // 套一层 Item clip 剪去 ListView 尾巴上多余的部分不显示出来
    Item {
        anchors.fill: parent
        anchors.margins: control.padding
        // 右侧留下滚动条位置，所以 columns 里 list_view.width 要减一个 padding
        anchors.rightMargin: 0
        clip: true

        ListView {
            id: list_view
            width: parent.width
            // 高度多一个 delegate 放置 footer，防止末尾的一行滑倒底部后隐藏
            // 多出来的一部分会被外部 Item clip 掉
            height: parent.height + control.delegateHeight + control.spacing
            flickableDirection: Flickable.HorizontalAndVerticalFlick
            boundsBehavior: Flickable.StopAtBounds
            headerPositioning: ListView.OverlayHeader
            // 底部多一个 footer 撑高可显示范围，防止末尾的一行滑倒底部后隐藏
            footerPositioning: ListView.OverlayFooter
            ScrollBar.vertical: ScrollBar {
                // padding 加上 ListView 多出来的一部分
                bottomPadding: padding + (control.delegateHeight + control.spacing)
                // 常驻显示只是方便调试
                policy: ScrollBar.AlwaysOn
            }
            footer: Item {
                // 竖向的 ListView 宽度无所谓
                width: control.delegateWidth
                // 高度大于等于 delegate 高度才能保证显示
                height: control.delegateHeight
            }
            // 奇数方便测试
            model: 31
            delegate: Item {
                width: control.delegateWidth
                // 每行第一个 Item 有高度，后面的没高度，这样就能排列到一行
                // 因为 0 高度 Item 在末尾，超出范围 visible 就置为 false 了，所以才需要 footer 撑高多显示一行的内容
                // delegate 高度不一致会导致滚动条滚动时长度变化
                height: (model.index % control.columns === 0) ? control.delegateHeight : 0
                // 放置真正的内容
                Rectangle {
                    // 根据列号计算 x
                    x: (model.index % control.columns) * control.delegateWidth
                    // 负高度就能和每行第一个的 y 一样
                    y: (model.index % control.columns !== 0) ? -control.delegateHeight : 0
                    width: control.itemWidth
                    height: control.itemHeight
                    border.color: "black"
                    Text {
                        anchors.centerIn: parent
                        // 显示行号列号
                        text: "(%1,%2)".arg(
                                  parseInt(model.index / control.columns)).arg(
                                  model.index % control.columns)
                    }
                }
            }
        }
    }
}
