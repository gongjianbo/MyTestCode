import QtQuick 2.12

//Skin基类，为了子类的可替换性，属性尽量在base里定义
QtObject {
    id: control

    property QtObject rectangle : QtObject{
        property color bgColor: "lightgray"
    }

    property QtObject button : QtObject{
        property color textColor: "white"
        property color bgColor: "gray"
    }
}
