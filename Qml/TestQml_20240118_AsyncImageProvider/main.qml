import QtQuick 2.15
import QtQuick.Window 2.15

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Async Image Provider")

    Image {
        id: image
        anchors.centerIn: parent
        asynchronous: true
        cache: false
        // image://provider_id/image_id
        source: "image://Provider/imageTag/0"
        // 指定 Image 的 sourceSize 会作为 requestedSize 参数，默认是 QSize(-1, -1)
        // sourceSize: Qt.size(120, 120)
    }
    // 加一个 Shader 是为了防止刷新时闪烁，Qt 6.5 之后也可以用 Image 的 layer.live
    // https://bugreports.qt.io/browse/QTBUG-66713
    ShaderEffectSource {
        id: texture
        anchors.fill: image
        sourceItem: image
        live: image.status === Image.Ready
        hideSource: true
    }

    property int updateNum: 0
    Connections {
        target: provider
        function onImageChanged(imageTag) {
            // Image 的 load 接口不是 public 的，所以只能切换 url 来重新加载数据
            // 异步加载时间更长，刷新会闪烁
            updateNum++
            image.source = "image://Provider/" + imageTag + "/" + updateNum
        }
    }
}
