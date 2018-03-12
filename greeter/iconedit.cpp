/* iconedit.cpp
 * Copyright (C) 2018 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301, USA.
**/
#include "iconedit.h"
#include <QHBoxLayout>
#include <QWidget>
#include <QPainter>
#include <QFile>
#include <QKeyEvent>
#include <QDebug>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>

///////////////////////////// TipEdit的成员 ////////////////////////////////////////
TipEdit::TipEdit(QWidget *parent)
    :QLineEdit(parent)
{

}

void TipEdit::paintEvent(QPaintEvent *event)
{
    QLineEdit::paintEvent(event);
    if(text().length() == 0 && m_tip.length() > 0){
        drawTip();
    }
}

void TipEdit::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_CapsLock) {
        checkCapsState();
    }
    return QLineEdit::keyReleaseEvent(event);
}

void TipEdit::focusInEvent(QFocusEvent *event)
{
    checkCapsState();
    return QLineEdit::focusInEvent(event);
}

/**
 * @brief TipEdit::drawTip
 * 绘制提示（和placeholder不同）
 */
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
/**
 * @brief TipEdit::checkCapsState
 * 监测大写键开关状态，并发送信号
 */
void TipEdit::checkCapsState()
{
    //判断大写键状态
    Display *display = XOpenDisplay(NULL);
    bool capsState = false;
    if(display) {
        unsigned int n;
        XkbGetIndicatorState(display, XkbUseCoreKbd, &n);
        capsState = (n & 0x01) == 1;
    }
    Q_EMIT capsStateChanged(capsState);
}


//////////////////////////// IconEdit的成员 ////////////////////////////////////////


IconEdit::IconEdit(QWidget *parent)
    :QWidget(parent)
{
    m_edit = new TipEdit(this);
    m_edit->setObjectName(QStringLiteral("passwdEdit"));
    m_edit->setAttribute(Qt::WA_InputMethodEnabled, false); //禁用输入法
    m_edit->setTextMargins(1, 1, height(), 1);   // 设置输入框中文件输入区，不让输入的文字在被隐藏在按钮下

    m_capsIcon = new QLabel(this);
    m_capsIcon->setObjectName(QStringLiteral("capsIcon"));
    m_capsIcon->setFixedSize(height(), height());
    m_capsIcon->hide();

    m_iconButton = new QPushButton(this);
    m_iconButton->setObjectName(QStringLiteral("loginButton"));
    m_iconButton->setFocusPolicy(Qt::NoFocus);
    m_iconButton->setFixedSize(height(), height());
    m_iconButton->setIcon(QIcon(":resource/arrow_right.png"));
    m_iconButton->setIconSize(QSize(height(), height()));
    m_iconButton->setCursor(QCursor(Qt::PointingHandCursor));
    m_iconButton->hide();

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(1, 1, 1, 1);
    layout->setSpacing(0);
    layout->addStretch();
    layout->addWidget(m_capsIcon);
    layout->addWidget(m_iconButton);

    connect(m_edit, &TipEdit::textChanged, this, &IconEdit::showIcon);
    connect(m_edit, &TipEdit::capsStateChanged, m_capsIcon, &QLabel::setVisible);
    connect(m_iconButton, &QPushButton::clicked, this, &IconEdit::clicked_cb);
    setFocusProxy(m_edit);
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
    {
        return clicked_cb();
    }
    return QWidget::keyReleaseEvent(event);
}

void IconEdit::clicked_cb()
{
    emit clicked(m_edit->text());
}
void IconEdit::showIcon(const QString &text)
{
    if(text.isEmpty()){
        m_iconButton->hide();
    } else {
        m_iconButton->show();
    }
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
    setPrompt("");
}

void IconEdit::setPrompt(const QString &prompt)
{
    m_edit->setInnerTip(prompt);
    update();   //不更新的话，第一次不会显示prompt
}

const QString IconEdit::text()
{
    return m_edit->text();
}

void IconEdit::setWaiting(bool wait)
{
    m_edit->setReadOnly(wait);
}
