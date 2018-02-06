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

    //启动session
    if(!startSessionSync(m_session)) {
        Q_EMIT showMessage(tr("failed to start session."), QLightDM::Greeter::MessageTypeError);
        //如果启动session失败，需要重新认证
        authenticate(authenticationUser());
    }
}
