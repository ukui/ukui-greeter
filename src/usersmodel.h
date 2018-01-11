#ifndef USERSMODEL_H
#define USERSMODEL_H

#include "extrarowproxymodel.h"

class UsersModel : public ExtraRowProxyModel
{
    Q_OBJECT
    Q_PROPERTY(bool m_showGuest READ showGuest WRITE setShowGuest)
    Q_PROPERTY(bool m_showManualLogin READ showManualLogin WRITE setShowManualLogin)
public:
    explicit UsersModel(QObject *parent=0);
    void setShowGuest(bool);
    bool showGuest() const;
    void setShowManualLogin(bool);
    bool showManualLogin() const;
private:
    bool m_showGuest;
    bool m_showManualLogin;
};

#endif // USERSMODEL_H
