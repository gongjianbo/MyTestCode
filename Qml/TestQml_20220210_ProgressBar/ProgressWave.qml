import QtQuick 2.12
import QtQml 2.12
import QtQuick.Controls 2.12

//canvas绘制波浪进度球
//龚建波 2022-02-10
Item {
    id: control

    implicitHeight: 160
    implicitWidth: 160

    property real from: 0 //最小值
    property real to: 100 //最大值
    property real value: 0 //当前值
    property real percent: (value-from)/(to-from) //百分比[0,1]
    //
    property int canvasMargin: 3
    property int canvasWidth: width < height ? width : height
    //
    property int fontPx: 34
    property string fontFamily: "Arial"
    property color waveColor: "#0486FF"
    property int waveBorder: 6
    property int waveRadius: canvasWidth / 2 - canvasMargin - waveBorder
    //
    property real waveWidth: 0.05   //波浪宽度,数越小越宽
    property real waveHeight: 5     //波浪高度,数越大越高
    property real speed: 0.1        //波浪速度,数越大速度越快
    property real offset: 0         //波浪x偏移量,用于动画

    Canvas {
        id: canvas
        width: canvasWidth
        height: canvasWidth
        onPaint: {
            var ctx = getContext("2d");
            ctx.clearRect(0, 0, canvasWidth, canvasWidth);
            ctx.lineCap = "round";

            ctx.save();
            ctx.beginPath();
            ctx.arc(canvasWidth/2, canvasWidth/2, waveRadius+waveBorder-canvasMargin, 0, 2*Math.PI);
            ctx.lineWidth = waveBorder;
            var color_offset = offset>Math.PI?(1-(offset-Math.PI)/Math.PI):(offset/Math.PI);
            ctx.strokeStyle = Qt.lighter(waveColor, 1.5+0.3*color_offset);
            ctx.stroke();

            var progress_text = qsTr("%1 %").arg(parseInt(percent*100));
            ctx.font = fontPx + "px '" + fontFamily + "'";
            ctx.textAlign = "center";
            ctx.fillStyle = waveColor;
            //canvas字体高度和居中还有点问题
            ctx.fillText(progress_text, canvasWidth/2, canvasWidth/2+fontPx/2-5);

            ctx.save();
            ctx.beginPath();
            ctx.lineWidth = 0;
            ctx.arc(canvasWidth/2, canvasWidth/2, waveRadius, 0, 2*Math.PI);
            ctx.clip();
            drawWave(ctx, waveColor, 0, 0, false);
            ctx.clip();
            ctx.font = fontPx + "px '" + fontFamily + "'";
            ctx.textAlign = "center";
            ctx.fillStyle = "white";
            ctx.fillText(progress_text, canvasWidth/2, canvasWidth/2+fontPx/2-5);
            ctx.restore();
        }

        //画笔，颜色，x偏移，y偏移，角度值取反
        function drawWave(ctx, w_color, x_offset, y_offset, reverse)
        {
            ctx.beginPath();
            var x_base = canvasWidth/2-waveRadius;
            var y_base = canvasWidth/2+waveRadius-waveRadius*2*percent;
            //正弦波浪，横坐标步进为5px，右侧多加5是为了填补不足步进的部分
            for(var x_value = 0; x_value <= waveRadius*2 + 5; x_value += 5){
                var y_value = waveHeight*Math.sin((reverse?-1:1)*(x_value)*waveWidth+offset+x_offset)+y_offset;
                ctx.lineTo(x_base+x_value, y_base+y_value);
            }
            //左右底部围城实心
            ctx.lineTo(canvasWidth/2+waveRadius, canvasWidth/2+waveRadius);
            ctx.lineTo(canvasWidth/2-waveRadius, canvasWidth/2+waveRadius);
            ctx.closePath();
            ctx.fillStyle = w_color;
            ctx.fill();
        }
    }

    //用定时器刷新
    Timer {
        running: visible
        repeat: true
        interval: 35
        onTriggered:{
            //波浪移动
            offset += speed;
            offset %= Math.PI*2;
            canvas.requestPaint();
        }
    }
    /*NumberAnimation {
        target: control
        running: visible
        loops: -1
        property: "offset"
        from: 0
        duration: 2000
        to: Math.PI*2
    }
    onOffsetChanged: canvas.requestPaint();*/
}
