import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import Test 2.15

MyWindow {
    id: root
    width: 640
    height: 480
    visible: true
    title: "TestDpi " + root.width + "x" + root.height + " / " + root.dpiRatio

    onScreenChanged: {
        // 此时 Screen 值可能还没更新，需要点一下测试按钮打印
        console.log("screen changed", Screen.devicePixelRatio, Screen.width, Window.width)
    }

    onDpiRatioChanged: {
        console.log("dpi changed", dpiRatio)
    }

    Row {
        anchors.centerIn: parent
        spacing: 10

        Rectangle {
            width: 40
            height: 40
            border.color: "black"
        }

        Button {
            text: width + "x" + height + " / " + MyWindow.dpiRatio
            width: 100 * MyWindow.dpiRatio
            height: 40 * MyWindow.dpiRatio
            onClicked: {

            }
        }

        Button {
            text: "测试"
            onClicked: {
                console.log("screen", Screen.devicePixelRatio, "mywindow", MyWindow.dpiRatio)
            }
        }
    }

}
