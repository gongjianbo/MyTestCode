#include "Clipboard.h"
#include <QGuiApplication>
#include <QClipboard>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QRegExp>
#include <QDebug>

void Clipboard::copyIp(QStringList ip)
{
    QStringList item_list;
    QString ip_text;
    if(ip.size()==4){
        //空文本直接作为0处理
        for(QString &item : ip)
            item_list.append(QString::number(item.toInt()));
        ip_text=item_list.join('.');
    }else if(ip.size()==1){
        ip_text=QString::number(ip.first().toInt());
    }
    if(!ip_text.isEmpty()){
        QClipboard *clip_board=QGuiApplication::clipboard();
        clip_board->setText(ip_text);
    }
    qDebug()<<"copy ip"<<ip<<ip_text;
}

QStringList Clipboard::pasteIp()
{
    QClipboard *clip_board=QGuiApplication::clipboard();
    QString ip_text=clip_board->text();
    //可以加上其他分隔符判断
    //可以用正则来判断数据合法性
    QStringList item_list=ip_text.split('.');
    QStringList ip;
    bool ret;
    int val;
    if(item_list.size()==4){
        /*for(QString item : item_list)
        {
            ret=true;
            if(item.size()>0&&item.size()<=3){
                for(QChar c : item)
                {
                    ret&=c.isDigit();
                    //可以直接在这里完成数字转换
                }
                if(ret){
                    val=item.toInt();
                    if(val>255||val<0){
                        ret=false;
                    }
                    if(val>255||val<0){
                        ret=false;
                    }
                }
            }else{
                ret=false;
            }
            if(!ret){
                ip.clear();
                break;
            }
            ip.append(QString::number(val));
        }*/
        //改为正则匹配
        if(checkIPv4(ip_text)){
            for(QString item : item_list)
                ip.append(QString::number(item.toInt()));
        }
    }else if(item_list.size()==1){
        //这个懒得写正则了
        QString item=item_list.first();
        ret=true;
        if(item.size()>0&&item.size()<=3){
            for(QChar c : item)
            {
                ret&=c.isDigit();
            }
            if(ret){
                val=item.toInt();
                if(val>=0&&val<=255){
                    ip.append(QString::number(val));
                }
            }
        }
    }
    qDebug()<<"paste ip"<<ip<<ip_text;
    return ip;
}

bool Clipboard::checkIPv4(const QString str)
{
    QString section="(\\d|[1-9]\\d|1\\d\\d|2[0-4]\\d|25[0-5])";
    QString exp=QString("^%1\\.%1\\.%1\\.%1$").arg(section);

    //Qt4
    //QRegExp reg1(exp);
    //qDebug()<<reg1.exactMatch(str);

    //Qt5
    QRegularExpression reg2(QRegularExpression::anchoredPattern(exp));
    QRegularExpressionMatch match=reg2.match(str);
    //qDebug()<<match.hasMatch();
    return match.hasMatch();
}
