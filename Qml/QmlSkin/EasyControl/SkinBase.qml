import QtQuick 2.12
import QtQml 2.12
import "./SkinElement"

//Skin基类
QtObject {
    id: control

    //基本组件样式，默认绑定基本样式
    property RectangleStyle rectangle: RectangleStyle{}
    property ButtonStyle button: ButtonStyle{}

    //特殊样式，根据需求扩展和绑定
    property ButtonStyle redButton: ButtonStyle{
        bgNormalColor: "red"
    }
}
