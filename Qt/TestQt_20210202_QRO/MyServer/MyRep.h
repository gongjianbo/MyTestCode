#ifndef MYREP_H
#define MYREP_H

#include <QObject>
#include "rep_MyRep_source.h"

class MyRep : public MyRepSource
{
    Q_OBJECT
public:
    explicit MyRep(QObject *parent = nullptr);

public slots:
    void myRecv(const QString &data, int num) override;

};

#endif // MYREP_H
