#pragma once

#include <QObject>
#include <QSharedPointer>

//枚举放到QObject注册到QML
class TalkData : public QObject
{
    Q_OBJECT
public:
    using QObject::QObject;
    //数据类型
    enum DataType
    {
        //文本
        Text,
        //图片
        Image,
        //音频
        Audio,
        //视频
        Video
    };
    Q_ENUM(DataType)

    //数据处理状态
    enum DataStatus
    {
        //无效数据
        DataError,
        //待解析
        DataReady,
        //解析中
        ParseOn,
        //解析成功
        ParseSuccess,
        //解析错误
        ParseError
    };
    Q_ENUM(DataStatus)
};

//消息基类
struct TalkDataBasic
{
    //消息id
    qint64 id;
    //消息所属用户
    QString user;
    //发送者
    QString sender;
    //发送时间 ms
    qint64 datetime;
    //消息类型：文本、语音、图片
    TalkData::DataType type = TalkData::Text;
    //处理状态
    TalkData::DataStatus status = TalkData::DataError;

    virtual ~TalkDataBasic(){}
};

//文本消息
struct TalkDataText : public TalkDataBasic
{
    //文本信息
    QString text;
};

//图片消息
//struct TalkDataImage : public TalkDataBasic

//语音消息
struct TalkDataAudio : public TalkDataBasic
{
    //音频路径
    QString path;
    //音频转文本
    QString text;
    //音频时长 ms
    qint64 duration;

    QString durationStr() const;
};

//视频消息
//struct TalkDataVideo : public TalkDataBasic
