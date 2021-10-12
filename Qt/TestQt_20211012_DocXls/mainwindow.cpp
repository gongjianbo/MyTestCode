#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QAxObject>
#include <QFile>
#include <QDir>
#include <QScopeGuard>
#include <QDialog>
#include <QApplication>
#include <QFileDialog>
#include <QDir>
#include <QFileInfo>
#include <QDesktopServices>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->btnDoc,&QPushButton::clicked,this,&MainWindow::testDoc);
    connect(ui->btnXls,&QPushButton::clicked,this,&MainWindow::testXls);
    connect(ui->btnDir,&QPushButton::clicked,[]{
        QDesktopServices::openUrl(QUrl("file:///"+QApplication::applicationDirPath()+"/temp"));
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::testDoc()
{
    static int index=0;
    index++;
    //不能自己创建目录
    QString doc_dir=QApplication::applicationDirPath()+"/temp";
    QString doc_path=doc_dir+QString("/mydoc%1.doc").arg(index);
    QDir dir(doc_dir);
    if(dir.exists() || dir.mkdir(doc_dir)){
        QFile file(doc_path);
        //操作前确保可以创建or修改，避免被占用
        if(file.exists()&&!file.remove()){
            qDebug()<<"doc remove failed";
            return;
        }
    }else{
        qDebug()<<"doc create failed";
        return;
    }
    std::unique_ptr<QAxObject> word(new QAxObject());

    bool is_wps = true;
    if(!word->setControl("KWPS.Application" )){
        qDebug() << "open wps word failed";
        is_wps = false;
        if(!word->setControl("Word.Application")){
            qDebug() << "open office word failed";
            return;
        }
    }
    word->setProperty("Visible",false);
    word->setProperty("EnableEvents", false);
    word->setProperty("DisplayAlerts", false);
    QAxObject *docs=word->querySubObject("Documents");
    if(!docs){
        word->dynamicCall("Quit()");
        qDebug()<<"query docs failed.";
        return;
    }
    //docs->dynamicCall("Add(QString)","xxxx/template.docx");
    docs->dynamicCall("Add()");
    QAxObject *doc=word->querySubObject("ActiveDocument");
    if(!doc){
        word->dynamicCall("Quit()");
        qDebug()<<"query active doc failed.";
        return;
    }
    QAxObject *selection = word->querySubObject("Selection");
    if(selection){
        QAxObject *font = selection->querySubObject("Font");
        if(font){
            font->setProperty("Name","SimSun");
            font->setProperty("Size",10.5);
        }
        selection->dynamicCall("TypeText(const QString&)",QString("Hello Office."));
    }else{
        qDebug()<<"query selection failed.";
    }

    qDebug()<<"save"<<QDir::toNativeSeparators(doc_path);
    //MS Office Word保存格式枚举参见
    //https://docs.microsoft.com/zh-cn/office/vba/api/word.wdsaveformat
    doc->dynamicCall("SaveAs(const QString&,int))",QDir::toNativeSeparators(doc_path),0);
    doc->dynamicCall("Close(bool)", true);
    word->dynamicCall("Quit()");
    qDebug()<<"export doc end";
}

void MainWindow::testXls()
{
    static int index=0;
    index++;
    QString doc_dir=QApplication::applicationDirPath()+"/temp";
    QString doc_path=doc_dir+QString("/myxls%1.xls").arg(index);
    QFileInfo fileinfo(doc_path);
    if(fileinfo.dir().exists() || fileinfo.dir().mkdir(fileinfo.dir().dirName())){
        QFile file(doc_path);
        //操作前确保可以创建or修改，避免被占用
        if(file.exists() && !file.remove()){
            qDebug()<<"xls remove failed";
            return;
        }
    }else{
        qDebug()<<"xls create failed";
        return;
    }
    std::unique_ptr<QAxObject> excel(new QAxObject());

    bool is_wps = true;
    if(!excel->setControl("KET.Application" )){
        qDebug() << "open wps excel failed";
        is_wps = false;
        if(!excel->setControl("Excel.Application")){
            qDebug() << "open office excel failed";
            return;
        }
    }
    excel->setProperty("Visible",false);
    excel->setProperty("EnableEvents", false);
    excel->setProperty("DisplayAlerts", false);
    QAxObject *books = excel->querySubObject("WorkBooks");
    if(!books){
        excel->dynamicCall("Quit()");
        qDebug()<<"query books failed.";
        return;
    }
    books->dynamicCall("Add()");
    QAxObject *book = excel->querySubObject("ActiveWorkBook");
    if(!book){
        excel->dynamicCall("Quit()");
        qDebug()<<"query active book failed.";
        return;
    }
    QAxObject *sheets = book->querySubObject("Sheets");
    if(sheets){
        QAxObject *sheet = sheets->querySubObject("Add()");
        if(sheet){
            //其他的一些操作
            //http://blog.sina.com.cn/s/blog_a6fb6cc90101gv2p.html
            //QAxObject *cell =sheet->querySubObject("Cells(int,int)",1,1);
            //cell->dynamicCall("SetValue(const QVariant&)", 1992);
            QStringList head={
                QString("标记名称"),
                QString("开始时间"),
                QString("结束时间"),
                QString("备注")
            };
            for(int i=0; i<head.size(); i++)
            {
                QAxObject *col = sheet->querySubObject("Cells(int,int)", 1, i+1);
                if(col){
                    col->setProperty("ColumnWidth", 30); //列宽
                    col->dynamicCall("SetValue(const QVariant&)", head.at(i));
                }
            }
                QAxObject *col = sheet->querySubObject("Cells(int,int)", 2, 3);
                if(col){
                    col->setProperty("NumberFormatLocal", "@"); //保持文本形式，不自动转格式
                    col->dynamicCall("SetValue(const QVariant&)", "12:12:12.123");
                }
            QAxObject *range = sheet->querySubObject("Range(const QString)", QString("A3:B4"));
            if(range && !range->isNull()){
                range->setProperty("NumberFormatLocal", "@");
                int k=0;
                QVariantList value;
                for(int i=0;i<2;i++)
                {
                    QVariantList sub;
                    for(int j=0;j<2;j++)
                    {
                        //sub.append("测试:"+QString::number(k++));
                        sub.append("12:12:12.123");
                    }
                    value.append(QVariant(sub));
                }
                range->dynamicCall("SetValue(const QVariant&)", QVariant(value));
            }
        }
    }else{
        qDebug()<<"query sheets failed.";
    }

    qDebug()<<"save"<<QDir::toNativeSeparators(doc_path);
    //MS Office Excel保存格式枚举参见
    //https://docs.microsoft.com/zh-cn/office/vba/api/excel.xlfileformat
    book->dynamicCall("SaveAs(const QString&,int))",QDir::toNativeSeparators(doc_path),56);
    book->dynamicCall("Close(bool)", true);
    excel->dynamicCall("Quit()");
    qDebug()<<"export xls end";
}

