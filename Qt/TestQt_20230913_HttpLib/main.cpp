#include <QApplication>
#include <thread>
#include <string>
#include "httplib.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    // 当前使用的 v0.14.0(2023-08-23)：https://github.com/yhirose/cpp-httplib/releases/tag/v0.14.0
    // 使用时可下载最新版本
    httplib::Server svr;
    std::thread th([&](){
        try {
            svr.set_error_handler([](const httplib::Request &req, httplib::Response &res) {
                std::cerr << "Server-error:" << req.method << "\t" << req.path << "\tstatus:" << res.status << std::endl;
                std::string json = R"({"message":"path error"})";
                res.set_content(json, "text/plain");
            });
            svr.set_logger([](const httplib::Request &req, const httplib::Response &res) {
                std::cerr << "Server-log:" << req.method << "\t" << req.path << "\tstatus:" << res.status << std::endl;
            });
            // 路由处理抛了异常，会被捕获，然后进入 set_error_handler 逻辑
            svr.Get("/hello", [](const httplib::Request &req, httplib::Response &res) {
                std::cerr << "Server-log:" << req.method << "\t" << req.path << "\t" << req.body << std::endl;
                for (auto it = req.params.begin(); it != req.params.end(); it++)
                {
                    std::cerr << "\t" << it->first << ": " << it->second << std::endl;
                }
                res.set_content(R"({"message":"get result"})", "appliation/json");
            });
            svr.Post("/hello", [](const httplib::Request &req, httplib::Response &res) {
                std::cerr << "Server-log:" << req.method << "\t" << req.path << "\t" << req.body << std::endl;
                std::string json = req.body;
                if (json.size() > 2 && json.at(0) == '{') {
                    json = R"({"message":"post result"})";
                } else {
                    json = R"({"message":"request is empty"})";
                }
                res.set_content(json, "appliation/json");
            });
            svr.Put("/hello", [](const httplib::Request &req, httplib::Response &res) {
                std::cerr << "Server-log:" << req.method << "\t" << req.path << "\t" << req.body << std::endl;
                for (auto it = req.params.begin(); it != req.params.end(); it++)
                {
                    std::cerr << "\t" << it->first << ": " << it->second << std::endl;
                }
                res.set_content(R"({"message":"put result"})", "appliation/json");
            });
            // 文件上传
            svr.Post("/upload", [](const httplib::Request &req, httplib::Response &res, const httplib::ContentReader &content_reader) {
                std::cerr << "Server-log: upload\t" << req.get_header_value("Content-Type") << std::endl;
                // 二进制数据可以用：multipart/form-data 和 application/octet-stream
                if (req.is_multipart_form_data()) {
                    httplib::MultipartFormDataItems files;
                    // 先拿到 file 信息，再流式读取
                    content_reader(
                        [&](const httplib::MultipartFormData &file) {
                            files.push_back(file);
                            std::cerr << "\tupload read " << file.filename << "\t" << file.content << std::endl;
                            return true;
                        },
                        [&](const char *data, size_t data_length) {
                            files.back().content.append(data, data_length);
                            std::cerr << "\tupload read:" << data_length << std::endl;
                            return true;
                        });
                } else {
                    std::string body;
                    content_reader([&](const char *data, size_t data_length) {
                        body.append(data, data_length);
                        std::cerr << "\tupload read:" << data_length << std::endl;
                        return true;
                    });
                    std::cerr << "\tupload read " << body << std::endl;
                }
                res.set_content(R"({"message":"upload result"})", "appliation/json");
            });
            // 文件下载
            svr.Get("/download/:id", [](const httplib::Request &req, httplib::Response &res) {
                std::cerr << "Server-log: download\t" << req.path_params.at("id") << "\t" << req.get_header_value("Content-Type") << std::endl;
                res.set_header("Cache-Control", "no-cache");
                res.set_header("Content-Disposition", "attachment; filename=hello.txt");
                res.set_chunked_content_provider(
                    "multipart/form-data", [](size_t offset, httplib::DataSink &sink) {
                        const char arr[] = "hello world";
                        auto ret = sink.write(arr + offset, sizeof(arr));
                        sink.done();
                        std::cerr << "\tdownload write:" << sizeof(arr) << std::endl;
                        return !!ret;
                    });
            });
            svr.listen("localhost", 12345);
        } catch(...) {
            std::cerr << "server error\n";
        }
    });

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    int ret = a.exec();
    svr.stop();
    if (th.joinable())
        th.join();
    return ret;
}
