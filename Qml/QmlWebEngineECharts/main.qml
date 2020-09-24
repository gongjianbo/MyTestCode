import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtWebEngine 1.2

Window {
    id: root
    width: 640
    height: 480
    visible: true
    title: qsTr("WebEngine + ECharts (By: GongJianBo1992)")
    property var now: new Date(1997, 9, 3)
    property var oneDay: 24 * 3600 * 1000

    WebEngineView{
        id: view
        anchors.fill: parent
        //@disable-check M7
        url:"file:///./index.html"
    }

    Row{
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 10
        spacing: 10

        Button{
            text: "单个数据"
            onClicked: {
                now = new Date(+now + oneDay);
                view.runJavaScript("window.appendData("+
                                   JSON.stringify({
                                                      "date":now.valueOf(),
                                                      "value":Number(Math.random() * 1000 + 500)
                                                  })+")");
            }
        }

        Button{
            text: "批量数据"
            onClicked: {
                let datalist=[];
                for(let i=0;i<100;i++)
                {
                    now = new Date(+now + oneDay);
                    datalist.push({"date":now.valueOf(),"value":Number(Math.random() * 1000 + 500)});
                }
                view.runJavaScript("window.setData("+
                                   JSON.stringify(datalist)+")");
            }
        }
    }
}
