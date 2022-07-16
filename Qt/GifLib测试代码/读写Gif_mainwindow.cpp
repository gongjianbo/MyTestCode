#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "gif_lib.h"
#include <QCoreApplication>
#include <QDebug>
#include <QThread>
#include <thread>
#include <memory>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->btnDecode,&QPushButton::clicked,[=]{
        gifToImage(QString(qApp->applicationDirPath()+"/test2.gif"));
    });

    connect(ui->btnDecode2,&QPushButton::clicked,[=]{
        gifToImage2(QString(qApp->applicationDirPath()+"/create.gif"));
    });

    connect(ui->btnEncode,&QPushButton::clicked,[=]{
        QString path=QString("out-%1.png");
        QStringList path_list;
        for(int i=0;i<9;i++)
            path_list.push_back(path.arg(i));
        imageToGif(path_list);
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
            qDebug()<<"type"<<record_type;
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
                qDebug()<<"extension"<<ext_code;
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
                    qDebug()<<"gcb:"<<gcb.DelayTime<<gcb.TransparentColor;
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

void MainWindow::gifToImage2(const QString &gifpath)
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
        error_code = DGifSlurp(gif_file);
        if(error_code!=GIF_OK){
            qDebug()<<"gif slurp error.";
            return;
        }

        qDebug()<<"screen size(w*h)"<<gif_file->SWidth<<"*"<<gif_file->SHeight;
        qDebug()<<"screen colors"<<(1 << gif_file->SColorResolution)
               <<"screen bgcolor"<<gif_file->SBackGroundColor
              <<"pixel aspect byte"<<(unsigned)gif_file->AspectByte;

        QVector<GifPixelType> screen_buf(gif_file->SWidth*gif_file->SHeight);
        memset(screen_buf.data(),gif_file->SBackGroundColor,sizeof(GifPixelType)*screen_buf.size());
        QImage img_temp=QImage(gif_file->SWidth,gif_file->SHeight,QImage::Format_RGB32);
        int trans_color = -1;
        for (int im = 0; im < gif_file->ImageCount; im++)
        {
            qDebug()<<"process num"<<im;
            SavedImage *image = &gif_file->SavedImages[im];
            for (ExtensionBlock *ep = image->ExtensionBlocks;
                 ep < image->ExtensionBlocks + image->ExtensionBlockCount;
                 ep++)
            {
                bool last = (ep - image->ExtensionBlocks == (image->ExtensionBlockCount - 1));
                if (ep->Function == GRAPHICS_EXT_FUNC_CODE)
                {
                    GraphicsControlBlock gcb;
                    if (DGifExtensionToGCB(ep->ByteCount, ep->Bytes, &gcb) == GIF_ERROR) {
                        qDebug()<<"invalid graphics control block";
                        return;
                    }
                    qDebug()<<"disposal mode"<< gcb.DisposalMode;
                    qDebug()<<"user input fla"<<(gcb.UserInputFlag ? "on" : "off");
                    qDebug()<<"delay"<<gcb.DelayTime;
                    qDebug()<<"transparent index"<<gcb.TransparentColor;
                    trans_color=gcb.TransparentColor;
                    QThread::msleep(gcb.DelayTime*10);
                }
                else if (!last
                         && ep->Function == APPLICATION_EXT_FUNC_CODE
                         && ep->ByteCount >= 11
                         && (ep+1)->ByteCount >= 3
                         && memcmp(ep->Bytes, "NETSCAPE2.0", 11) == 0)
                {
                    unsigned char *params = (++ep)->Bytes;
                    unsigned int loopcount = params[1] | (params[2] << 8);
                    qDebug()<< "netscape loop "<<loopcount;
                }
                else
                {
                    while (!last && ep[1].Function == CONTINUE_EXT_FUNC_CODE) {
                        ++ep;
                        last = (ep - image->ExtensionBlocks == (image->ExtensionBlockCount - 1));
                    }
                }
            }

            qDebug()<<"left"<<image->ImageDesc.Left
                   <<"top"<<image->ImageDesc.Top
                  <<"width"<<image->ImageDesc.Width
                 <<"height"<<image->ImageDesc.Height
                <<"color"<<image->ImageDesc.ColorMap->ColorCount;
            if (image->ImageDesc.Interlace)
                qDebug()<<"image interlaced";

            //如果没有局部色彩管理就用全局的
            ColorMapObject *color_map = (image->ImageDesc.ColorMap ? image->ImageDesc.ColorMap : gif_file->SColorMap);
            if(color_map == NULL){
                qDebug()<<"color map error.";
                return;
            }

            QImage img=img_temp.copy();
            GifColorType *color_map_entry;
            for (int i = image->ImageDesc.Top; i < image->ImageDesc.Top+image->ImageDesc.Height; i++)
            {
                //for (int j = 0; j < image->ImageDesc.Width; j++)
                memcpy(screen_buf.data()+i*gif_file->SWidth+image->ImageDesc.Left,
                       image->RasterBits+(i-image->ImageDesc.Top)*image->ImageDesc.Width,
                       image->ImageDesc.Width);
                for (int j = image->ImageDesc.Left; j < image->ImageDesc.Left+image->ImageDesc.Width; j++)
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

void
GifFreeSavedImages2(GifFileType *GifFile)
{
    SavedImage *sp;

    if ((GifFile == NULL) || (GifFile->SavedImages == NULL)) {
        return;
    }
    for (sp = GifFile->SavedImages;
         sp < GifFile->SavedImages + GifFile->ImageCount; sp++) {
        if (sp->ImageDesc.ColorMap != NULL) {
            GifFreeMapObject(sp->ImageDesc.ColorMap);
            sp->ImageDesc.ColorMap = NULL;
        }

        if (sp->RasterBits != NULL)
            free((char *)sp->RasterBits);

        GifFreeExtensions(&sp->ExtensionBlockCount, &sp->ExtensionBlocks);
    }
    free((char *)GifFile->SavedImages);
    GifFile->SavedImages = NULL;
}

//https://blog.csdn.net/orchestra56/article/details/79387782
//https://www.ihubin.com/blog/audio-video-basic-19-giflib-handle-gif/
//http://giflib.sourceforge.net/gif_lib.html#idm73
void MainWindow::imageToGif(const QStringList &imgpaths)
{
    qDebug()<<"write begin.";
    //没释放
    QVector<GifByteType> red_buffer(300*300);
    QVector<GifByteType> green_buffer(300*300);
    QVector<GifByteType> blue_buffer(300*300);

    //以编码方式打开文件
    int error_code;
    GifFileType *gif_file=EGifOpenFileName("create.gif",false,&error_code);
    if(gif_file == NULL){
        const char* gif_error = GifErrorString(error_code);
        qDebug() << "open error:" << gif_error;
        return;
    }

    gif_file->SWidth=300;
    gif_file->SHeight=300;
    gif_file->SColorResolution=8;
    gif_file->SBackGroundColor=0;
    gif_file->SColorMap=NULL;
    gif_file->SavedImages=NULL;
    gif_file->ImageCount=0;
    gif_file->ExtensionBlockCount=0;
    gif_file->ExtensionBlocks=NULL;

    int map_size=256;
    for(int i = 0; i < imgpaths.count(); i++) {
        qDebug()<<imgpaths.at(i);
        QImage img(imgpaths.at(i));
        SavedImage *image = GifMakeSavedImage(gif_file, NULL);
        if(img.width()<300||img.height()<300)
            continue;
        for(int row=0;row<300;row++)
        {
            for(int col=0;col<300;col++)
            {
                red_buffer[row*300+col]=img.pixelColor(col,row).red();
                green_buffer[row*300+col]=img.pixelColor(col,row).green();
                blue_buffer[row*300+col]=img.pixelColor(col,row).blue();
            }
        }
        if ((image->ImageDesc.ColorMap = GifMakeMapObject(256, NULL)) == NULL){
            qDebug()<<"Failed to allocate memory required, aborted.";
            return;
        }
        image->RasterBits  = (GifPixelType *)malloc(sizeof(GifPixelType) * 300 * 300);
        //该函数已废弃
        if (GifQuantizeBuffer(300, 300, &map_size,
                              red_buffer.data(),
                              green_buffer.data(),
                              blue_buffer.data(),
                              image->RasterBits,
                              image->ImageDesc.ColorMap->Colors) == GIF_ERROR)
        {
            qDebug()<<"error";
            return;
        }

        qDebug()<<"MakeSavedImage"<<i<<"mapsize"<<map_size;

        image->ImageDesc.Left = 0;
        image->ImageDesc.Top = 0;
        image->ImageDesc.Width = 300;
        image->ImageDesc.Height = 300;
        image->ImageDesc.Interlace = false;
        //image->ImageDesc.ColorMap = color_map;
        //image->RasterBits = out_buffer;

        GraphicsControlBlock gcb;
        gcb.DisposalMode = DISPOSAL_UNSPECIFIED;
        gcb.DelayTime = 10;
        gcb.UserInputFlag = false;
        gcb.TransparentColor = NO_TRANSPARENT_COLOR;

        qDebug()<<"GCB To Saved"<<i;
        EGifGCBToSavedExtension(&gcb, gif_file, i);

        if(i==0){
            unsigned char params[3] = {1, 0, 0};
            //Create a Netscape 2.0 loop block
            if (GifAddExtensionBlock(&image->ExtensionBlockCount,
                                     &image->ExtensionBlocks,
                                     APPLICATION_EXT_FUNC_CODE,
                                     11,
                                     (unsigned char *)"NETSCAPE2.0")==GIF_ERROR) {
                qDebug()<<"out of memory while adding loop block.";
                return;
            }
            //params[1] = (intval & 0xff);
            //params[2] = (intval >> 8) & 0xff;
            if (GifAddExtensionBlock(&image->ExtensionBlockCount,
                                     &image->ExtensionBlocks,
                                     0, sizeof(params), params) == GIF_ERROR) {
                qDebug()<<"out of memory while adding loop continuation.";
                return;
            }
        }
    }

    qDebug()<<gif_file->ImageCount;
    int saved_count=gif_file->ImageCount;
    SavedImage *saved_images=gif_file->SavedImages;
    //关闭GIF并释放相关存储。
    EGifSpew(gif_file);
    qDebug()<<gif_file->ImageCount;
    {
        SavedImage *sp;
        for (sp = saved_images;
             sp < saved_images + saved_count; sp++) {
            if (sp->ImageDesc.ColorMap != NULL) {
                GifFreeMapObject(sp->ImageDesc.ColorMap);
                sp->ImageDesc.ColorMap = NULL;
            }

            if (sp->RasterBits != NULL)
                free((char *)sp->RasterBits);

            GifFreeExtensions(&sp->ExtensionBlockCount, &sp->ExtensionBlocks);
        }
        free((char *)saved_images);
        saved_images = NULL;
        sp=NULL;
    }

    qDebug()<<"write finish.";
}

