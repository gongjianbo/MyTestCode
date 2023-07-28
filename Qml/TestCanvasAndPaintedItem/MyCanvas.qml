import QtQuick 2.15

Canvas {
    id: control

    property real mLineCount: 360 * 4
    property real mRotation: 0

    function rotate(rotation)
    {
        mRotation = rotation
        control.requestPaint()
    }

    function resize(count)
    {
        mLineCount = count
        control.requestPaint()
    }

    onPaint: {
        console.log("canvas begin", Qt.formatDateTime(new Date, "hh:mm:ss.zzz"))
        var r = Math.min(width, height) / 2 - 2
        var ctx = getContext("2d")
        ctx.clearRect(0, 0, width, height)

        ctx.save()
        ctx.translate(width / 2, height / 2)
        ctx.strokeStyle = Qt.rgba(0.5, 0, 0, 1)
        ctx.lineWidth = 0.5 // + 1E-7
        // QML Canvas 默认 flat, QPainter 默认 square
        // 如果不使用 flat，多次旋转后可能会不均匀
        ctx.lineCap = "square"
        for (var i = 0; i < mLineCount; i++)
        {
            // 如果不使用 save/restore, 多次旋转后可能会不均匀
            ctx.save()
            ctx.rotate((mRotation + 360.0 / mLineCount * i) * Math.PI / 180.0)
            ctx.beginPath()
            ctx.moveTo(0, 0)
            ctx.lineTo(r, 0)
            ctx.stroke()
            ctx.restore()
        }
        ctx.restore()
        console.log("canvas end", Qt.formatDateTime(new Date, "hh:mm:ss.zzz"))
    }
}
