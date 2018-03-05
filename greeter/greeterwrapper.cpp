/* greeterwrapper.cpp
 * Copyright (C) 2018 yanghao <yanghao@kylinos.cn>
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
#include <QDebug>
#include "globalv.h"

GreeterWrapper::GreeterWrapper(QObject *parent) : QLightDM::Greeter(parent)
{
    //连接到lightdm
    if(!connectToDaemonSync()){
        qDebug() << "connect to Daemon failed";
        exit(1);
    }
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

void GreeterWrapper::startSession()
{ 
    //设置language
    if(!m_language.isEmpty()) {
        setLanguage(m_language);
    }
    qDebug() << "start session: " << m_session;
    //启动session
    if(!startSessionSync(m_session)) {
        Q_EMIT showMessage(tr("failed to start session."), QLightDM::Greeter::MessageTypeError);
        //如果启动session失败，需要重新认证
        authenticate(authenticationUser());
    }
}
