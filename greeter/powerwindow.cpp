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
#include <QPushButton>
#include <QPainter>
#include <QApplication>
#include <QDesktopWidget>
#include <QMouseEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QImage>
#include <QDebug>
#include <QException>
#include "globalv.h"

PowerWindow::PowerWindow(bool hasOpenSessions, QWidget *parent)
    : QWidget(parent),
      m_hasOpenSessions(hasOpenSessions),
      m_power(new QLightDM::PowerInterface(this))
{
    initUI();
    QDesktopWidget *dw = QApplication::desktop();
    setGeometry((dw->width()-width())/2, (dw->height()-height())/2,
                width(), height());
}

void PowerWindow::initUI()
{
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground, true);

    m_centerWidget = new QWidget(this);
    m_centerWidget->setObjectName(QStringLiteral("powerCenterWidget"));

    setFixedHeight(290);
    QString text = tr("Goodbye. Would you like to…");
    if(m_hasOpenSessions) {
        QString text2 = tr("Other users are currently logged in to this computer, "
                   "shutting down now will also close these other sessions.");
        text = QString("%1\n\n%2").arg(text2).arg(text);
        //中文只有一行，英文有两行
        if(language == QLocale::Chinese)
            this->setFixedHeight(330);
        else
            this->setFixedHeight(350);
    }
    //重启和关机一定存在，根据是否能挂起和休眠确定窗口宽度
    int cnt = 0;
    if(m_power->canHibernate())
        cnt++;
    if(m_power->canSuspend())
        cnt++;
    this->setFixedWidth(455 + 188 * cnt);
    m_centerWidget->setGeometry(QRect(24, 24, width()-24*2, height()-24*2));

    QVBoxLayout *vbox = new QVBoxLayout(m_centerWidget);
    vbox->setContentsMargins(20, 10, 20, 2);

//    QPalette plt;
//    plt.setColor(QPalette::WindowText, Qt::white);

    m_prompt = new QLabel(m_centerWidget);
    m_prompt->adjustSize();
    m_prompt->setText(text);
    m_prompt->setWordWrap(true);
    m_prompt->setAlignment(Qt::AlignLeft | Qt::AlignTop);
//    m_prompt->setPalette(plt);
    m_prompt->setFont(QFont("Ubuntu Light 10"));

    vbox->addWidget(m_prompt);

    QHBoxLayout *hbox = new QHBoxLayout();
    hbox->setSpacing(20);

    if(m_power->canSuspend()){
        QVBoxLayout *vboxSuspend = new QVBoxLayout();
        m_suspend = new QLabel(m_centerWidget);
        m_suspend->setFixedSize(168, 168);
        m_suspend->setObjectName(QStringLiteral("suspend"));
//        m_suspend->setStyleSheet("QLabel{background:url(:/resource/suspend.png); background-repeat:no-repeat}"
//                                 "QLabel::hover{background:url(:/resource/suspend_highlight.png); background-repeat:no-repeat}");
        m_suspend->installEventFilter(this);
        m_suspendLabel = new QLabel(m_centerWidget);
        m_suspendLabel->setAlignment(Qt::AlignCenter);
        m_suspendLabel->setFixedSize(168, 30);
//        m_suspendLabel->setPalette(plt);

        vboxSuspend->addWidget(m_suspend);
        vboxSuspend->addWidget(m_suspendLabel);

        hbox->addLayout(vboxSuspend);
    }
    if(m_power->canHibernate()) {
        QVBoxLayout *vboxHibernate = new QVBoxLayout();
        m_hibernate = new QLabel(m_centerWidget);
        m_hibernate->setFixedSize(168, 168);
        m_hibernate->setObjectName(QStringLiteral("hibernate"));
//        m_hibernate->setStyleSheet("QLabel{background:url(:/resource/hibernate.png); background-repeat:no-repeat}"
//                                 "QLabel::hover{background:url(:/resource/hibernate_highlight.png); background-repeat:no-repeat}");
        m_hibernate->installEventFilter(this);
        m_hibernateLabel = new QLabel(m_centerWidget);
        m_hibernateLabel->setAlignment(Qt::AlignCenter);
        m_hibernateLabel->setFixedSize(168, 30);
//        m_hibernateLabel->setPalette(plt);

        vboxHibernate->addWidget(m_hibernate);
        vboxHibernate->addWidget(m_hibernateLabel);

        hbox->addLayout(vboxHibernate);
    }

    QVBoxLayout *vboxStart = new QVBoxLayout();
    m_restart = new QLabel(m_centerWidget);
    m_restart->setFixedSize(168, 168);
    m_restart->setObjectName(QStringLiteral("restart"));
