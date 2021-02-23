#include "form.h"
#include "ui_form.h"

#include <QFileDialog>
#include <QDebug>

Form::Form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);

    connect(ui->btnPath,&QPushButton::clicked,this,[=]{
        const QString exe_path=QFileDialog::getOpenFileName(this);
        ui->editPath->setText(exe_path);
    });
    connect(ui->btnStart,&QPushButton::clicked,this,[=]{
        const QString exe_path=ui->editPath->text();
        if(exe_path.isEmpty())
            return;
        const QString exe_key=ui->editKey->text();
        const QString exe_arg=ui->editArg->text();
        const QStringList arg_list=exe_arg.isEmpty()?QStringList():QStringList()<<exe_arg;
        ProcessManager::getInstance()->startProcess(exe_path,exe_key,arg_list);
    });
    connect(ui->btnStop,&QPushButton::clicked,this,[=]{
        const QString exe_path=ui->editPath->text();
        if(exe_path.isEmpty())
            return;
        const QString exe_key=ui->editKey->text();
        ProcessManager::getInstance()->stopProcess(exe_path,exe_key);
    });
}

Form::~Form()
{
    delete ui;
}
