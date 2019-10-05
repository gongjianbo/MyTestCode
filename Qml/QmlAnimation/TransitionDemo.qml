import QtQuick 2.12

Column{
    id: light_column
    spacing: 10

    //绿灯
    Rectangle{
        id: green_light
        width: 100; height: 100; radius:50
    }
    //红灯
    Rectangle{
        id: red_light
        width: 100; height: 100; radius:50
    }
    Text {
        id: number
        color: "black"
        font.pixelSize: 20
        text: Number(count).toString()
        readonly property int count_init: 5 //默认5s切换一次状态
        property int count: count_init
        Timer{
            interval: 1000 //1 s跳一次
            repeat: true
            running: true
            onTriggered: {
                if(number.count>1){
                    number.count-=1;
                }else{
                    number.count=number.count_init;
                    light_column.state=(light_column.state == "stop"? "go" : "stop");
                }
            }
        }
    }

    state: "stop"
    states: [
        State {
            name: "go"
            PropertyChanges { target: red_light; color: "black" }
            PropertyChanges { target: green_light; color: "green" }
        },
        State {
            name: "stop"
            PropertyChanges { target: red_light; color: "red" }
            PropertyChanges { target: green_light; color: "black" }
        }
    ]

    transitions: [
        Transition {
            from: "stop"
            to: "go"
            ColorAnimation { targets: [green_light,red_light]; duration: 1000 }
        },
        Transition {
            from: "go"
            to: "stop"
            ColorAnimation { targets: [green_light,red_light]; duration: 1000 }
        }
    ]
}
