#include "iconedit.h"
#include <QHBoxLayout>
#include <QWidget>
#include <QPainter>
#include <QFile>
#include <QKeyEvent>
#include <QDebug>


//////////////////////////// IconButton的成员 ////////////////////////////////////////

IconButton::IconButton(QLineEdit *edit)
    :QPushButton(edit)
{
    m_size = QSize(edit->height(), edit->height());

    this->setMinimumSize(m_size);
    this->setMaximumSize(m_size);
    this->setFocusPolicy(Qt::NoFocus);  //得到焦点时不显示虚线框
    this->setFlat(true);
    this->setCursor(QCursor(Qt::PointingHandCursor));
    this->setIconSize(m_size);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins(1, 1, 1, 1);
    buttonLayout->addStretch();
    buttonLayout->addWidget(this);
    buttonLayout->setDirection(QBoxLayout::LeftToRight);
    edit->setLayout(buttonLayout);

    // 设置输入框中文件输入区，不让输入的文字在被隐藏在按钮下
    edit->setTextMargins(1, 1, m_size.width() , 1);
    edit->setStyleSheet(" QLineEdit { border: 1px solid #026096 ; lineedit-password-character:8226}");  //设置密码显示为中等大小的实心圆点
    this->setStyleSheet("QPushButton{background:transparent; border:0px}");
}
IconButton::IconButton(QLineEdit *edit, const QIcon &icon)
    :IconButton(edit)
{
    this->setIcon(icon);
}
void IconButton::resize(const QSize& size)
{
    this->setMinimumSize(size);
    this->setMaximumSize(size);
}

///////////////////////////// TipEdit的成员 ////////////////////////////////////////
TipEdit::TipEdit(QWidget *parent)
    :QLineEdit(parent)
{

}

void TipEdit::paintEvent(QPaintEvent *event)
{
    QLineEdit::paintEvent(event);
    if(text().length() == 0 && m_tip.length() > 0)
        drawTip();
}


void TipEdit::drawTip()
{

    QRect csRect = cursorRect();
    QRect tipRect(csRect.right()-3, rect().top(), rect().right(), rect().bottom());

    QPainter painter(this);
    painter.setPen(QColor("#888"));
    painter.setFont(QFont("ubuntu", 10));
    QTextOption option(Qt::AlignLeft | Qt::AlignVCenter);
    option.setWrapMode(QTextOption::WordWrap);

    painter.drawText(tipRect, m_tip, option);
}


//////////////////////////// IconEdit的成员 ////////////////////////////////////////


IconEdit::IconEdit(QWidget *parent)
    :QWidget(parent)
{
    m_edit = new TipEdit(this);
    m_iconButton = new IconButton(m_edit);
    connect(m_iconButton, SIGNAL(clicked(bool)), this, SLOT(clicked_cb()));
}

IconEdit::IconEdit(const QIcon& icon, QWidget *parent)
    :IconEdit(parent)
{
    m_iconButton->setIcon(icon);
    m_edit->setEchoMode(QLineEdit::Password);
}
void IconEdit::setType(QLineEdit::EchoMode type)
{
    m_edit->setEchoMode(type);
}

void IconEdit::setText(const QString &text)
{
    if(m_edit->echoMode() == QLineEdit::Normal){
        m_edit->setText(text);
    }
}

void IconEdit::keyReleaseEvent ( QKeyEvent * event )
{
    if(event->key() == Qt::Key_Return)
        return clicked_cb();
    return QWidget::keyReleaseEvent(event);
}

void IconEdit::clicked_cb()
{
    emit clicked(m_edit->text());
}

void IconEdit::setIcon(const QString &filename)
{
    m_iconButton->setIcon(QIcon(filename));
}

void IconEdit::setIcon(const QIcon &icon)
{
    m_iconButton->setIcon(icon);
}

void IconEdit::resize(const QSize& size)
{
    m_edit->resize(size);
    m_iconButton->resize(QSize(size.height(), size.height()));
}

void IconEdit::clear()
{
    m_edit->setText("");
}

void IconEdit::setPrompt(const QString &prompt)
{
    m_edit->setInnerTip(prompt);
}

const QString& IconEdit::text()
{
    return m_edit->text();
}

