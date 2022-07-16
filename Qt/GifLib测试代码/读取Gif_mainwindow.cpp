#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "gif_lib.h"
#include <QCoreApplication>
#include <QDebug>
#include <QThread>
#include <thread>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->pushButton,&QPushButton::clicked,[=]{
        gifToImage(QString(qApp->applicationDirPath()+"/test2.gif"));
    });

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::gifToImage(const QString &gifpath)
{
    std::thread th([=]{
        qDebug()<<"gif to image:"<<gifpath;
        const QByteArray gifpathd = gifpath.toLocal8Bit();
        const char *filepath = gifpathd.constData();
        int error_code;
        //解码器打开文件
        GifFileType *gif_file = DGifOpenFileName(filepath, &error_code);
        if (gif_file  == NULL) {
            const char* gif_error = GifErrorString(error_code);
            qDebug() << "open error:" << gif_error;
            return;
        }
        qDebug() << "gif height:" << gif_file->SHeight << " width:" << gif_file->SWidth;
        if (gif_file->SHeight<0 || gif_file->SWidth<0) {
            qDebug() << "size error.";
            return;
        }

        QVector<GifPixelType> screen_buf(gif_file->SWidth*gif_file->SHeight);
        //The way Interlaced image should be read - offsets and jumps...
        int interlaced_offset[] = { 0, 4, 2, 1 };
        int interlaced_jumps[] = { 8, 8, 4, 2 };
        GifByteType *extension;
        GifRecordType record_type;
        ColorMapObject *color_map;
        int ext_code;
        int the_col;
        int the_row;
        int the_width;
        int the_height;
        int trans_color = -1;
        memset(screen_buf.data(),gif_file->SBackGroundColor,sizeof(GifPixelType)*screen_buf.size());
        QImage img_temp=QImage(gif_file->SWidth,gif_file->SHeight,QImage::Format_RGB32);
        do {
            error_code = DGifGetRecordType(gif_file, &record_type);
            if (error_code == GIF_ERROR){
                const char* gif_error = GifErrorString(gif_file->Error);
                qDebug() << "get record error:" << gif_error;
                return;
            }
            //qDebug()<<"type"<<record_type;
            switch (record_type)
            {
            case IMAGE_DESC_RECORD_TYPE:
                error_code = DGifGetImageDesc(gif_file);
                if (error_code == GIF_ERROR) {
                    const char* gif_error = GifErrorString(gif_file->Error);
                    qDebug() << "get image error:" << gif_error;
                    return;
                }
                //memset(screen_buf.data(),gif_file->SBackGroundColor,sizeof(GifPixelType)*screen_buf.size());
                the_row = gif_file->Image.Top;
                the_col = gif_file->Image.Left;
                the_width = gif_file->Image.Width;
                the_height = gif_file->Image.Height;
                //qDebug()<<the_row<<the_col<<the_width<<the_height<<gif_file->Image.Interlace<<gif_file->SBackGroundColor;
                if (the_col+the_width>gif_file->SWidth || the_row+the_height>gif_file->SHeight) {
                    qDebug()<<"Image X is not confined to screen dimension, aborted.";
                    return;
                }
                if (gif_file->Image.Interlace){
                    for (int i = 0; i < 4; i++)
                    {
                        for (int j=the_row+interlaced_offset[i]; j<the_row+the_height; j+=interlaced_jumps[i])
                        {
                            error_code = DGifGetLine(gif_file, &screen_buf[j*4+the_col], the_width);
                            if (error_code == GIF_ERROR){
                                const char* gif_error = GifErrorString(gif_file->Error);
                                qDebug() << "get line error:" << gif_error;
                            }
                        }
                    }
                }else{
                    for (int i = 0; i < the_height; i++)
                    {
                        error_code = DGifGetLine(gif_file, &screen_buf[(the_row+i)*gif_file->SWidth+the_col],the_width);
                        if (error_code == GIF_ERROR){
                            const char* gif_error = GifErrorString(gif_file->Error);
                            qDebug() << "get line error:" << gif_error;
                        }
                    }
                }
                //如果没有局部色彩管理就用全局的
                color_map = (gif_file->Image.ColorMap ? gif_file->Image.ColorMap : gif_file->SColorMap);
                if(color_map == NULL){
                    qDebug()<<"color map error.";
                    return;
                }else{
                    QImage img=img_temp.copy();
                    GifColorType *color_map_entry;
                    for (int i = the_row; i < the_height+the_row; i++)
                    {
                        for (int j = the_col; j < the_width+the_col; j++)
                        {
                            if(trans_color!=-1&&trans_color==screen_buf[i*gif_file->SWidth+j])
                                continue;
                            color_map_entry = &color_map->Colors[screen_buf[i*gif_file->SWidth+j]];
                            img.setPixelColor(j,i,QColor(
                                                  color_map_entry->Red,
                                                  color_map_entry->Green,
                                                  color_map_entry->Blue));
                        }
                    }
                    img_temp=img;
                    //qDebug()<<img.width()<<img.height();
                    //只是测试，所以在线程中直接操作ui
                    ui->label->setPixmap(QPixmap::fromImage(img));
                }
                break;
            case EXTENSION_RECORD_TYPE:
                error_code = DGifGetExtension(gif_file, &ext_code, &extension);
                if (error_code == GIF_ERROR){
                    const char* gif_error = GifErrorString(gif_file->Error);
                    qDebug() << "get extension error:" << gif_error;
                    return;
                }
                if(ext_code==GRAPHICS_EXT_FUNC_CODE){
                    GraphicsControlBlock gcb;
                    if(extension==NULL){
                        qDebug()<<"gcb extension null";
                        return;
                    }
                    error_code = DGifExtensionToGCB(extension[0], extension+1, &gcb);
                    if (error_code == GIF_ERROR) {
                        const char* gif_error = GifErrorString(gif_file->Error);
                        qDebug() << "extension to gcb error:" << gif_error;
                        return;
                    }
                    trans_color = gcb.TransparentColor;
                    //qDebug()<<"gcb:"<<gcb.DelayTime<<gcb.TransparentColor;
                    QThread::msleep(gcb.DelayTime*10);
                }
                while (extension != NULL)
                {
                    error_code = DGifGetExtensionNext(gif_file, &extension);
                    if (error_code == GIF_ERROR){
                        const char* gif_error = GifErrorString(gif_file->Error);
                        qDebug() << "get extension next error:" << gif_error;
                    }
                }
                break;
            case TERMINATE_RECORD_TYPE:
                break;
            default:
                break;
            }
        } while (record_type != TERMINATE_RECORD_TYPE);

        //关闭文件
        if (DGifCloseFile(gif_file, &error_code) == GIF_ERROR){
            const char* gif_error = GifErrorString(error_code);
            qDebug() << "close error:" << gif_error;
            return;
        }
        qDebug()<<"process finish";
    });
    th.detach();
}

