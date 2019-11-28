import QtQuick 2.12
import QtQuick.Shapes 1.12

Item {
    id: control
    implicitWidth: 150
    implicitHeight: 150
    property double wave_offset: 0.0
    property color wave_color: "red"
    property alias running: animation.running

    Shape {
        anchors.fill: parent
        //这些设置感觉没效果
        //多重采样抗锯齿
        //layer.enabled: true
        //layer.smooth: true
        //layer.samples: 8
        //平滑处理
        //smooth: true
        //反走样抗锯齿
        //antialiasing: true

        ShapePath{
            startX:0
            startY: 0
            strokeColor: "transparent"
            //需要圈起来他才会填充渐变色
            PathLine {x: control.width;y: 0}
            PathLine {x: control.width;y: control.height}
            PathLine {x: 0;y: control.height}
            PathLine {x: 0;y: 0}

            fillGradient: RadialGradient {
                //ShapeGradient.PadSpread	该区域将填充最接近的停止色。
                //ShapeGradient.RepeatSpread	在渐变区域外重复渐变。
                //ShapeGradient.ReflectSpread	渐变会反射到渐变区域之外。
                //spread: ShapeGradient.PadSpread
                centerRadius: control.width/2
                //对于简单的径向渐变，应该将focusRadius设置为0（默认值）
                centerX: control.width/2 //中心
                centerY: control.height/2
                focalX: centerX  //焦点
                focalY: centerY
                //待改进，根据参数来设置波纹数量, 很遗憾，不能用Repeater
                GradientStop{ position: 0; color: "transparent" }
                GradientStop{ position: control.wave_offset+0.001; color: control.wave_color }
                GradientStop{ position: control.wave_offset+0.01; color: "transparent" }
                GradientStop{ position: control.wave_offset+0.15; color: "transparent" }
                GradientStop{ position: control.wave_offset+0.2; color: control.wave_color }
                GradientStop{ position: control.wave_offset+0.21; color: "transparent" }
                GradientStop{ position: control.wave_offset+0.35; color: "transparent" }
                GradientStop{ position: control.wave_offset+0.4; color: control.wave_color }
                GradientStop{ position: control.wave_offset+0.41; color: "transparent" }
                GradientStop{ position: control.wave_offset+0.55; color: "transparent" }
                GradientStop{ position: control.wave_offset+0.6; color: control.wave_color }
                GradientStop{ position: control.wave_offset+0.61; color: "transparent" }
                GradientStop{ position: control.wave_offset+0.75; color: "transparent" }
                GradientStop{ position: control.wave_offset+0.8; color: control.wave_color }
                GradientStop{ position: control.wave_offset+0.81; color: "transparent" }
            }
        }
    }

    PropertyAnimation{
        id: animation
        target: control
        property: "wave_offset"
        from:0
        to:0.2
        duration: 1000
        running: control.visible
        loops: Animation.Infinite
    }
}
