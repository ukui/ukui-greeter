#ifndef GREETERWRAPPER_H
#define GREETERWRAPPER_H

#include <QLightDM/Greeter>

class GreeterWrapper : public QLightDM::Greeter
{
    Q_OBJECT
public:
    explicit GreeterWrapper(QObject *parent=0);
    void setLang(const QString&);
    QString lang();
    void setSession(const QString&);
    QString session();
    void startSession();

private:
    QString m_language;
    QString m_session;

signals:
    void aboutToLogin();

};

#endif // GREETERWRAPPER_H
