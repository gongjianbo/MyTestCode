import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Shapes 1.12

Window {
    width: 660
    height: 500
    visible: true
    title: qsTr("Hello ShapeGradient")

    Grid {
        anchors.centerIn: parent
        rows: 2
        columns: 3
        rowSpacing: 10
        columnSpacing: 10
        Circle {
            id: c1
            //线性渐变
            fillGradient: LinearGradient {
                //spread为Gradient通用的属性，定义起止点范围外的填充方式
                //ShapeGradient.PadSpread	该区域将填充最接近的停止色，默认值
                //ShapeGradient.RepeatSpread	在渐变区域外重复渐变
                //ShapeGradient.ReflectSpread	渐变会反射到渐变区域之外
                spread: ShapeGradient.PadSpread
                //x1起点和x2终点位置对应position的0到1
                x1: 0; y1: c1.height/2
                x2: c1.width; y2: c1.height/2
                GradientStop { position: 0.0; color: "red" }
                GradientStop { position: 0.5; color: "green" }
                GradientStop { position: 1.0; color: "blue" }
            }
        }
        Circle {
            id: c2
            //线性渐变
            fillGradient: LinearGradient {
                spread: ShapeGradient.RepeatSpread
                x1: c1.width/3*1; y1: c1.height/3*1
                x2: c1.width/3*2; y2: c1.height/3*2
                GradientStop { position: 0.0; color: "red" }
                GradientStop { position: 0.5; color: "green" }
                GradientStop { position: 1.0; color: "blue" }
            }
        }
        Circle {
            id: c3
            //径向渐变
            fillGradient: RadialGradient {
                //中心点
                centerX: 100; centerY: 100; centerRadius: 100
                //焦点
                focalX: 100; focalY: 100; focalRadius: 0
                GradientStop { position: 0.0; color: "red" }
                GradientStop { position: 0.5; color: "green" }
                GradientStop { position: 1.0; color: "blue" }
            }
        }
        Circle {
            id: c4
            //径向渐变
            fillGradient: RadialGradient {
                spread: ShapeGradient.RepeatSpread
                //中心点
                centerX: 50; centerY: 100; centerRadius: 100
                //焦点
                focalX: 100; focalY: 100; focalRadius: 30
                GradientStop { position: 0.0; color: "red" }
                GradientStop { position: 0.5; color: "green" }
                GradientStop { position: 1.0; color: "blue" }
            }
        }
        Circle {
            id: c5
            //锥形渐变
            fillGradient: ConicalGradient {
                //中心点
                centerX: 100; centerY: 100;
                GradientStop { position: 0.0; color: "red" }
                GradientStop { position: 0.5; color: "green" }
                GradientStop { position: 1.0; color: "blue" }
            }
        }
        Circle {
            id: c6
            //锥形渐变
            fillGradient: ConicalGradient {
                //中心点
                centerX: 80; centerY: 80; angle: 45
                GradientStop { position: 0.0; color: "red" }
                GradientStop { position: 0.5; color: "green" }
                GradientStop { position: 1.0; color: "blue" }
            }
        }
    }
}
