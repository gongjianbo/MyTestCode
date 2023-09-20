#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QUrlQuery>
#include <QUrl>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // 数值也用字符串简化转换操作
    ui->textRequest->setPlainText(R"({
"user":"gongjianbo",
"create-time":"1992"
})");

    // 测试不同的请求方式
    connect(ui->btnGet, &QPushButton::clicked, this, &MainWindow::doGet);
    connect(ui->btnPost, &QPushButton::clicked, this, &MainWindow::doPost);
    connect(ui->btnPut, &QPushButton::clicked, this, &MainWindow::doPut);
    connect(ui->btnPatch, &QPushButton::clicked, this, &MainWindow::doPatch);
    connect(ui->btnDelete, &QPushButton::clicked, this, &MainWindow::doDelete);
    // 测试文件上传下载
    connect(ui->btnUp, &QPushButton::clicked, this, &MainWindow::doUp);
    connect(ui->btnDown, &QPushButton::clicked, this, &MainWindow::doDown);
}

MainWindow::~MainWindow()
{
    delete ui;
}

httplib::Client MainWindow::getClient() const
{
    if (ui->radioUrl->isChecked()) {
        return httplib::Client{ui->editUrl->text().toStdString()};
    } else {
        return httplib::Client{ui->editIp->text().toStdString(), ui->spinPort->value()};
    }
}

std::string MainWindow::getPath(const std::string &method) const
{
    if (ui->radioUrl->isChecked()) {
        return std::string("/") + method;
    } else {
        return std::string("/") + ui->editPath->text().toStdString();
    }
}

std::string MainWindow::jsonToUri(const std::string &json) const
{
    QJsonObject obj = QJsonDocument::fromJson(QByteArray::fromStdString(json)).object();
    QUrlQuery query;
    for (auto it = obj.begin(); it != obj.end(); it++)
    {
        query.addQueryItem(it.key(), it.value().toString());
    }
    return query.toString().toStdString();
}

void MainWindow::printResult(const httplib::Result &res) const
{
    if (!res) {
        return;
    }
    std::cerr << __FUNCTION__ << std::endl;
    std::cerr << "Status Code:" << res->status << std::endl;
    std::cerr << "Header:\n";
    for (auto it = res->headers.begin(); it != res->headers.end(); it++)
    {
        std::cerr << "\t" << it->first << ": " << it->second << std::endl;
    }
    std::cerr << "Body:\n" << res->body << std::endl;
    ui->textResult->setPlainText(QString::fromStdString(res->body));
}

void MainWindow::doGet()
{
    httplib::Client cli = getClient();
    // 请求体中的 args 属性是请求路径中的查询参数
    std::string path = getPath("get") + "?" + jsonToUri(ui->textRequest->toPlainText().toStdString());

    // keep alive 默认 false
    // cli.set_keep_alive(true);
    httplib::Result res = cli.Get(path);
    std::cerr << __FUNCTION__ << "\tpath:" << path << "\tres:" << (!!res) << std::endl;
    printResult(res);
}

void MainWindow::doPost()
{
    httplib::Client cli = getClient();
    std::string path = getPath("post");
    // data 属性
    // 当 content-type 不是 multipart/form-data 和 application/x-www-form-urlencoded 时，data 才会有值
    // json 属性
    // 当 content-type 是 application/json 时，请求体的数据会被解析为 json 属性
    std::string json = ui->textRequest->toPlainText().toStdString();

    // httplib::Headers headers;
    // headers.emplace("Content-Type", "appliation/json");
    // httplib::Result res = cli.Post("/post", headers);
    httplib::Result res = cli.Post(path, json, "application/json");
    std::cerr << __FUNCTION__ << "\tpath:" << path << "\tres:" << (!!res) << std::endl;
    printResult(res);
}

void MainWindow::doPut()
{
    httplib::Client cli = getClient();
    // args 和 form 都会在 httplib::Server 的 httplib::Request.params 字段
    std::string path = getPath("put") + "?" + jsonToUri(ui->textRequest->toPlainText().toStdString());
    // form 属性
    // 当 content-type 是 application/x-www-form-urlencoded 或 multipart/form-data 时，
    // 请求体的数据会被解析为 form 属性
    httplib::Params params;
    params.insert(std::pair<std::string, std::string>{"form", "hello wolrd"});

    httplib::Result res = cli.Put(path, params);
    std::cerr << __FUNCTION__ << "\tpath:" << path << "\tres:" << (!!res) << std::endl;
    printResult(res);
}

void MainWindow::doPatch()
{
    httplib::Client cli = getClient();
    std::string path = getPath("patch");

    httplib::Result res = cli.Patch(path);
    std::cerr << __FUNCTION__ << "\tpath:" << path << "\tres:" << (!!res) << std::endl;
    printResult(res);
}

void MainWindow::doDelete()
{
    httplib::Client cli = getClient();
    std::string path = getPath("delete");

    httplib::Result res = cli.Delete(path);
    std::cerr << __FUNCTION__ << "\tpath:" << path << "\tres:" << (!!res) << std::endl;
    printResult(res);
}

void MainWindow::doUp()
{
    httplib::Client cli = getClient();

#if 0
    // 小文件一次性发送
    httplib::MultipartFormData form;
    form.name = "myfile";
    form.content = "hello world";
    form.filename = "hello.txt";
    form.content_type = "multipart/form-data";
    httplib::MultipartFormDataItems form_items;
    form_items.push_back(form);
    httplib::Result res = cli.Post("/upload", form_items);
#else
    // 大文件用流式接口
    httplib::MultipartFormDataProvider provider;
    provider.name = "myfile";
    provider.filename = "hello.txt";
    provider.provider = [&](size_t offset, httplib::DataSink &sink){
        // offset 是已发送的偏移量
        const char arr[] = "hello world";
        auto ret = sink.write(arr + offset, sizeof(arr));
        std::cerr << "Client write:" << arr << std::endl;
        // 发送完成
        sink.done();
        return !!ret;
    };
    provider.content_type = "multipart/form-data";
    httplib::MultipartFormDataProviderItems provider_items;
    provider_items.push_back(provider);
    // header 的 Content-Type 会默认设置为 multipart/form-data，且自动加上 boundary
    httplib::Result res = cli.Post("/upload", {}, {}, provider_items);
#endif
    std::cerr << __FUNCTION__ << "\tpath:/upload\tres:" << (!!res) << std::endl;
    printResult(res);
}

void MainWindow::doDown()
{
    httplib::Client cli = getClient();
#if 0
    // 小文件一次性读取
    httplib::Result res = cli.Get("/download/0");
    if (res) {
        // Content-Disposition: attachment; filename=hello.txt
        std::string dispsition = res->get_header_value("Content-Disposition");
        std::cerr << dispsition << std::endl;
    }
#else
    // 大文件用流式接口
    httplib::Result res = cli.Get(
        "/download/0",
        [&](const httplib::Response &response) {
            std::cerr << "Client read:" << response.status << std::endl;
            return true;
        },
        [&](const char *data, size_t data_length) {
            std::cerr << "Client read:" << std::string(data, data_length) << std::endl;
            return true;
        });
#endif
    std::cerr << __FUNCTION__ << "\tpath:/download/0\tres:" << (!!res) << std::endl;
    printResult(res);

}
