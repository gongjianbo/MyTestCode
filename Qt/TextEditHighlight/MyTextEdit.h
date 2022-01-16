#pragma once
#include <QTextEdit>

class MyTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    explicit MyTextEdit(QWidget *parent = nullptr);

public slots:
    void updateHighlight();
};
