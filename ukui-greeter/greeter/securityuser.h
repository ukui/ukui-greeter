/* securityuser.h
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
#ifndef SECURITYUSER_H
#define SECURITYUSER_H

#include <QStringList>
#include <QString>

enum SECURITYTYPE
{
    NORMAL,
    WHITELIST,
    BLACKLIST
};

class SecurityUser
{
public:
    SecurityUser();
    int getMode();
    QStringList getBlackList();
    QStringList getWhiteList();
    bool isSecrityUser(QString username);
    static SecurityUser *instance();

private:
    int mode;
    QStringList blacklist;
    QStringList whitelist;

    void loadJsonFile();

    static SecurityUser    *instance_;
};

#endif // SECURITYUSER_H
