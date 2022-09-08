import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import Test 1.0

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello GongJianBo")

    CppObject {
        id: cpp_obj
    }

    Column {
        anchors.centerIn: parent
        spacing: 12
        Button {
            text: "invoke"
            onClicked: {
                cpp_obj.doInvoke(1992, qml_obj)
            }
        }
        Button {
            text: "qml callback"
            onClicked: {
                doSomething(1992, cpp_obj.cppFunc)
            }
        }
        Button {
            text: "cpp callback"
            onClicked: {
                //传递js函数作为回调函数
                cpp_obj.doSomething(1992, function(val, flag){
                    console.log("qml function run", val, flag);
                    return "qml return value";
                })
            }
        }
    }

    Item {
        id: qml_obj
        objectName: "qml_obj"
        function qmlFunc(val)
        {
            console.log("qml function run", val);
            return "qml return value";
        }
    }

    function doSomething(val, func)
    {
        console.log("qml doSomething", val);
        var ret = func(val);
        console.log("qml callback ret:", ret);
    }
}
