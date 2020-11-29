#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

/**
 * @brief wav文件头结构体
 * @author 龚建波
 * @date 2020-11-12
 * @details
 * wav头是不定长格式，不过这里用的比较简单的格式
 * （数值以小端存储，不过pc一般是小端存储，暂不特殊处理）
 * 参照：https://www.cnblogs.com/ranson7zop/p/7657874.html
 * 参照：https://www.cnblogs.com/Ph-one/p/6839892.html
 */
struct EasyWavHead
{
    char riffFlag[4]; //文档标识，大写"RIFF"
    //从下一个字段首地址开始到文件末尾的总字节数。
    //该字段的数值加 8 为当前文件的实际长度。
    unsigned int riffSize; //数据长度
    char waveFlag[4]; //文件格式标识，大写"WAVE"
    char fmtFlag[4]; //格式块标识，小写"fmt "
    unsigned int fmtSize; //格式块长度，可以是 16、 18 、20、40 等
    unsigned short compressionCode; //编码格式代码，1为pcm
    unsigned short numChannels; //通道个数
    unsigned int sampleRate; //采样频率
    //该数值为:声道数×采样频率×每样本的数据位数/8。
    //播放软件利用此值可以估计缓冲区的大小。
    unsigned int bytesPerSecond; //码率（数据传输速率）
    //采样帧大小。该数值为:声道数×位数/8。
    //播放软件需要一次处理多个该值大小的字节数据,用该数值调整缓冲区。
    unsigned short blockAlign; //数据块对其单位
    //存储每个采样值所用的二进制数位数。常见的位数有 4、8、12、16、24、32
    unsigned short bitsPerSample; //采样位数（采样深度）
    char dataFlag[4]; //表示数据开头，小写"data"
    unsigned int dataSize; //数据部分的长度
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void paintEvent(QPaintEvent *event) override;

    //silk解码为pcm
    void decodeSilk(const QString &filepath);

    //生成wav(pcm)文件头信息
    //sampleRate: 采样频率
    //dataSize: pcm数据字节长度
    //return EasyWavHead: wav头
    static EasyWavHead createWavHead(int sampleRate,unsigned int dataSize);

private:
    Ui::MainWindow *ui;

    QByteArray pcmData;
};
#endif // MAINWINDOW_H
