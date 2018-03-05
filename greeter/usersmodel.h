/* usersmodel.h
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
#ifndef USERSMODEL_H
#define USERSMODEL_H

#include "extrarowproxymodel.h"

class UsersModel : public ExtraRowProxyModel
{
    Q_OBJECT
    Q_PROPERTY(bool m_showGuest READ showGuest WRITE setShowGuest)
    Q_PROPERTY(bool m_showManualLogin READ showManualLogin WRITE setShowManualLogin)
public:
    explicit UsersModel(bool hideUsers=false, QObject *parent=0);
    void setShowGuest(bool);
    bool showGuest() const;
    void setShowManualLogin(bool);
    bool showManualLogin() const;
private:
    bool m_showGuest;
    bool m_showManualLogin;
};

#endif // USERSMODEL_H
