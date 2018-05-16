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
#include "common/keyeventmonitor.h"


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
void TipEdit::keyPressEvent(QKeyEvent *event)
{
    /* 屏蔽掉按下Super键时的按键 */
    if(event->modifiers() == Qt::META)
        return;
    return QLineEdit::keyPressEvent(event);
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


//////////////////////////// IconEdit的成员 ////////////////////////////////////////


IconEdit::IconEdit(QWidget *parent)
    : QWidget(parent),
      keyMonitor(KeyEventMonitor::instance(this))
{
    keyMonitor->start();

    m_edit = new TipEdit(this);
    m_edit->setObjectName(QStringLiteral("passwdEdit"));
    m_edit->setAttribute(Qt::WA_InputMethodEnabled, false); //禁用输入法
    m_edit->setContextMenuPolicy(Qt::NoContextMenu);    //禁用右键菜单

    m_capsIcon = new QLabel(this);
    m_capsIcon->setObjectName(QStringLiteral("capsIcon"));
    m_capsIcon->hide();

    m_iconButton = new QPushButton(this);
    m_iconButton->setObjectName(QStringLiteral("loginButton"));
    m_iconButton->setFocusPolicy(Qt::NoFocus);
    m_iconButton->setIcon(QIcon(":resource/arrow_right.png"));
    m_iconButton->setCursor(QCursor(Qt::PointingHandCursor));
    m_iconButton->hide();

    m_modeButton = new QPushButton(this);
    m_modeButton->setObjectName(QStringLiteral("modeButton"));
    m_modeButton->setFocusPolicy(Qt::NoFocus);
    m_modeButton->setCursor(QCursor(Qt::ArrowCursor));
    m_modeButton->setStyleSheet("QPushButton{background:transparent;}");
    connect(m_modeButton, &QPushButton::clicked, this, [&]{
        if(m_edit->echoMode() == QLineEdit::Password) {
            setType(QLineEdit::Normal);
        } else {
            setType(QLineEdit::Password);
        }
    });

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(1, 1, 1, 1);
    layout->setSpacing(0);
    layout->addStretch();
    layout->addWidget(m_capsIcon);
    layout->addWidget(m_modeButton);
    layout->addWidget(m_iconButton);

    connect(m_edit, &TipEdit::textChanged, this, &IconEdit::showIconButton);
    connect(m_edit, &TipEdit::returnPressed, this, &IconEdit::clicked_cb);
    connect(m_iconButton, &QPushButton::clicked, this, &IconEdit::clicked_cb);
    connect(keyMonitor, &KeyEventMonitor::CapsLockChanged, this, &IconEdit::onCapsStateChanged);

    setFocusProxy(m_edit);
}

void IconEdit::setType(QLineEdit::EchoMode type)
{
    m_edit->setEchoMode(type);
    if(type == QLineEdit::Password)
        m_modeButton->setIcon(QIcon(":/resource/hide-password.png"));
    else
        m_modeButton->setIcon(QIcon(":/resource/show-password.png"));
}

void IconEdit::setText(const QString &text)
{
    if(m_edit->echoMode() == QLineEdit::Normal){
        m_edit->setText(text);
    }
}

void IconEdit::resizeEvent(QResizeEvent *)
{
    m_edit->setTextMargins(1, 1, height(), 1);   // 设置输入框中文件输入区，不让输入的文字在被隐藏在按钮下
    m_capsIcon->setFixedSize(height(), height());
    m_iconButton->setFixedSize(height(), height());
    m_iconButton->setIconSize(QSize(height(), height()));
}

void IconEdit::clicked_cb()
{
    m_iconButton->setFocus();       //按回车后输入框光标会消失或者不再闪烁，先让其他控件获取焦点，就会解决该问题
    emit clicked(m_edit->text());
}

void IconEdit::showIconButton(const QString &text)
{
    m_iconButton->setVisible(!text.isEmpty());
}

void IconEdit::onCapsStateChanged(int capsState)
{
    m_capsIcon->setVisible(capsState);
    m_edit->setTextMargins(1, 1, capsState ? height() * 2 : height(), 1);
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
