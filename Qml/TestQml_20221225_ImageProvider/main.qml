import QtQuick 2.15
import QtQuick.Window 2.15

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Image Provider")

    Image {
        id: image
        anchors.centerIn: parent
        cache: false
        //image://provider id/image id
        source: "image://Provider/imageTag/0"
        //指定Image的sourceSize会作为requestedSize参数
        //sourceSize: Qt.size(120, 120)
    }

    property int id: 0
    Connections {
        target: provider
        function onImageChanged(imageTag) {
            //Image的load接口不是public的，所以只能切换url来重新加载数据
            //可能Image的设计就是用于加载静态图像的
            id++
            image.source = "image://Provider/" + imageTag + "/" + id
        }
    }
}
