#include "usersmodel.h"
#include <QLightDM/UsersModel>

UsersModel::UsersModel(bool hideUsers, QObject *parent) :
    ExtraRowProxyModel(parent),
    m_showGuest(false)
{
    if(!hideUsers)
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
        QStandardItemModel *model = extraRowModel();
        for(int i = 0; i < model->rowCount(); i++){
            QStandardItem *item = model->item(i, 0);
            if(item->text() == tr("Guest")){
                model->removeRow(i);
            }
        }
    }
}

bool UsersModel::showGuest() const
{
    return m_showGuest;
}

void UsersModel::setShowManualLogin(bool isShowManualLogin)
{
    if(m_showManualLogin == isShowManualLogin)
        return;
    m_showManualLogin = isShowManualLogin;
    if(m_showManualLogin){
        QStandardItem *manualLogin = new QStandardItem(tr("Login"));
        manualLogin->setData("*login", QLightDM::UsersModel::NameRole);
        extraRowModel()->appendRow(manualLogin);
    }
    else{
        QStandardItemModel *model = extraRowModel();
        for(int i = 0; i < model->rowCount(); i++){
            QStandardItem *item = model->item(i, 0);
            if(item->text() == tr("Login")){
                model->removeRow(i);
            }
        }
    }
}

bool UsersModel::showManualLogin() const
{
    return m_showManualLogin;
}