//    m_restart->setStyleSheet("QLabel{background:url(:/resource/restart.png); background-repeat:no-repeat}"
//                             "QLabel::hover{background:url(:/resource/restart_highlight.png); background-repeat:no-repeat}");
    m_restart->installEventFilter(this);
    m_restartLabel = new QLabel(m_centerWidget);
    m_restartLabel->setAlignment(Qt::AlignCenter);
    m_restartLabel->setFixedSize(168, 30);
//    m_restartLabel->setPalette(plt);
    vboxStart->addWidget(m_restart);
    vboxStart->addWidget(m_restartLabel);

    QVBoxLayout *vboxShutdown = new QVBoxLayout();
    m_shutdown = new QLabel(m_centerWidget);
    m_shutdown->setFixedSize(168, 168);
    m_shutdown->setObjectName(QStringLiteral("shutdown"));
//    m_shutdown->setStyleSheet("QLabel{background:url(:/resource/shutdown.png); background-repeat:no-repeat}"
//                              "QLabel::hover{background:url(:/resource/shutdown_highlight.png); background-repeat:no-repeat}");
    m_shutdown->installEventFilter(this);
    m_shutdownLabel = new QLabel(m_centerWidget);
    m_shutdownLabel->setAlignment(Qt::AlignCenter);
    m_shutdownLabel->setFixedSize(168, 30);
//    m_shutdownLabel->setPalette(plt);
    vboxShutdown->addWidget(m_shutdown);
    vboxShutdown->addWidget(m_shutdownLabel);

    hbox->addLayout(vboxStart);
    hbox->addLayout(vboxShutdown);

    vbox->addLayout(hbox);
    vbox->addStretch();

    m_close = new QPushButton(this);
    m_close->setObjectName(QStringLiteral("close"));
    m_close->setGeometry(QRect(0, 0, 24, 24));
    connect(m_close, &QPushButton::clicked, this, &PowerWindow::close);
//    m_close->setStyleSheet("QLabel{background:url(:/resource/dialog_close.png); background-repeat:no-repeat}"
//                           "QLabel::hover{background:url(:/resource/dialog_close_highlight.png); background-repeat:no-repeat}");
//    m_close->installEventFilter(this);
}

void PowerWindow::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.setPen(Qt::transparent);
    painter.setBrush(QColor(255, 255, 255, 20));
    painter.drawRect(rect());

//    QRect center = QRect(24, 24, width()-24 * 2, height()-24*2);
//    painter.setBrush(QColor(46, 39, 101, 250));
//    painter.drawRect(center);
    QWidget::paintEvent(e);
}

void PowerWindow::closeEvent(QCloseEvent *e)
{
    emit aboutToClose();
    QWidget::closeEvent(e);
}

bool PowerWindow::eventFilter(QObject *obj, QEvent *event)
{
    /*if(obj == m_close) {
        if(event->type() == QEvent::MouseButtonPress) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if(mouseEvent->button() == Qt::LeftButton) {
                m_close->setStyleSheet("QLabel{background:url(:/resource/dialog_close_press.png)}");
                return true;
            }
        }
        if(event->type() == QEvent::MouseButtonRelease) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if(mouseEvent->button() == Qt::LeftButton) {
                m_close->setStyleSheet("QLabel{background:url(:/resource/dialog_close.png)}"
                                       "QLabel::hover{background:url(:/resource/dialog_close_highlight.png)}");
                this->close();
                return true;
            }
        }
    } else*/ if(obj == m_suspend) {
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

            }
        }
    }
    return QWidget::eventFilter(obj, event);
}
