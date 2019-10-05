import QtQuick 2.12

Column {
    spacing: 10
    Row{
        spacing: 10

        Rectangle{
            id: rect_red
            width: 100; height: 100; color: "red"
            //属性动画，加载完就执行
            RotationAnimation on rotation {
                from: 0  //起始角度，默认为当前角度
                to: 360  //终止角度
                duration: 2000  //动画时间
                //running: true //默认加载完就run
                //loops: 1      //默认循环1次
            }
        }

        Rectangle{
            id: rect_green
            width: 100; height: 100; color: "green"
            //属性行为，属性改变时执行
            Behavior on rotation {
                RotationAnimation { duration: 1000 }
            }
            MouseArea{
                anchors.fill: parent
                onClicked: rect_green.rotation+=180; //触发属性行为
            }
        }

        Rectangle{
            id: rect_blue
            width: 100; height: 100; color: "blue"
            //独立动画，手动执行
            //每个动画都有一个start()，stop()，resume()，restart()函数
            RotationAnimation{
                id: rotation_blue
                target: rect_blue //关联动画对象
                from: 0
                to: 360
                duration: 2000
            }
            MouseArea{
                anchors.fill: parent
                onClicked: rotation_blue.start(); //手动执行
            }
        }
    }

    Row{
        spacing: 10
        Rectangle{
            id: rect_sequential
            width: 100; height: 100; color: "orange"
            SequentialAnimation{
                id: sequential
                running: true //加载完就执行
                NumberAnimation{
                    target: rect_sequential
                    properties: "width"
                    to: 200
                    duration: 2000
                    easing.type: Easing.OutBack //改变缓和曲线类型
                }
                NumberAnimation{
                    target: rect_sequential
                    properties: "height"
                    to: 200
                    duration: 2000
                    easing.type: Easing.OutBack
                }
                //嵌套
                ParallelAnimation{
                    NumberAnimation{
                        target: rect_sequential
                        properties: "width"
                        to: 100
                        duration: 2000
                        easing.type: Easing.InOutBack
                    }
                    NumberAnimation{
                        target: rect_sequential
                        properties: "height"
                        to: 100
                        duration: 2000
                        easing.type: Easing.InOutBack
                    }
                }
            }
        }
        Rectangle{
            id: rect_parallel
            width: 100; height: 100; color: "purple"
            ParallelAnimation{
                id: parallel
                running: true //加载完就执行
                NumberAnimation{
                    target: rect_parallel
                    properties: "width"
                    to: 200
                    duration: 2000
                }
                NumberAnimation{
                    target: rect_parallel
                    properties: "height"
                    to: 200
                    duration: 2000
                }
            }
        }
    }
}
