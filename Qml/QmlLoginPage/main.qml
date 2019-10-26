import QtQuick 2.12
import QtQuick.Window 2.12

import "LoginPage"
import "MainPage"


Window {
    visible: true
    width: 900
    height: 600
    title: "龚建波 QQ：654344883"

    LoginPage{
        id: login_page
        anchors.fill: parent
    }
    MainPage{
        id: main_page
        visible: !login_page.visible
    }
}
