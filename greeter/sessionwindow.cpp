/* sessionwindow.cpp
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
#include "sessionwindow.h"
#include <QLabel>
#include <QPushButton>
#include <QMouseEvent>
#include <QPainter>
#include <QDebug>
#include <QListWidget>
#include <QLightDM/SessionsModel>
#include "globalv.h"

class IconLabel : public QWidget
{
public:
    IconLabel(int width, int height, QWidget* parent = 0)
        : QWidget(parent),
          m_iconLabel(new QLabel(this)),
          m_textLabel(new QLabel(this))
    {
        m_iconLabel->setGeometry(3, (height-22)/2, 22, 22);
        m_textLabel->setGeometry(height+5, 0, width-height-5, height);
//        m_textLabel->setStyleSheet("QLabel{color: white}");
    }

    void setIcon(const QString& icon)
    {
        m_iconLabel->setPixmap(QPixmap(icon));
    }

    void setText(const QString& text)
    {
        m_textLabel->setText(text);
        m_textLabel->setFont(QFont("Ubutnu", 14));
        m_textLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    }

private:
    QLabel  *m_iconLabel;
    QLabel  *m_textLabel;
};



SessionWindow::SessionWindow(const QString& session, QWidget *parent)
    : QWidget(parent),
      m_defaultSession(session),
      m_sessionsModel(new QLightDM::SessionsModel(QLightDM::SessionsModel::LocalSessions))

{
    for(int i = 0; i < m_sessionsModel->rowCount(); i++){
        qDebug() << "session " << m_sessionsModel->index(i, 0).data(Qt::UserRole).toString() << " added";
    }
    initUI();
}

void SessionWindow::initUI()
{
    if (objectName().isEmpty())
        setObjectName(QStringLiteral("SessionWnd"));
    int sessionNum = m_sessionsModel->rowCount();
    int height = 55 + 40 * sessionNum + 20 * (sessionNum - 1);
    resize(550, height);
//    QPalette plt;
//    plt.setBrush(QPalette::Window, QBrush("#142D6F"));
//    plt.setColor(QPalette::WindowText, Qt::white);
//    setPalette(plt);

    m_backLabel = new QPushButton(this);
    m_backLabel->setObjectName(QStringLiteral("backButton"));
    m_backLabel->setGeometry(QRect(0, 0, 32, 32));
    m_backLabel->setShortcut(Qt::Key_Escape);
    connect(m_backLabel, &QPushButton::clicked, this, &SessionWindow::back);

    m_prompt = new QLabel(this);
    m_prompt->setObjectName(QStringLiteral("m_prompt"));
    m_prompt->setGeometry(QRect(220, 0, 330, 30));
    m_prompt->setFont(QFont("Ubuntu", 16));
    m_prompt->setText(tr("select the desktop environment"));

    m_sessionsList = new QListWidget(this);
    m_sessionsList->setObjectName(QStringLiteral("sessionsList"));
//    m_sessionsList->setStyleSheet("QListWidget{background:transparent;}"
//                                  "QListWidget::item{height:40px}"
//                                  "QListWidget::item:hover{background:rgb(150, 150, 150, 50); border:1px solid white; border-radius:4px;}"
//                                  "QListWidget::item:selected{background:rgb(0, 0, 0, 150); border:1px solid white; border-radius:4px;}");
    m_sessionsList->setSpacing(5);
    m_sessionsList->setGeometry(220, 55, 300, 40*sessionNum+20*(sessionNum-1));
    connect(m_sessionsList, &QListWidget::itemClicked, this, &SessionWindow::saveAndBack);
    addSessionLabels();
//    testAddSessionLabels();
}

void SessionWindow::addSessionLabels()
{
    for(int i = 0; i < m_sessionsModel->rowCount(); i++) {
        QString sessionName = m_sessionsModel->index(i, 0).data().toString();
        QString sessionKey = m_sessionsModel->index(i, 0).data(Qt::UserRole).toString();
        IconLabel *sessionLabel = new IconLabel(300, 40, this);
        sessionLabel->setIcon(getSessionIcon(sessionKey));
        if(sessionKey == m_defaultSession)
            sessionLabel->setText(sessionName + tr(" (Default)"));
        else
            sessionLabel->setText(sessionName);
        QListWidgetItem *newItem = new QListWidgetItem();
        m_sessionsList->insertItem(i,newItem);
        m_sessionsList->setItemWidget(newItem, sessionLabel);
    }
}

bool SessionWindow::eventFilter(QObject *obj, QEvent *event)
{
//    if(obj == m_backLabel) {
//        if(event->type() == QEvent::MouseButtonPress) {
//            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
//            if(mouseEvent->button() == Qt::LeftButton) {
//                m_backLabel->setPixmap(scaledPixmap(32, 32, ":/resource/arrow_left_active.png"));
//                return true;
//            }
//        }
//        if(event->type() == QEvent::MouseButtonRelease) {
//            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
//            if(mouseEvent->button() == Qt::LeftButton) {
//                m_backLabel->setPixmap(scaledPixmap(32, 32, ":/resource/arrow_left.png"));
//                saveAndBack();
//                return true;
//            }
//        }
//    }

    return QWidget::eventFilter(obj, event);
}

void SessionWindow::keyReleaseEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Tab){
        if(m_sessionsList->currentRow() == m_sessionsList->count() - 1)
            m_sessionsList->setCurrentRow(0);
        else
            m_sessionsList->setCurrentRow(m_sessionsList->currentRow()+1);
    }
    else if(e->key() == Qt::Key_Up){
        if(m_sessionsList->currentRow() == 0)
            m_sessionsList->setCurrentRow(m_sessionsList->count() - 1);
        else
            m_sessionsList->setCurrentRow(m_sessionsList->currentRow() - 1);
    }
    else if(e->key() == Qt::Key_Down){
        if(m_sessionsList->currentRow() == m_sessionsList->count() - 1)
            m_sessionsList->setCurrentRow(0);
        else
            m_sessionsList->setCurrentRow(m_sessionsList->currentRow() + 1);
    }
}

void SessionWindow::saveAndBack()
{
    int currentRow = m_sessionsList->currentRow();
    //传递给loginWindow的是session的唯一标识
    emit sessionSelected(m_sessionsModel->index(currentRow, 0).data(Qt::UserRole).toString());
    emit back();
}

void SessionWindow::setSessionModel(QSharedPointer<QAbstractItemModel> model)
{
    if(model.isNull()) {
        return ;
    }
    m_sessionsModel = model;
    initUI();
}

void SessionWindow::setSession(const QString &session)
{
    qDebug() << "current session: " << session;
    for(int i = 0; i < m_sessionsModel->rowCount(); i++){
        QString sessionKey = m_sessionsModel->index(i, 0).data(Qt::UserRole).toString();
        if(sessionKey == session){
            m_sessionsList->setCurrentRow(i);
            return;
        }
    }
}

QString SessionWindow::getSessionIcon(const QString &session)
{
    QString sessionIcon;
    sessionIcon = IMAGE_DIR + QString("badges/%1_badge.png").arg(session.toLower());
    QFile iconFile(sessionIcon);
    if(!iconFile.exists()){
        sessionIcon = IMAGE_DIR + QString("badges/unknown_badge.png");
    }
    return sessionIcon;
}

/**
 * @brief SessionWindow::testAddSessionLabels
 * 测试有多个session的情况
 */
void SessionWindow::testAddSessionLabels()
{
    QStringList sessionList{"MATE", "Ubuntu", "Xfce", "LUbuntu", "XUbuntu", "GNOME", "KDE", "Pantheon"};
    for(int i = 0; i < sessionList.size(); i++) {
        QString sessionName = sessionList[i];
        IconLabel *sessionLabel = new IconLabel(300, 40, this);
        sessionLabel->setIcon(getSessionIcon(sessionName));
        sessionLabel->setText(sessionName);
        QListWidgetItem *newItem = new QListWidgetItem();
        m_sessionsList->insertItem(i,newItem);
        m_sessionsList->setItemWidget(newItem, sessionLabel);
    }
}
