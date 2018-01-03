#ifndef USERSMODEL_H
#define USERSMODEL_H

#include "extrarowproxymodel.h"

class UsersModel : public ExtraRowProxyModel
{
    Q_OBJECT
    Q_PROPERTY(bool showGuest READ showGuest WRITE setShowGuest)
public:
    explicit UsersModel(QObject *parent=0);
    void setShowGuest(bool);
    bool showGuest() const;

private:
    bool m_showGuest;
};

#endif // USERSMODEL_H
