#include "CppTool.h"
#include <QDebug>

int data_counter = 0;

CppData::CppData()
{
    data.resize(1024 * 1024 * 100);
    qDebug() << __FUNCSIG__ << ++data_counter;
}

CppData::CppData(const CppData &other)
{
    id = other.id;
    name = other.name;
    data.resize(1024 * 1024 * 100);
    qDebug() << __FUNCSIG__ << ++data_counter;
}

CppData::~CppData()
{
    qDebug() << __FUNCSIG__ << --data_counter;
}

CppData &CppData::operator=(const CppData &other)
{
    if (this == &other)
        return *this;
    id = other.id;
    name = other.name;
    data = other.data;
    qDebug() << __FUNCSIG__;
    return *this;
}

QString CppData::toString()
{
    return QString("CppData(id:%1, name:%2)").arg(id).arg(name);
}

CppTool::CppTool(QObject *parent)
    : QObject{parent}
{

}

CppTool::~CppTool()
{

}

CppTool *CppTool::getInstance()
{
    static CppTool instance;
    return &instance;
}

CppData CppTool::createData()
{
    return CppData{};
}

void CppTool::setData(const CppData &data)
{
    qDebug() << __FUNCSIG__ << data.id << data.name;
}

QVariant CppTool::createMeta()
{
    // QImage/QPixmap这种稀缺资源类型，QML不会持有其引用，生成之后可以立即使用但没法用变量暂存
    QImage data(1024 * 10, 1024 * 10, QImage::Format_Grayscale8);
    // QByteArray data(1024 * 1024 * 100, '\0');
    // QString data(1024 * 1024 * 100, '\0');
    // QVector<QImage> data(10, QImage(1024 * 10, 1024 * 10, QImage::Format_Grayscale8));
    QVariant var = QVariant::fromValue(data);
    qDebug() << __FUNCSIG__ << data.size() << var.userType();
    return var;
}

void CppTool::setMeta(const QVariant &var)
{
    QImage data = var.value<QImage>();
    qDebug() << __FUNCSIG__ << data.size() << var.userType();
}
