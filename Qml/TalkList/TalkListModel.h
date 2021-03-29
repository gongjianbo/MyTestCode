#ifndef TALKLISTMODEL_H
#define TALKLISTMODEL_H

#include <QAbstractListModel>
#include "TalkListDefine.h"

//聊天框ListView的model
//龚建波 2021-3-29
class TalkListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit TalkListModel(QObject *parent = nullptr);

    //data
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    //清空数据
    Q_INVOKABLE void clearModel();
    //测试发送文本
    Q_INVOKABLE void appendText(const QString &user,
                                const QString &sender,
                                const QString &text);
    //测试发送语音
    Q_INVOKABLE void appendAudio(const QString &user,
                                const QString &sender);

    //解析，如语音转文字，文档转换等
    Q_INVOKABLE void parseRow(int row);

private:
    bool isVaidRow(int row) const;

private:
    //会话数据
    QList<QSharedPointer<TalkDataBasic>> talkList;
};

#endif // TALKLISTMODEL_H
