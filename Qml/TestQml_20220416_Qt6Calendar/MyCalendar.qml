import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

// qt6 qml日历样式自定义
// 龚建波 2025-09-19
Rectangle {
    id: control
    implicitWidth: 320
    implicitHeight: 350

    border.color: "#B8B8B8"

    property alias font: month_grid.font
    property alias locale: month_grid.locale
    property date selectDate: new Date()

    // 自定义按钮样式
    component CalendarButton : AbstractButton {
        id: c_btn
        implicitWidth: 30
        implicitHeight: 30
        contentItem: Text {
            font: control.font
            text: c_btn.text
            color: "#414141"
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
        background: Item{}
    }

    GridLayout {
        anchors.fill: parent
        anchors.margins: 2
        columns: 2
        columnSpacing: 1
        rowSpacing: 1

        Rectangle {
            implicitWidth: 30
            implicitHeight: 40
            border.color: "#B8B8B8"
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    // 日期复位
                    let cur_date = new Date();
                    month_grid.year = cur_date.getUTCFullYear();
                    month_grid.month = cur_date.getUTCMonth();
                }
            }
        }

        Rectangle {
            Layout.row: 0
            Layout.column: 1
            Layout.fillWidth: true
            implicitHeight: 40
            border.color: "#B8B8B8"
            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 20
                anchors.rightMargin: 20
                CalendarButton {
                    text: "<"
                    onClicked: {
                        month_grid.year -= 1;
                    }
                }
                Text {
                    font: control.font
                    color: "#414141"
                    text: month_grid.year
                }
                CalendarButton {
                    text: ">"
                    onClicked: {
                        month_grid.year += 1;
                    }
                }
                Item {
                    implicitWidth: 20
                }
                CalendarButton {
                    text: "<"
                    onClicked: {
                        if (month_grid.month === 0) {
                            month_grid.year -= 1;
                            month_grid.month = 11;
                        } else {
                            month_grid.month -= 1;
                        }
                    }
                }
                Text {
                    font: control.font
                    color: "#414141"
                    text: month_grid.month+1
                }
                CalendarButton {
                    text: ">"
                    onClicked: {
                        if (month_grid.month === 11) {
                            month_grid.year += 1;
                            month_grid.month = 0;
                        } else {
                            month_grid.month += 1;
                        }
                    }
                }
            }
        }

        Rectangle {
            implicitWidth: 30
            implicitHeight: 40
            border.color: "#B8B8B8"
        }

        // 星期1-7
        DayOfWeekRow {
            id: week_row
            Layout.row: 1
            Layout.column: 1
            Layout.fillWidth: true
            implicitHeight: 40
            spacing: 1
            topPadding: 0
            bottomPadding: 0
            font: control.font
            // locale设置会影响显示星期数中英文
            locale: control.locale
            delegate: Rectangle {
                border.color: "#B8B8B8"
                required property string shortName
                Text {
                    anchors.centerIn: parent
                    text: shortName
                    font: week_row.font
                    color: "#414141"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
            // 之前的版本可以在contentItem放Layout自己做布局，新版本内部做了处理会影响布局
            contentItem: Row {
                spacing: week_row.spacing
                Repeater {
                    model: week_row.source
                    delegate: week_row.delegate
                }
            }
        }

        // 左侧周数
        WeekNumberColumn {
            id: week_col
            Layout.row: 2
            Layout.fillHeight: true
            implicitWidth: 30
            spacing: 1
            leftPadding: 0
            rightPadding: 0
            font: control.font
            month: month_grid.month
            year: month_grid.year
            locale: control.locale
            delegate: Rectangle {
                border.color: "#B8B8B8"
                required property int weekNumber
                Text {
                    anchors.centerIn: parent
                    text: weekNumber
                    font: week_col.font
                    color: "#414141"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
            contentItem: Column {
                spacing: week_col.spacing
                Repeater {
                    model: week_col.source
                    delegate: week_col.delegate
                }
            }
        }

        // 日期单元格
        MonthGrid {
            id: month_grid
            Layout.fillWidth: true
            Layout.fillHeight: true
            // month: Calendar.December
            // year: 2022
            locale: Qt.locale("zh_CN")
            spacing: 1
            font{
                family: "SimHei"
                pixelSize: 14
            }
            delegate: Rectangle {
                color: model.today
                       ? "orange"
                       : control.selectDate.valueOf() === model.date.valueOf()
                         ? "lightBlue"
                         : "#F5F5F5"
                border.color: "#B8B8B8"
                Rectangle {
                    anchors.fill: parent
                    anchors.margins: 2
                    color: "transparent"
                    border.color: "gold"
                    visible: item_mouse.containsMouse
                }
                Text {
                    anchors.centerIn: parent
                    text: model.day
                    color: (model.month === month_grid.month) ? "#414141" : "#888888"
                }
                MouseArea {
                    id: item_mouse
                    anchors.fill: parent
                    hoverEnabled: true
                    acceptedButtons: Qt.NoButton
                }
            }
            onClicked: (date)=> {
                           control.selectDate = date;
                           console.log('click', month_grid.title, month_grid.year, month_grid.month, "--",
                                       date.getUTCFullYear(), date.getUTCMonth(), date.getUTCDate(), date.getUTCDay())
                       }
        }
    }
}
