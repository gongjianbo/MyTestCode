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
        // image://provider_id/image_id
        source: "image://Provider/imageTag/0"
        // 指定 Image 的 sourceSize 会作为 requestedSize 参数，默认是 QSize(-1, -1)
        // sourceSize: Qt.size(120, 120)
    }

    property int updateNum: 0
    Connections {
        target: provider
        function onImageChanged(imageTag) {
            // Image 的 load 接口不是 public 的，所以只能切换 url 来重新加载数据
            updateNum++
            image.source = "image://Provider/" + imageTag + "/" + updateNum
        }
    }
}
