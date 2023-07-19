import QtQuick 2.15

Canvas {
    id: control

    property real mLineCount: 3600
    property real mRotation: 0

    function rotate(rotation)
    {
        mRotation = rotation
        control.requestPaint()
    }

    onPaint: {
        console.log("canvas begin", Qt.formatDateTime(new Date, "hh:mm:ss.zzz"))
        var r = Math.min(width, height) / 2 - 2
        var ctx = getContext("2d")
        ctx.clearRect(0, 0, width, height)

        ctx.save()
        ctx.translate(width / 2, height / 2)
        ctx.lineWidth = 1 + 1E-7
        ctx.rotate((mRotation) * Math.PI / 180.0)
        for (var i = 0; i < mLineCount; i++)
        {
            ctx.beginPath()
            ctx.strokeStyle = Qt.rgba(i / mLineCount, 0, 0, 1)
            ctx.moveTo(0, 0)
            ctx.lineTo(r, 0)
            ctx.stroke()
            ctx.rotate((360.0 / mLineCount) * Math.PI / 180.0)
        }
        ctx.restore()
        console.log("canvas end", Qt.formatDateTime(new Date, "hh:mm:ss.zzz"))
    }
}
