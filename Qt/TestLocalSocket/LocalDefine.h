#pragma once
#include <string>
#include <QString>
#include <QDataStream>
//! 数据字段定义
//! 数据头[4]+不含头的长度[4]+一级类型[1]+二级类型[1]+数据[N]+数据尾/校验[2]
//! 一级类型为大的消息/模块分组，二级类型为该分组下的具体操作
//! 数据长度以memcpy int变量设置和读取，以PC默认的大小端
//! 头尾暂时为定值，兼容TCP数据流的方式进程处理
//! 数据序列化使用 QDataStream

//帧头
static const unsigned char Local_Msg_Head[4]={0x0F,0xF0,0xFF,0x33};
//帧尾 /r/n
static const unsigned char Local_Msg_Tail[2]={0x0D,0x0A};
//server地址，可以放到配置文件好在有冲突时进行修改
static const char *VoicePrint_Server="VoicePrintServer";

//消息/模块分组,switch(quint8)
//LEM:local message enum
namespace LMEType
{
enum : unsigned char
{
    None=0x00      //无效
    ,Network       //网络状态，连接断开等
    ,Feature=0x20  //特征提取
    ,Compare       //声纹比对
    ,Cluster       //聚类分析
};
}

//特征提取的二级类型
namespace LMEFeature
{
enum : unsigned char
{
    None=0x00        //无效
    ,AppendFile      //添加文件列表
    ,EditFile        //编辑文件
    ,RemoveFile      //删除文件列表
    ,ExtractProgress //提取进度
    ,RequestProgress //请求反馈提取进度，默认不发
};
}

//声纹和聚类任务的二级类型
namespace LMETask
{
enum : unsigned char
{
    None=0x00     //无效
    ,Request      //任务请求
    ,Response     //任务结果
    ,Progress     //任务进度
};
}

