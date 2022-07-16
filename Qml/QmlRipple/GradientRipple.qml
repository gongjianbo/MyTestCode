import QtQuick 2.12
import QtGraphicalEffects 1.0

//使用渐变制作波纹效果
RadialGradient{
    id: control
    implicitWidth: 150
    implicitHeight: 150
    property double wave_offset: 0.0
    property color wave_color: "red"
    property alias running: animation.running

    NumberAnimation{
        id: animation
        target: control
        property: "wave_offset"
        from: 0
        to: 0.1
        duration: 2000
        running: control.visible
        loops: Animation.Infinite //-1
    }

    //待改进，根据参数来设置波纹数量
    gradient: Gradient{
        //很遗憾，不能用Repeater，可以试下createComponent，然后属性用binding绑定offset
        GradientStop{ position: 0; color: "transparent" }
        GradientStop{ position: control.wave_offset+0.001; color: control.wave_color }
        GradientStop{ position: control.wave_offset+0.01; color: "transparent" }
        GradientStop{ position: control.wave_offset+0.05; color: "transparent" }
        GradientStop{ position: control.wave_offset+0.1; color: control.wave_color }
        GradientStop{ position: control.wave_offset+0.11; color: "transparent" }
        GradientStop{ position: control.wave_offset+0.15; color: "transparent" }
        GradientStop{ position: control.wave_offset+0.2; color: control.wave_color }
        GradientStop{ position: control.wave_offset+0.21; color: "transparent" }
        GradientStop{ position: control.wave_offset+0.25; color: "transparent" }
        GradientStop{ position: control.wave_offset+0.3; color: control.wave_color }
        GradientStop{ position: control.wave_offset+0.31; color: "transparent" }
        GradientStop{ position: control.wave_offset+0.35; color: "transparent" }
        GradientStop{ position: control.wave_offset+0.4; color: control.wave_color }
        GradientStop{ position: control.wave_offset+0.41; color: "transparent" }
    }
}
