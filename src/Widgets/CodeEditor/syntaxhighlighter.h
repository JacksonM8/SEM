#ifndef SYNTAXHIGHLIGHTER_H
#define SYNTAXHIGHLIGHTER_H

#include <QSyntaxHighlighter>

#include <QHash>
#include <QTextCharFormat>

class QString;

class QTextDocument;


struct ParenthesisInfo
{
    char character;
    int position;
    int actualPosition;
    bool isOpening() const;
    bool isClosing() const;
    char getMatching() const;
};

class TextBlockData : public QTextBlockUserData
{
public:
    TextBlockData();

    QVector<ParenthesisInfo *> parentheses();
    void insert(ParenthesisInfo *info);

private:
    QVector<ParenthesisInfo *> m_parentheses;
};



class SyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    SyntaxHighlighter(QTextDocument *parent = 0);

protected:
    void highlightBlock(const QString &text);
    void highlightParenthesis(const QString &text);\


private:
    void themeChanged();
    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };

    QVector<HighlightingRule> highlightingRules;

    QRegExp commentStartExpression;
    QRegExp commentEndExpression;

    QTextCharFormat keywordFormat;
    QTextCharFormat classFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat multiLineCommentFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat functionFormat;
};

#endif // SYNTAXHIGHLIGHTER_H
