#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QFile>
#include <QTime>
#include <QPainter>
#include <QDebug>

#include "SKP_Silk_SDK_API.h"

#define MAX_BYTES_PER_FRAME     1024
#define MAX_INPUT_FRAMES        5
#define MAX_FRAME_LENGTH        480
#define FRAME_LENGTH_MS         20
#define MAX_API_FS_KHZ          48

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->lineEdit->setText(qApp->applicationDirPath()+"/weixin.amr");

    //选择文件
    connect(ui->btnFile,&QPushButton::clicked,[=]{
        const QString filepath=QFileDialog::getOpenFileName(this);
        if(!filepath.isEmpty())
            ui->lineEdit->setText(filepath);
    });

    //解码
    connect(ui->btnDecode,&QPushButton::clicked,[=]{
        const QString filepath=ui->lineEdit->text();
        if(filepath.isEmpty())
            return;
        qDebug()<<"---------------------------------";
        qDebug()<<"sdk version"<<SKP_Silk_SDK_get_version();
        qDebug()<<"begin decode"<<QTime::currentTime();

        decodeSilk(filepath);
        qDebug()<<"end decode"<<QTime::currentTime();
        qDebug()<<"data size"<<pcmData.size();
        update();
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    if(pcmData.count()<4)
        return;
    painter.translate(0,height()/2);
    const int length=pcmData.count()/2;
    const short *datas=(const short *)pcmData.constData();
    //点的x间隔
    double xspace=width()/(double)length;
    //绘制采样点步进，测试用的固定值，文件比较大懒得算
    const int step=1;
    //qDebug()<<"step"<<step;
    for(int i=0;i<length-step;i+=step)
    {
        painter.drawLine(xspace*i,-datas[i]/150,xspace*(i+step),-datas[i+step]/150);
    }
}

void MainWindow::decodeSilk(const QString &filepath)
{
    //SDK地址：https://gitee.com/alvis/SILK_SDK_SRC
    //参照示例：SILK_SDK_SRC_v1.0.9\SILK_SDK_SRC_ARM_v1.0.9\test\Decoder.c
    //参照博客：https://blog.csdn.net/weixin_34292924/article/details/87987436
    QFile file(filepath);
    qDebug()<<"Filepath"<<filepath<<"exists"<<file.exists();
    if(file.size()<10||!file.open(QIODevice::ReadOnly))
        return;

    //先判断silk头
    //#!SILK_V3
    QByteArray read_temp=file.read(1);
    if(read_temp=="#"){
        read_temp+=file.read(8);
    }else{
        //微信的silk前面加个一个字节的0x02
        read_temp=file.read(9);
    }
    if(read_temp!="#!SILK_V3"){
        file.close();
        return;
    }

    SKP_SILK_SDK_DecControlStruct dec_ctrl;
    //采样率，可作为参数传入
    //Valid values are 8000,12000, 16000, 24000, 32000, 44100, and 48000.
    const int sample_rate=8000;
    dec_ctrl.API_sampleRate = sample_rate;
    dec_ctrl.framesPerPacket = 1;

    //创建解码器
    SKP_int32 dec_size;
    int ret = SKP_Silk_SDK_Get_Decoder_Size(&dec_size);
    qDebug()<<"SKP_Silk_SDK_Get_Decoder_Size return"<<ret<<"size"<<dec_size;
    SKP_uint8 *dec_state=new SKP_uint8[dec_size];
    //初始化解码器
    ret=SKP_Silk_SDK_InitDecoder(dec_state);
    qDebug()<<"SKP_Silk_SDK_InitDecoder return"<<ret;

    //默认解出来貌似是16bit的精度
    SKP_uint8 payload[MAX_BYTES_PER_FRAME * MAX_INPUT_FRAMES], *payload_ptr = NULL;
    SKP_int16 out[((FRAME_LENGTH_MS * MAX_API_FS_KHZ) << 1) * MAX_INPUT_FRAMES], *out_ptr = NULL;
    SKP_int16 n_bytes=0;
    SKP_int32 read_counter=0;
    SKP_int16 len=0;
    SKP_int16 total_len=0;
    SKP_int32 frames=0;
    pcmData.clear();

    //循环读取并解码
    while (true) {
        //读取有效数据大小
        read_counter=file.read((char*)&n_bytes,sizeof(SKP_int16));
        if(n_bytes<1||read_counter<1)
            break;
        //读取有效数据
        read_counter=file.read((char*)payload,n_bytes);
        //qDebug()<<"read_counter"<<read_counter<<n_bytes<<QByteArray((char*)dec_state,sizeof(SKP_uint8)).toHex();
        if( (SKP_int16)read_counter < n_bytes ) {
            break;
        }
        payload_ptr=payload;
        out_ptr=out;
        total_len = 0;
        frames = 0;
        do {
            //解码
            ret = SKP_Silk_SDK_Decode(dec_state, &dec_ctrl, 0, payload_ptr, n_bytes, out_ptr, &len);
            if (ret) {
                qDebug()<<"SKP_Silk_SDK_Decode returned"<<ret;
            }

            frames++;
            out_ptr += len;
            total_len += len;
            if (frames > MAX_INPUT_FRAMES) {
                qDebug()<<"frames > MAX_INPUT_FRAMES"<<frames<<total_len;
                out_ptr = out;
                total_len = 0;
                frames = 0;
            }
        } while (dec_ctrl.moreInternalDecoderFrames);

        //将解码后的数据pcm保存
        pcmData.append((const char *)out,total_len*2);
    }

    //清理
    file.close();
    delete[]dec_state;

    //以wav(pcm s16)格式写入
    file.setFileName(filepath+".pcm.wav");
    if(file.open(QIODevice::WriteOnly)){
        //自定义的wav头结构体
        EasyWavHead head=createWavHead(sample_rate,pcmData.count());
        file.write((const char*)&head,sizeof(head));
        file.write(pcmData);
        file.close();
    }
}

EasyWavHead MainWindow::createWavHead(int sampleRate, unsigned int dataSize)
{
    //默认貌似是16位精度，单通道
    const int bits=16;
    const int channels=1;
    const int head_size = sizeof(EasyWavHead);
    EasyWavHead wav_head;
    memset(&wav_head, 0, head_size);

    memcpy(wav_head.riffFlag, "RIFF", 4);
    memcpy(wav_head.waveFlag, "WAVE", 4);
    memcpy(wav_head.fmtFlag, "fmt ", 4);
    memcpy(wav_head.dataFlag, "data", 4);

    //出去头部前8个字节的长度，用的44字节的格式头，所以+44-8=36
    wav_head.riffSize = dataSize + 36;
    //不知道干嘛的
    wav_head.fmtSize = 16;
    //1为pcm
    wav_head.compressionCode = 0x01;
    wav_head.numChannels = channels;
    wav_head.sampleRate = sampleRate;
    wav_head.bytesPerSecond = (bits / 8) * channels * sampleRate;
    wav_head.blockAlign = (bits / 8) * channels;
    wav_head.bitsPerSample = bits;
    //除去头的数据长度
    wav_head.dataSize = dataSize;

    return wav_head;
}

