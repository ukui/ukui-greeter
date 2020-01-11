/* powerwindow.cpp
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
#include "powerwindow.h"
#include <QLabel>
#include <QMouseEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>
#include <QException>
#include <QFontMetrics>
#include <qmath.h>
#include "globalv.h"

PowerWindow::PowerWindow(bool hasOpenSessions, QWidget *parent)
    : FakeDialog(parent),
      m_hasOpenSessions(hasOpenSessions),
      m_power(new QLightDM::PowerInterface(this))
{
    initUI();
}

void PowerWindow::initUI()
{
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setGeometry(parentWidget()->rect());

    //重启和关机一定存在，根据是否能挂起和休眠确定窗口宽度
    int cnt = 0;
    if(m_power->canHibernate())
        cnt++;
    if(m_power->canSuspend())
        cnt++;

    setDialogSize(455 + 188 * cnt, 290);
    //根据提示内容的长度确定窗口的高度
    QFont font("ubuntu", 12);
    QString text = tr("Goodbye. Would you like to...");
    int lineNum = 1;
    if(m_hasOpenSessions) {
        QString text2 = tr("Other users are currently logged in to this computer, "
                   "shutting down now will also close these other sessions.");
        text = QString("%1\n\n%2").arg(text2).arg(text);
        QFontMetrics fm(font);
        int textWide = fm.width(text2);
        lineNum = qCeil(textWide * 1.0 / centerWidget()->width()) + 1 + lineNum;
    }
    setDialogSize(455 + 188 * cnt, 280 + 20*lineNum);

    QVBoxLayout *vbox = new QVBoxLayout(centerWidget());
    vbox->setContentsMargins(20, 10, 20, 2);
    vbox->setSpacing(20);

    m_prompt = new QLabel(centerWidget());
    m_prompt->adjustSize();
    m_prompt->setText(text);
    m_prompt->setWordWrap(true);
    m_prompt->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    m_prompt->setFont(font);

    vbox->addWidget(m_prompt);

    QHBoxLayout *hbox = new QHBoxLayout();
    hbox->setSpacing(20);

    /* 挂起 */
    if(m_power->canSuspend()){
        QVBoxLayout *vboxSuspend = new QVBoxLayout();
        m_suspend = new QLabel(centerWidget());
        m_suspend->setFixedSize(168, 168);
        m_suspend->setObjectName(QStringLiteral("suspend"));
        m_suspend->installEventFilter(this);

        m_suspendLabel = new QLabel(centerWidget());
        m_suspendLabel->setAlignment(Qt::AlignCenter);
        m_suspendLabel->setFixedSize(168, 30);

        vboxSuspend->addWidget(m_suspend);
        vboxSuspend->addWidget(m_suspendLabel);

        hbox->addLayout(vboxSuspend);
    }
    /* 休眠 */
    if(m_power->canHibernate()) {
        QVBoxLayout *vboxHibernate = new QVBoxLayout();
        m_hibernate = new QLabel(centerWidget());
        m_hibernate->setFixedSize(168, 168);
        m_hibernate->setObjectName(QStringLiteral("hibernate"));
        m_hibernate->installEventFilter(this);

        m_hibernateLabel = new QLabel(centerWidget());
        m_hibernateLabel->setAlignment(Qt::AlignCenter);
        m_hibernateLabel->setFixedSize(168, 30);

        vboxHibernate->addWidget(m_hibernate);
        vboxHibernate->addWidget(m_hibernateLabel);

        hbox->addLayout(vboxHibernate);
    }

    /* 重启 */
    QVBoxLayout *vboxStart = new QVBoxLayout();
    m_restart = new QLabel(centerWidget());
    m_restart->setFixedSize(168, 168);
    m_restart->setObjectName(QStringLiteral("restart"));
    m_restart->installEventFilter(this);

    m_restartLabel = new QLabel(centerWidget());
    m_restartLabel->setAlignment(Qt::AlignCenter);
    m_restartLabel->setFixedSize(168, 30);

    vboxStart->addWidget(m_restart);
    vboxStart->addWidget(m_restartLabel);

    /* 关机 */
    QVBoxLayout *vboxShutdown = new QVBoxLayout();
    m_shutdown = new QLabel(centerWidget());
    m_shutdown->setFixedSize(168, 168);
    m_shutdown->setObjectName(QStringLiteral("shutdown"));
    m_shutdown->installEventFilter(this);

    m_shutdownLabel = new QLabel(centerWidget());
    m_shutdownLabel->setAlignment(Qt::AlignCenter);
    m_shutdownLabel->setFixedSize(168, 30);

    vboxShutdown->addWidget(m_shutdown);
    vboxShutdown->addWidget(m_shutdownLabel);

    hbox->addLayout(vboxStart);
    hbox->addLayout(vboxShutdown);

    vbox->addLayout(hbox);
    vbox->addStretch();
}

bool PowerWindow::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == m_suspend) {
        if(event->type() == QEvent::Enter) {
            m_suspendLabel->setText(tr("suspend"));
        } else if(event->type() == QEvent::Leave) {
            m_suspendLabel->setText("");
        } else if(event->type() == QEvent::MouseButtonRelease){
            qDebug() << "suspend";
            try{
                m_power->suspend();
                close();
            }catch(QException &e) {
                qWarning() << e.what();
            }
        }
    }else if(obj == m_hibernate) {
        if(event->type() == QEvent::Enter) {
            m_hibernateLabel->setText(tr("hibernate"));
        } else if(event->type() == QEvent::Leave) {
            m_hibernateLabel->setText("");
        } else if(event->type() == QEvent::MouseButtonRelease){
            qDebug() << "hibernate";
            try{
                m_power->hibernate();
                close();
            }catch(QException &e) {
                qWarning() << e.what();
            }
        }
    } else if(obj == m_restart) {
        if(event->type() == QEvent::Enter) {
            m_restartLabel->setText(tr("restart"));
        }else if(event->type() == QEvent::Leave) {
            m_restartLabel->setText("");
        }else if(event->type() == QEvent::MouseButtonRelease){
            qDebug() << "restart";
            try{
                m_power->restart();
                close();
            }catch(QException &e) {
                qWarning() << e.what();
            }
        }
    } else if(obj == m_shutdown) {
        if(event->type() == QEvent::Enter) {
            m_shutdownLabel->setText(tr("shutdown"));
        }else if(event->type() == QEvent::Leave) {
            m_shutdownLabel->setText("");
        }else if(event->type() == QEvent::MouseButtonRelease){
            try{
                qDebug() << "shutdown";
                m_power->shutdown();
                close();
            }catch(QException &e) {
                qWarning() << e.what();
            }
        }
    }
    return QWidget::eventFilter(obj, event);
}

void PowerWindow::mousePressEvent(QMouseEvent *event)
{
    if(!dialog()->geometry().contains(event->pos()))
        close();
}
