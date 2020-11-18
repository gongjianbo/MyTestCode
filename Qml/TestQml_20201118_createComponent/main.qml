import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQml 2.12

//js动态创建qml对象
//参考文档：https://doc.qt.io/qt-5/qml-qtqml-component.html
//参考文档：https://doc.qt.io/qt-5/qml-qtqml-qt.html
Window {
    id: root
    visible: true
    width: 640
    height: 480
    title: qsTr("Dynamic QML Object Creation from JavaScript")

    Row{
        x:10
        spacing: 10
        Button{
            text: "url"
            onClicked: {
                createFromUrl();
            }
        }
        Button{
            text: "Component"
            onClicked: {
                createFromComponent();
            }
        }
        Button{
            text: "qml code"
            onClicked: {
                createFromQml();
            }
        }
        Button{
            text: "incubate"
            onClicked: {
                createFromIncubate();
            }
        }
    }

    //通过url加载qml组件后再创建item
    function createFromUrl(){
        //object Qt::createComponent(url, mode, parent)
        //如果未传递mode，第二个参数可以是parent
        //mode有两个枚举值：
        //Component.PreferSynchronous 最好立即加载/编译组件。这并不总是可能的。例如，远程URL将始终异步加载。
        //Component.Asynchronous 在后台线程中加载/编译组件
        var component = Qt.createComponent("TestItem.qml");

        //status有四个枚举值：
        //Component.Null 该Component没有数据。调用loadUrl或setData添加QML内容。
        //Component.Ready 已准备就绪，可以调用create
        //Component.Loading 正在加载
        //Component.Error 发生了错误
        if (component.status === Component.Ready) {
            //object Component::createObject(QtObject parent, object properties)
            //可见的item需要绑定图形对象，不可兼得也可以绑定非图形对象
            var item = component.createObject(flow,{"width":100,"height":100});
            //... ...
        }else if(component.status === Component.Loading){
            //如果没有立即加载好就绑定到statusChanged，异步加载
            //判断下Loading是因为Error状态绑定onStatusChanged会出错
            //也可以component.statusChanged.connect(callback);
            component.onStatusChanged = function(status) {
                if (status === Component.Ready) {
                    var item = component.createObject(flow,{"width":100,"height":100});
                    //... ...
                }
            }
        }
    }

    Component{
        id: test_comp
        Rectangle{
            color: "green"
        }
    }
    //通过Component直接创建item
    function createFromComponent(){
        //object Component::createObject(QtObject parent, object properties)
        //可见的item需要绑定图形对象，不可兼得也可以绑定非图形对象
        var item = test_comp.createObject(flow,{"width":100,"height":100});
        //... ...
    }

    //通过qml代码创建item
    function createFromQml(){
        //object Qt::createQmlObject(string qml, object parent, string filepath)
        //如果指定了filepath，它将用于创建对象的错误报告
        var item = Qt.createQmlObject('import QtQuick 2.12; Rectangle {color: "blue"; width: 100; height: 100}',flow);
        //... ...
    }

    //通过孵化器创建新实例
    function createFromIncubate(){
        //object Component::incubateObject(Item parent, object properties, enumeration mode)
        //为此组件的实例创建一个incubator孵化器，孵化器允许异步实例化新组件实例
        //mode有两个枚举
        //Qt.Synchronous 同步创建。也可能异步创建对象，比如组件本身就是异步创建的
        //Qt.Asynchronous (默认)异步创建
        //返回Incubator孵化器，可调用forceCompletion() 完成同步
        var incubator=test_comp.incubateObject(flow,{"color":"yellow","width":100,"height":100});
        if (incubator.status !== Component.Ready) {
            incubator.onStatusChanged = function(status) {
                if (status === Component.Ready) {
                    //... ...
                }
            }
        } else {
            //... ...
        }
    }

    Rectangle{
        anchors.fill: parent
        anchors.topMargin: 50
        border.color: "gray"
        Flow{
            id: flow
            anchors.fill: parent
            anchors.margins: 10
            spacing: 10
        }
    }
}
