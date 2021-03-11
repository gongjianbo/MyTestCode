import QtQuick 2.12
import QtQuick.Controls 2.12

//波浪小球
//参照：https://blog.csdn.net/zhengtianzuo06/article/details/78687838
//参照：https://blog.csdn.net/lecepin/article/details/53536445
//参照：https://blog.csdn.net/yyy269954107/article/details/43449087
Canvas {
    id: control

    width: 250
    height: 250

    //数值范围
    property real maxValue: 100
    property real minValue: 0
    property real curValue: 0

    //显示进度
    property real curProgress: (curValue-minValue)/(maxValue-minValue)*100
    property string curText: (curProgress<0?0:curProgress).toFixed(1)+" %"

    //画布
    property int canvasWidth: width   //画布宽
    property int canvasHeight: height //画布高
    property real outWidth: 15   //外圈线宽
    property real outMargin: 10  //外圈离边界距离
    property real outPadding: 15 //外圈离波浪球距离
    property real outRadius: canvasWidth/2-outMargin //外圈半径
    property real waveReadius: outRadius-outPadding-outWidth //水球半径

    property int fontPx: 34
    property color textColor: Qt.lighter(waveColor) //文本颜色
    property color waveColor: "green" //波浪颜色

    property real waveWidth: 0.05   //波浪宽度,数越小越宽
    property real waveHeight: 5     //波浪高度,数越大越高
    property real speed: 0.1        //波浪速度,数越大速度越快
    property real offset: 0         //波浪x偏移量,用于动画

    onPaint: {
        var ctx = getContext("2d");
        ctx.clearRect(0, 0, canvasWidth, canvasHeight);
        //保存默认设置
        ctx.save();

        //【1】画外圈
        ctx.lineWidth = outWidth;
        ctx.strokeStyle = waveColor;
        ctx.beginPath();
        //以正中为圆心绘制圆
        ctx.arc(canvasWidth/2, canvasHeight/2, outRadius-outWidth/2, 0, 2*Math.PI);
        ctx.stroke();
        //深色半圆
        ctx.strokeStyle = Qt.darker(waveColor);
        ctx.beginPath();
        ctx.arc(canvasWidth/2, canvasHeight/2, outRadius-outWidth/2, offset, offset+Math.PI);
        ctx.stroke();

        //【2】截取波浪圆形区域
        //截取圆圈范围进行绘制
        ctx.lineWidth = 1;
        ctx.beginPath();
        ctx.arc(canvasWidth/2, canvasHeight/2, waveReadius, 0, 2*Math.PI);
        ctx.closePath();
        ctx.clip();

        //【3】画波浪
        //画波浪，可以导出波浪个数属性
        drawWave(ctx, Qt.darker(waveColor), 1.5, -1, true);
        drawWave(ctx, waveColor, 0, 0, false);

        //【4】画文本
        ctx.font = fontPx + 'px Arial';
        ctx.textAlign = 'center';
        ctx.fillStyle = textColor;
        ctx.fillText(curText, canvasWidth/2, canvasHeight/2+fontPx/2);

        //恢复默认设置，防止污染下一次刷新
        ctx.restore();
    }

    //画笔，颜色，x偏移，y偏移，角度值取反
    function drawWave(ctx,w_color,x_offset,y_offset,reverse)
    {
        //sin曲线
        ctx.beginPath();
        var first = true;
        var x_base = canvasWidth/2-waveReadius;
        var y_base = canvasHeight/2+waveReadius-waveReadius*2*(curProgress/100);
        //横坐标步进为5px
        for(var x_value = 0; x_value <= waveReadius*2 + 5; x_value += 5){
            var y_value = waveHeight*Math.sin((reverse?-1:1)*(x_value)*waveWidth+offset+x_offset)+y_offset;
            if(first){
                first=false;
                ctx.moveTo(x_base+x_value, y_base+y_value);
            }else{
                ctx.lineTo(x_base+x_value, y_base+y_value);
            }
        }

        //波浪
        ctx.lineTo(canvasWidth/2+waveReadius, canvasHeight/2+waveReadius);
        ctx.lineTo(canvasWidth/2-waveReadius, canvasHeight/2+waveReadius);
        ctx.closePath();
        ctx.fillStyle = w_color;
        ctx.fill();
    }

    //用定时器刷新，试了下动画的刷新率更高CPU占用率也高了
    Timer {
        running: visible
        repeat: true
        interval: 30
        onTriggered:{
            //波浪移动
            offset += speed;
            offset %= Math.PI*2;
            control.requestPaint();
        }
    }
}
