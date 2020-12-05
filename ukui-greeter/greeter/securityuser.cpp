#include <QCoreApplication>
#include <QJsonObject>
#include <QDebug>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonDocument>
#include <QStringLiteral>
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
