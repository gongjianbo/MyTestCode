#include "MyHighlighter.h"

MyHighlighter::MyHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    //关键字高亮
    HighlightingRule keys_rule;
    keys_rule.pattern.setPattern("(def|function|return)");
    keys_rule.format.setForeground(QColor(0, 0, 255));
    keys_rule.format.setFontWeight(QFont::Bold);
    highlightingRules.append(keys_rule);

    //双引号范围高亮
    HighlightingRule str_rule;
    str_rule.pattern.setPattern("\".*\"");
    str_rule.format.setForeground(QColor(200, 100, 0));
    str_rule.format.setFontWeight(QFont::Bold);
    highlightingRules.append(str_rule);
}

void MyHighlighter::highlightBlock(const QString &text)
{
    //QRegularExpression
    for (const HighlightingRule &rule : qAsConst(highlightingRules)) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
    //QRegExp
    /*for (const HighlightingRule &rule : highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }*/
}
