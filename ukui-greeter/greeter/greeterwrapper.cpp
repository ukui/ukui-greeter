/* greeterwrapper.cpp
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
#include "greeterwrapper.h"
#include "rootWindowBackground.h"
#include <QDesktopWidget>
#include <QDebug>
#include <QBitArray>
#include <QTextCodec>
#include "globalv.h"
#include "common/configuration.h"

GreeterWrapper::GreeterWrapper(QObject *parent) : QLightDM::Greeter(parent)
{
    //连接到lightdm
    if(!connectToDaemonSync()){
        qDebug() << "connect to Daemon failed";
        exit(1);
    }

    drawBackgroundType = 0;
    drawBackgroundColor = 0x0;
}

void GreeterWrapper::setLang(const QString &lang)
{
    this->m_language = lang;
}

QString GreeterWrapper::lang()
{
    return this->m_language;
}

void GreeterWrapper::setSession(const QString &session)
{
    m_session = session;
}

QString GreeterWrapper::session()
{
    return m_session;
}

QString GreeterWrapper::userName()
{
    return m_userName;
}

void GreeterWrapper::setUserName(const QString &userName)
{
    m_userName = userName;
}

void GreeterWrapper::setrootWindowBackground(int type,unsigned int color,QString filename)
{
    drawBackgroundType = type;
    if(drawBackgroundType == rootWinPicture)
        m_rootWindowBackground = filename;
    else if(drawBackgroundType == rootWinColor)
        drawBackgroundColor = color;  
}

bool GreeterWrapper::getdrawBackgroundIsStarted()
{
    return drawBackgroundIsStarted;
}

void GreeterWrapper::setrootWindow()
{
    drawBackgroundIsStarted = true;
    Configuration  *m_configure = Configuration::instance();
    QString m_defaultBackgroundPath = m_configure->getDefaultBackgroundName();
    if(m_rootWindowBackground.isEmpty())
        m_rootWindowBackground = m_defaultBackgroundPath;
    if(m_rootWindowBackground.isEmpty())
        return;

    char*  path;
    QByteArray ba = m_rootWindowBackground.toLatin1(); // must
    path=ba.data();

    if(drawBackgroundType == 0)
     	setRootWindowBackground(0,0,path);
    else
        setRootWindowBackground(1,drawBackgroundColor,NULL);
    drawBackgroundIsStarted = false;
}

QString GreeterWrapper::getEnsureShareDir(QString username)
{
    return ensureSharedDataDirSync(username);
}

void GreeterWrapper::startSession()
{
    if(isAuthenticated())
        Q_EMIT authenticationSucess();

    //启动session
    //setrootWindow();
    draw_background();
    qDebug()<<"start session";
    if(!startSessionSync(m_session)) {
        Q_EMIT startSessionFailed();
        Q_EMIT showMessage(tr("failed to start session."), QLightDM::Greeter::MessageTypeError);
    }
}
