#include "usersmodel.h"
#include <QLightDM/UsersModel>

UsersModel::UsersModel(QObject *parent) :
    ExtraRowProxyModel(parent),
    m_showGuest(false)
{
    setSourceModel(new QLightDM::UsersModel(this));
}

void UsersModel::setShowGuest(bool isShowGuest)
{
    if(m_showGuest == isShowGuest)
        return;
    m_showGuest = isShowGuest;
    if(m_showGuest)
    {
        QStandardItem *guest = new QStandardItem(tr("Guest"));
        guest->setData("*guest", QLightDM::UsersModel::NameRole);
        extraRowModel()->appendRow(guest);
    }
    else
    {
        extraRowModel()->removeRow(0);
    }
}

bool UsersModel::showGuest() const
{
    return m_showGuest;
}
