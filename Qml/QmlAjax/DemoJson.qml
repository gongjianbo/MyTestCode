import QtQuick 2.12

ListView {
    id: view

    spacing: 10
    //model:['red','green','blue']
    delegate: Rectangle{
        width: ListView.view.width
        height: 30
        color: modelData
    }

    //请求网络数据
    function request() {
        let xhr = new XMLHttpRequest();
        xhr.onreadystatechange = function() {
            if(xhr.readyState === 4) {
                print('request DONE',xhr.status);
                if(xhr.status === 200){
                    print(xhr.responseText.toString())
                    view.model = JSON.parse(xhr.responseText.toString());
                }
            }
        }
        xhr.open("POST", "http://127.0.0.1:54321/json");
        xhr.setRequestHeader("Content-type", "application/json");
        //xhr.send("['red','green','blue','orange','cyan']");
        xhr.send(JSON.stringify(['red','green','blue','orange','cyan']));
    }

    //访问本地文件
    function requestLocal() {
        let xhr = new XMLHttpRequest();
        xhr.onreadystatechange = function() {
            if(xhr.readyState === 4) {
                print('request DONE',xhr.status);
                if(xhr.status === 200){
                    print(xhr.responseText.toString())
                    view.model = JSON.parse(xhr.responseText.toString());
                }
            }
        }
        //json文件的文本内容，字符串双引号：["red","green","blue"]
        xhr.open("GET", "qrc:/localfile.json");
        xhr.send();
    }

    Component.onCompleted: request()
    //Component.onCompleted: requestLocal()
}
