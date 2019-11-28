import QtQuick 2.12
import QtGraphicalEffects 1.0

Item {
    id: control
    implicitWidth: 100
    implicitHeight: 100
    visible: false
    property double wave_offset: 0.0
    property color wave_color: "red"

    RadialGradient{
        id: gradient
        anchors.fill: parent
        //horizontalRadius: 0
        //verticalRadius: 0
        //horizontalOffset: 0
        //verticalOffset: 0

        gradient: Gradient{
            GradientStop{
                position: (control.wave_offset>0.1)?(control.wave_offset-0.1):0
                color: "transparent"
            }
            GradientStop{ position: control.wave_offset+0.09; color: control.wave_color }
            GradientStop{ position: control.wave_offset+0.11; color: "transparent" }
        }
    }
    PropertyAnimation{
        id: animation
        target: control
        property: "wave_offset"
        from:0
        to:0.4
        duration: 400
        running: false
        onStarted: control.visible=true
        onFinished: control.visible=false
    }

    function click(ptx,pty){
        animation.stop()
        control.x=ptx-control.width/2
        control.y=pty-control.height/2
        animation.start()
    }
}
