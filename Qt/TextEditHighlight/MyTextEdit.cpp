#include "MyTextEdit.h"
#include "MyHighlighter.h"
#include <QTextBlock>
#include <QPalette>

MyTextEdit::MyTextEdit(QWidget *parent)
    : QTextEdit(parent)
{
    //背景和选区颜色
    QPalette pt = palette();
    pt.setBrush(QPalette::Text, Qt::white);
    pt.setBrush(QPalette::Base, Qt::black);
    pt.setBrush(QPalette::Highlight, Qt::gray);
    pt.setBrush(QPalette::HighlightedText, Qt::NoBrush);
    setPalette(pt);
    //qss貌似没有NoBrush对应的设置
    //setStyleSheet("QTextEdit{color:white;background-color:black;"
    //              "selection-color:white;selection-background-color:gray;}");

    //语法高亮
    MyHighlighter *keys_lighter = new MyHighlighter(this->document());
    Q_UNUSED(keys_lighter)

    //当前行颜色设置
    connect(this, &MyTextEdit::cursorPositionChanged, this, &MyTextEdit::updateHighlight);
    updateHighlight();

    //代码选中指定位置
    setPlainText("def function(){\n    print(\"hello qt!\")\n}");
    QString key = "hello";
    /*
    QString text = this->toPlainText();
    int pos = text.indexOf(key);
    if (pos >= 0) {
        QTextCursor cursor = this->textCursor();
        cursor.setPosition(pos, QTextCursor::MoveAnchor); //移到key起始位置
        cursor.movePosition(QTextCursor::NoMove, QTextCursor::KeepAnchor, key.length());
        cursor.select(QTextCursor::WordUnderCursor);
        setTextCursor(cursor);
    }*/
    QTextCursor cursor = this->textCursor();
    QTextDocument *doc = this->document();
    //find有重载，可以指定查找开始的位置
    QTextCursor ret = doc->find(key, cursor, QTextDocument::FindWholeWords);
    if (!ret.isNull()) {
        setTextCursor(ret);
    }
}

void MyTextEdit::updateHighlight()
{
    QList<QTextEdit::ExtraSelection> extra_selections;

    //当前行背景样式
    QTextEdit::ExtraSelection line;
    line.format.setBackground(QColor(50, 50, 50));
    line.format.setProperty(QTextFormat::FullWidthSelection, true);
    line.cursor = this->textCursor();
    line.cursor.clearSelection();
    extra_selections.append(line);

    this->setExtraSelections(extra_selections);
}
