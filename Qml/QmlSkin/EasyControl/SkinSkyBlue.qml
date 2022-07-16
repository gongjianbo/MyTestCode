pragma Singleton

import QtQuick 2.12
import QtQml 2.12

SkinBase {
    id: control

    //公用的颜色可以多个组件样式绑定到一个
    property color commonNormalColor: "skyblue"

    rectangle.bgColor: "mintcream"

    button{
        textColor: "white"
        bgNormalColor: commonNormalColor
    }
}
