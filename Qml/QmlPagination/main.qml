import QtQuick 2.12
import QtQuick.Window 2.12

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("龚建波 1992")

    Pagination1{
        x: 10
        y: 10
        height: 40
        //width: parent.width-20
        border.color: "gray"
        radius: 4

        //pageCurrent: 10
        itemCount: 99
        onRequestPage: {
            console.log("Pagination1 request page:",page,"count:",count)
            pageCurrent=page;
        }
    }
}
