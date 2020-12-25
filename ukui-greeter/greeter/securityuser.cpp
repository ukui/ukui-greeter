/* securityuser.cpp
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
#include <QCoreApplication>
#include <QJsonObject>
#include <QDebug>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>

#include "securityuser.h"

#define GREETER_SECURITY_CONFIG_PATH "/etc/ukui-greeter/ukui-greeter-security-config.json"

SecurityUser* SecurityUser::instance_ = nullptr;

SecurityUser::SecurityUser()
    :mode(NORMAL)
{
    loadJsonFile();
}

SecurityUser* SecurityUser::instance()
{
    if(instance_ == nullptr)
        instance_ = new SecurityUser();
    return instance_;
}

void SecurityUser::loadJsonFile()
{
    QFile file(GREETER_SECURITY_CONFIG_PATH);

    if(file.exists() == false){
        qDebug()<<"file is not exists";
        return;
    }

    qDebug()<<"/etc/ukui-greeter/ukui-greeter-security-config.json is find!";

    file.open(QIODevice::ReadOnly);
    QString value = file.readAll();
    file.close();

    QJsonParseError parseJsonErr;
    QJsonDocument document  = QJsonDocument::fromJson(value.toUtf8(),&parseJsonErr);
    if(!(parseJsonErr.error == QJsonParseError::NoError)){
        qWarning()<<"parser json file failed";
        qWarning()<<parseJsonErr.errorString();
        return ;
    }

    QJsonObject  rootObj = document.object();
    if(rootObj.contains(QStringLiteral("ukui-greeter"))){
        QJsonObject greeterObj = rootObj.value(QStringLiteral("ukui-greeter")).toObject();
        if(greeterObj.contains(QStringLiteral("mode"))){
            QString tmp;
            tmp = greeterObj.value(QStringLiteral("mode")).toString();
            if(tmp == "normal"){
                mode = NORMAL;
            }
            else if(tmp == "blacklist"){
                mode = BLACKLIST;
            }
            else if(tmp == "whitelist"){
                mode = WHITELIST;
            }
        }else{
            qWarning()<<"file can not find mode!";
        }
        qDebug()<<"user security mode is"<<mode;

        if(mode == NORMAL)
            return ;

        if(greeterObj.contains(QStringLiteral("blacklist"))){
            QJsonArray tmpblacklist = greeterObj.value(QStringLiteral("blacklist")).toArray();
            for(int i = 0 ; i < tmpblacklist.size(); i++ ){
                blacklist.append(tmpblacklist.at(i).toString());
                qDebug()<<"blacklist add "<<blacklist.at(i);
            }
        }

        if(greeterObj.contains(QStringLiteral("whitelist"))){
            QJsonArray tmpwhitelist = greeterObj.value(QStringLiteral("whitelist")).toArray();
            for(int i = 0 ; i < tmpwhitelist.size(); i++ ){
                whitelist.append(tmpwhitelist.at(i).toString());
                qDebug()<<"whitelist add "<<whitelist.at(i);
            }
        }
    }
}

bool SecurityUser::isSecrityUser(QString username)
{
    if(username == "*login")
        return true;

    if(username == "*guest")
        return true;

    switch (mode) {
    case NORMAL:
        return true;
        break;
    case BLACKLIST:
        if(blacklist.contains(username))
            return false;
        else
            return true;
    case WHITELIST:
        if(whitelist.contains(username))
            return true;
        else
            return false;
    default:
        break;
    }
    return true;
}

int SecurityUser::getMode()
{
    return mode;
}

QStringList SecurityUser::getBlackList()
{
    return blacklist;
}

QStringList SecurityUser::getWhiteList()
{
    return whitelist;
}
