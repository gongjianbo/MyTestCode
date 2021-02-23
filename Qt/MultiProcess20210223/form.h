#ifndef FORM_H
#define FORM_H

#include <QWidget>

#include "ProcessManager.h"
#include "ProcessWorker.h"

namespace Ui {
class Form;
}

class Form : public QWidget
{
    Q_OBJECT

public:
    explicit Form(QWidget *parent = nullptr);
    ~Form();

private:
    Ui::Form *ui;
};

#endif // FORM_H
