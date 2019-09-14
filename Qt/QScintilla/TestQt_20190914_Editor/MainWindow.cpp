#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //qsciscintilla.h文件还定义了各参数枚举
    //编辑器类
    QsciScintilla *editor=new QsciScintilla(this);

    //设置语法
    QsciLexerCSS *textLexer=new QsciLexerCSS;//创建一个词法分析器
    editor->setLexer(textLexer); //给QsciScintilla设置词法分析器
    textLexer->setPaper(QColor(200,250,200));//文本区域背景色
    textLexer->setColor(QColor(0,170,0),QsciLexerCSS::Comment); //设置自带的注释行为灰色
    textLexer->setColor(QColor(Qt::yellow),QsciLexerCSS::Tag);//标签
    textLexer->setColor(QColor(Qt::yellow),QsciLexerCSS::IDSelector);//id选择器

    //行号提示
    //editor->SendScintilla(QsciScintilla::SCI_SETCODEPAGE,QsciScintilla::SC_CP_UTF8);//设置编码为UTF-8
    QFont line_font;
    line_font.setFamily("SimSun");
    line_font.setPointSize(11);
    editor->setFont(line_font);//设置文本字体
    editor->setWrapMode(QsciScintilla::WrapWord); //文本自动换行模式
    //editor->setWrapVisualFlags(QsciScintilla::WrapFlagByText);

    editor->setEolMode(QsciScintilla::EolWindows); //微软风格换行符
    //editor->setEolVisibility(true);//显示换行符

    editor->setWhitespaceVisibility(QsciScintilla::WsVisible);//此时空格为点，\t为箭头
    editor->setWhitespaceSize(2);//空格点大小

    //indent
    editor->setIndentationsUseTabs(false);//false用表示用空格代替\t
    editor->setTabWidth(4);//\t宽度设为四个空格
    editor->setIndentationGuides(true);//用tab键缩进时，在缩进位置上显示一个竖点线，缩进有效，在字符串后加空格不显示
    editor->setIndentationWidth(0);//如果在行首部空格位置tab，缩进的宽度字符数，并且不会转换为空格
    editor->setAutoIndent(true);//换行后自动缩进
    editor->setBackspaceUnindents(true);
    editor->setTabIndents(true);//True如果行前空格数少于tabWidth，补齐空格数,False如果在文字前tab同true，如果在行首tab，则直接增加tabwidth个空格
    editor->setBraceMatching(QsciScintilla::SloppyBraceMatch);//括号匹配

    //current line color
    editor->setCaretWidth(2);//光标宽度，0表示不显示光标
    editor->setCaretForegroundColor(QColor("darkCyan"));  //光标颜色
    editor->setCaretLineVisible(true); //是否高亮显示光标所在行
    editor->setCaretLineBackgroundColor(QColor(100,250,100));//光标所在行背景颜色

    //selection color
    editor->setSelectionBackgroundColor(Qt::black);//选中文本背景色
    editor->setSelectionForegroundColor(Qt::white);//选中文本前景色

    //edge
    //editor->setEdgeColumn(50); //edge宽度，没发现有啥用
    //editor->setEdgeMode(QsciScintilla::EdgeLine);
    //editor->setEdgeColor(QColor("green"));

    QFont margin_font;
    margin_font.setFamily("SimSun");
    margin_font.setPointSize(11);//边栏字体设置px我这里显示不出行号，不知道是怎么回事
    editor->setMarginsFont(margin_font);//设置页边字体
    editor->setMarginType(0,QsciScintilla::NumberMargin);//设置标号为0的页边显示行号
    //editor->setMarginMarkerMask(0,QsciScintilla::Background);//页边掩码
    //editor->setMarginSensitivity(0,true);//注册通知事件，当用户点击边栏时，scintilla会通知我们
    editor->setMarginLineNumbers(0,true);//设置第0个边栏为行号边栏，True表示显示
    editor->setMarginWidth(0,15);//设置0边栏宽度
    editor->setMarginsBackgroundColor(Qt::gray);
    editor->setMarginsForegroundColor(Qt::white);

    editor->setFolding(QsciScintilla::BoxedTreeFoldStyle);//折叠样式
    editor->setFoldMarginColors(Qt::gray,Qt::lightGray);//折叠栏颜色


    //代码提示
    QsciAPIs *apis=new QsciAPIs(textLexer);
    apis->add(QString("gongjianbo"));
    apis->add(QString("Gong"));
    apis->prepare();

    //auto complete
    //Acs[None|All|Document|APIs]
    //禁用自动补全提示功能|所有可用的资源|当前文档中出现的名称都自动补全提示|使用QsciAPIs类加入的名称都自动补全提示
    editor->setAutoCompletionSource(QsciScintilla::AcsAll);//自动补全。对于所有Ascii字符
    editor->setAutoCompletionCaseSensitivity(true);//大小写敏感度，设置lexer可能会更改，不过貌似没啥效果
    editor->setAutoCompletionThreshold(1);//设置每输入一个字符就会出现自动补全的提示
    //editor->setAutoCompletionReplaceWord(false);//是否用补全的字符串替代光标右边的字符串

    setCentralWidget(editor);
}

MainWindow::~MainWindow()
{
    delete ui;
}

