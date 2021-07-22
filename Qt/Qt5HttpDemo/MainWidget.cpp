#include "MainWidget.h"
#include "ui_MainWidget.h"

#include <QUrlQuery>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

#include <QDebug>

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{
    ui->setupUi(this);

    manager=new QNetworkAccessManager(this);

    //查看支持的协议
    qDebug()<<manager->supportedSchemes();
    //安装了openssl，会有https
    //("ftp", "file", "qrc", "http", "data")

    //QNetworkAccessManager是异步的
    connect(manager, &QNetworkAccessManager::finished, this, &MainWidget::receiveReply);
    //测试get
    connect(ui->btnGet,&QPushButton::clicked,this,&MainWidget::getTest);
    //测试post
    connect(ui->btnPost,&QPushButton::clicked,this,&MainWidget::postTest);
    //测试form
    connect(ui->btnForm,&QPushButton::clicked,this,&MainWidget::formTest);


    //定时器拿来验证是否阻塞界面
    connect(&timer,&QTimer::timeout,this,[this](){
        ui->editTime->setTime(QTime::currentTime());
    });
    timer.start(1000); //定时1s
}

MainWidget::~MainWidget()
{
    delete ui;
}

void MainWidget::getTest()
{
    //构建请求对象
    QNetworkRequest request;
    request.setUrl(QUrl(ui->editGetUrl->text()));

    //设置请求头
    //已知头类型参见文档QNetworkRequest::KnownHeaders
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
    //自定义类型
    request.setRawHeader("self-header","gongjianbo");

    //发送请求
    manager->get(request);

    //同步处理，可以开启一个局部的事件循环，等待响应，不会阻塞线程
    //QNetworkReply *reply=manager->get(request);
    //QEventLoop eventLoop;
    //connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
    //eventLoop.exec();
    //receiveReply(reply);

    //超时处理，可以使用定时器调用abort、close来终止当前的请求
    //QNetworkReply *reply=manager->get(request);
    //if(reply->isRunning()){
    //    QTimer *timer=new QTimer(reply);//对象树关联释放，也可以在finish进行释放
    //    timer->setSingleShot(true);
    //    //超时就结束
    //    connect(timer,&QTimer::timeout,reply,&QNetworkReply::abort);
    //    //结束就关定时器
    //    connect(reply,&QNetworkReply::finished,timer,&QTimer::stop);
    //    //定时
    //    timer->start(5000);
    //}
}

void MainWidget::postTest()
{
    //构建请求对象
    QNetworkRequest request;
    request.setUrl(QUrl(ui->editPostUrl->text()));

    //设置请求头
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");

    //发送请求
    manager->post(request,ui->editPostData->toPlainText().toUtf8());
}

void MainWidget::formTest()
{
    QNetworkAccessManager manager2;
    //构建一个multiPart用于提交表单
    //注意，multiPart请在请求完成后再删除
    QHttpMultiPart *multiPart=new QHttpMultiPart(QHttpMultiPart::FormDataType);

    //文本内容
    QHttpPart namePart;
    //Content-Type对照表详情百度http://tool.oschina.net/commons/
    namePart.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("text/plain"));
    namePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"thename\";"));
    namePart.setBody("gongjianbo");
    QHttpPart agePart;
    agePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"theage\";"));
    agePart.setBody("27");
    multiPart->append(namePart);
    multiPart->append(agePart);

    //文件内容
    QHttpPart filePart;
    filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain")); //我用application传各种类型文件
    //示例里没有filename，导致提交不成功
    filePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"thefile\";filename=\"file.txt\";"));
    QFile *textFile = new QFile(":/file.txt");
    textFile->setParent(multiPart); //在删除reply时一并释放
    if(textFile->open(QIODevice::ReadOnly)){
        //要读取小块数据，请使用setBody(); 对于像图像这样的较大数据块，请使用setBodyDevice()。
        filePart.setBodyDevice(textFile);
        multiPart->append(filePart);
    }

    //图片内容
    QHttpPart imagePart;
    imagePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/png"));
    //示例里没有filename，导致提交不成功
    imagePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"theimage\";filename=\"image.png\";"));
    QFile *imageFile = new QFile(":/image.png");
    imageFile->setParent(multiPart); //在删除reply时一并释放
    if(imageFile->open(QIODevice::ReadOnly)){
        imagePart.setBodyDevice(imageFile);
        multiPart->append(imagePart);
    }

    //在我们公司里使用的时候，没有Boundary也会导致提交不成功
    multiPart->setBoundary("qtdata");
    QNetworkRequest request(QUrl("http://httpbin.org/post"));
    request.setRawHeader("Content-Type","multipart/form-data;boundary=qtdata");

    //提交表单
    QNetworkReply *reply=manager2.post(request,multiPart);
    multiPart->setParent(reply); //在删除reply时一并释放

    QEventLoop eventLoop;
    connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
    eventLoop.exec();

    qDebug()<<"reply:"<<reply->readAll();
    reply->deleteLater();
}

void MainWidget::receiveReply(QNetworkReply *reply)
{
    //请求方式get、post等，对应QNetworkAccessManager的Operation枚举
    //对于restful接口需要判断此枚举
    qDebug()<<"operation:"<<reply->operation();
    //路径
    qDebug()<<"url:"<<reply->url();
    //状态码
    const int status_code=reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    qDebug()<<"status code:"<<status_code;
    //主动调用QNetworkReply的close或者abort也会触发finished，这时状态码为0
    //访问失败状态码也会为0
    if(status_code==0 && reply->errorString()=="Operation canceled"){
        qDebug()<<"timeout";
    }

    if(reply->error()!=QNetworkReply::NoError){
        //处理中的错误信息
        qDebug()<<"reply error:"<<reply->errorString();
    }else{
        //重定向
        /*if (status_code == 301 || status_code == 302){
              // Or the target URL if it was a redirect:
              QVariant redirectionTargetUrl =reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
              //qDebug() << "redirection Url is " << redirectionTargetUrl.toString();
              QUrl url(redirectionTargetUrl.toString());
              manager->get(QNetworkRequest(url));
        }*/

        //获取响应信息并展示到ui，这里没有解析
        const QByteArray reply_data=reply->readAll();
        //qDebug()<<"read all:"<<reply_data;
        ui->editRecv->append("[Reply] "+QTime::currentTime().toString("hh:mm:ss"));
        //根据自己的编码选用对应的QString方法
        ui->editRecv->append(QString::fromUtf8(reply_data));

        //解析json
        /*QJsonParseError json_error;
        QJsonDocument document=QJsonDocument::fromJson(reply_data, &json_error);
        if (json_error.error==QJsonParseError::NoError) {
            if(document.isObject()){
                const QJsonObject obj=document.object();
                qDebug()<<obj;
                if(obj.contains("args")){
                    QJsonValue value=obj.value("args");
                    qDebug()<<value;
                    //QJsonValue(object, QJsonObject({"ie":"utf-8"}))
                }
            }
        }else{
            qDebug()<<"json error:"<<json_error.errorString();
        }*/
    }
    reply->deleteLater();
}
