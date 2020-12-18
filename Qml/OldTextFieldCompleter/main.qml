import QtQuick 2.10
import QtQuick.Window 2.10
import QtQuick.Controls 2.3

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    /*Flow{
        id: control
        anchors.fill: parent
        spacing: 10
        Component{
            id: component
            TextFieldWithCompleter {
                dynamic: true
                completerModel: SuggestionsModel
            }
        }
        Button{
            text: "Add"
            onClicked: {
                var edit = component.createObject(control);
                parent=null;
                parent=control;
                edit.focus=true;
            }
        }
    }*/

    Flow{
        id: control
        anchors.fill: parent
        spacing: 10
        Component{
            id: component
            Rectangle{
                property alias text: text.text
                width: 120
                height: 30
                color: "gray"
                Text {
                    id: text
                    anchors.centerIn: parent
                }
            }
        }

        TextFieldWithCompleter {
            completerModel: SuggestionsModel
            onEditEnd: {
                var txt = component.createObject(control);
                parent=null;
                parent=control;
                txt.text=text;
            }
        }
    }
}
