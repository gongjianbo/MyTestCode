import QtQuick 2.12

Text{
    id: txt
    font.pixelSize: 30

    function request() {
        let xhr = new XMLHttpRequest();
        xhr.onreadystatechange = function() {
            //XMLHttpRequest.DONE为状态枚举值4
            if(xhr.readyState === 4) {
                print('request DONE',xhr.status);
                if(xhr.status === 200){
                    txt.text = xhr.responseText.toString();
                }
            }
        }
        //【get】
        //xhr.open("GET", "http://127.0.0.1:54321/text?arg=Haha");
        //xhr.send();
        //【post】
        xhr.open("POST", "http://127.0.0.1:54321/text");
        //如果要使用post提交数据,添加此行
        xhr.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
        xhr.send("arg=Xixi");
    }

    Component.onCompleted: request()
}
