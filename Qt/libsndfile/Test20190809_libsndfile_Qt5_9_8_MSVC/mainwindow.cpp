#include "mainwindow.h"
#include "ui_mainwindow.h"

#pragma comment(lib,"E:/git_space/vcpkg/installed/x86-windows-static/lib/libsndfile.lib")
#pragma comment(lib,"E:/git_space/vcpkg/installed/x86-windows-static/lib/FLAC.lib")
#pragma comment(lib,"E:/git_space/vcpkg/installed/x86-windows-static/lib/ogg.lib")
#pragma comment(lib,"E:/git_space/vcpkg/installed/x86-windows-static/lib/vorbis.lib")

#include <QFileDialog>
#include <QDebug>

//typedef long long sf_count_t
//所以用qint64来表示更清楚点
qint64 vioFileLength(void *user_data)
{
    AudioPacket* packet=(AudioPacket*)user_data;
    qDebug()<<"vioFileLength"<<packet->data.count();
    return packet->data.count();
}

qint64 vioFileTell(void *user_data); //声明，下面要用
qint64 vioFileSeek(sf_count_t offset, int whence, void *user_data)
{
    //qDebug()<<"vioFileSeek offset"<<offset<<whence;
    AudioPacket* packet=(AudioPacket*)user_data;
    //qDebug()<<"packet/offset"<<packet->offset;
    qint64 position = 0;
    switch (whence)
    {
    case SEEK_SET :
        position = offset;
        break;
    case SEEK_CUR :
        position = vioFileTell(user_data) + offset;
        break;
    case SEEK_END :
        position = packet->data.count() + offset - 1;
        break;
    default :
        return (-1);
        break;
    }

    if (position >= packet->data.count())
        position = packet->data.count() - 1;
    else if (position < 0)
        position = 0;

    packet->offset=position;
    //qDebug()<<"seek/position"<<position;
    return position;
}

qint64 vioFileRead(void *ptr, sf_count_t count, void *user_data)
{
    //qDebug()<<"vioFileRead count"<<count;
    AudioPacket* packet=(AudioPacket*)user_data;
    //qDebug()<<"packet/offset"<<packet->offset;
    //qint64 offset = vioFileTell(user_data);
    if(packet->offset>=packet->data.count()){
        return 0;
    }

    if(packet->offset+count>packet->data.count()){
        count = packet->data.count()-packet->offset;
    }

    memcpy((char*)ptr,packet->data.data()+packet->offset,count);

    //qDebug()<<"read count"<<count;//<<QByteArray(packet->data.data()+packet->offset,count).toHex(' ');

    packet->offset+=count;
    return count;
}

qint64 vioFileWrite(const void *ptr, sf_count_t count, void *user_data)
{
    Q_UNUSED(ptr)
    Q_UNUSED(count)
    Q_UNUSED(user_data)
    //这里我只用到了解析，没用生成
    return 0;
}

qint64 vioFileTell(void *user_data)
{
    AudioPacket* packet=(AudioPacket*)user_data;

    //return ftell(cpfInfos->filePtr)-cpfInfos->StartOffset;
    return packet->offset;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //test();

    connect(ui->btnTest,&QPushButton::clicked,this,[=](){
        const QString filepath=QFileDialog::getOpenFileName(this);
        if(filepath.isEmpty())
            return;
        ui->testEdit->setText(filepath);
        test(filepath);
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::test(const QString &filepath)
{
    qDebug()<<"test:"<<filepath;
    QString fileName = filepath; //"C:/Users/zhaozhao/Desktop/test/wav-gsm610.wav";
    QByteArray rawdata;
    QFile rawfile(fileName);
    if(rawfile.open(QIODevice::ReadOnly)){
        rawdata=rawfile.readAll();
        rawfile.close();
        qDebug()<<"read close"<<rawdata.count();
    }
    SF_VIRTUAL_IO vio;
    vio.get_filelen=&vioFileLength;
    vio.seek=&vioFileSeek;
    vio.read=&vioFileRead;
    vio.write=&vioFileWrite;
    vio.tell=&vioFileTell;
    //qDebug()<<vio.get_filelen;
    AudioPacket packet{
        0,rawdata
    };

    SndfileHandle snd(vio,&packet);
    //目前文件格式可以正确解析，但是不能正确read出来pcm数据
    qDebug()<<"sample-channel:"<<snd.samplerate()<<snd.channels();
    qDebug()<<snd.strError();

    QByteArray array(65536,0);
    const int count=snd.read((short *)array.data(),65536/2);
    if(count<65536/2)
        array.resize(count*2);

    //c语言接口gsm编码不能seek，而且sf_read_short读到的长度貌似和cpp接口的不一样，
    //不过adobe audio看幅度波形是一样的，也懒得在去看源码了
    /*SNDFILE * sndfile;
    SF_INFO sfinfo = {0, 0, 0, 0, 0, 0};
    //sfinfo.format=(SF_FORMAT_WAV|SF_FORMAT_GSM610);
    sndfile = sf_open(fileName.toUtf8(), SFM_READ, &sfinfo);
    qDebug()<<"seekable:"<<sfinfo.seekable;
    qDebug()<<"sample-channel:"<<sfinfo.samplerate<<sfinfo.channels;
    //gsm格式不能用seek，否则会有提示错误，并且返回size为-1
    //int size = 65536;//sf_seek(sndfile, 0, SEEK_END);
    //sf_seek(sndfile, 0, SEEK_END);
    //sf_seek(sndfile, 0, SEEK_SET);
    QByteArray array(65536, 0);

    const int read_size=sf_read_short(sndfile, (short *)array.data(), 65536/2);
    qDebug()<<"read size:"<<read_size<<sf_strerror(sndfile);
    array.resize(read_size*2);*/


    QFile file(fileName+".pcm");
    if(file.open(QIODevice::ReadWrite)){
        file.write(array);
        file.waitForBytesWritten(1000);
        file.close();
        qDebug()<<"write close"<<array.count();
    }

    /*QBuffer buffer(&array);
    buffer.open(QIODevice::ReadWrite);

    QAudioFormat audioFormat;
    audioFormat.setCodec("audio/pcm");
    audioFormat.setByteOrder(QAudioFormat::LittleEndian);
    audioFormat.setSampleRate(sfinfo.samplerate);
    audioFormat.setChannelCount(sfinfo.channels);
    //audioFormat.setSampleRate(snd.samplerate());
    //audioFormat.setChannelCount(snd.channels());
    audioFormat.setSampleSize(16);
    audioFormat.setSampleType(QAudioFormat::SignedInt);
    QAudioOutput *audio = new QAudioOutput(QAudioDeviceInfo::defaultOutputDevice(), audioFormat );
    audio->start(&buffer);

    connect(audio,&QAudioOutput::stateChanged,this,[=](QAudio::State state){
        qDebug()<<state;
        if(state!=QAudio::ActiveState){
            audio->stop();
            loop.quit();
        }
    });
    loop.exec();
    qDebug()<<"loop end";*/

}
