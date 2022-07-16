import QtQuick 2.12
import QtQuick.Shapes 1.12

//两个圆弧组成的圆
Shape {
    id: control

    width: 200
    height: 200
    //抗锯齿
    layer{
        enabled: true
        smooth: true
        samples: 16
    }

    property alias fillGradient: content_path.fillGradient

    ShapePath {
        id: content_path
        strokeWidth: -1
        fillGradient: LinearGradient {
            //spread为Gradient通用的属性，定义起止点范围外的填充方式
            //ShapeGradient.PadSpread	该区域将填充最接近的停止色，默认值
            //ShapeGradient.RepeatSpread	在渐变区域外重复渐变
            //ShapeGradient.ReflectSpread	渐变会反射到渐变区域之外
            spread: ShapeGradient.ReflectSpread
            //x1起点和x2终点位置对应position的0到1
            x1: 0; y1: control.height/2
            x2: control.width; y2: control.height/2
            GradientStop { position: 0.0; color: "red" }
            GradientStop { position: 0.5; color: "green" }
            GradientStop { position: 1.0; color: "blue" }
        }
        startX: 0
        startY: control.height/2
        PathArc {
            x: control.width; y: control.height/2
            radiusX: control.width/2; radiusY: control.height/2
            useLargeArc: true
        }
        PathArc {
            x: 0; y: control.height/2
            radiusX: control.width/2; radiusY: control.height/2
            useLargeArc: true
        }
    }
}
