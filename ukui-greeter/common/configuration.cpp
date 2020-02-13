/*
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
#include "configuration.h"
#include <QFile>
#include <QDir>
#include <QFont>
#include <QFontMetrics>
#include <QTextOption>
#include <QPainter>
#include <QDebug>
#include "globalv.h"

#define CONFIG_FILE "/etc/lightdm/ukui-greeter.conf"

Configuration* Configuration::instance_ = nullptr;

Configuration::Configuration(QObject *parent)
    : QObject(parent)
{
    QString recodfile = QDir::homePath() + "/.cache/ukui-greeter.conf";
    qDebug() << recodfile;

    configSettings = new QSettings(CONFIG_FILE, QSettings::IniFormat, this);
    recordSettings = new QSettings(recodfile, QSettings::IniFormat, this);
}

Configuration* Configuration::instance(QObject *parent)
{
    if(instance_ == nullptr)
        instance_ = new Configuration(parent);
    return instance_;
}

QVariant Configuration::getValue(const QString &key)
{
    configSettings->beginGroup("Greeter");
    QVariant value = configSettings->value(key);
    configSettings->endGroup();

    return value;
}

void Configuration::setValue(const QString &key, const QVariant &value)
{
    configSettings->beginGroup("Greeter");
    configSettings->setValue(key, value);
    configSettings->endGroup();
}

QString Configuration::getLastLoginUser()
{
    recordSettings->beginGroup("Greeter");
    QString lastLoginUser = recordSettings->value("lastLoginUser").toString();
    recordSettings->endGroup();
    return lastLoginUser;
}

void Configuration::saveLastLoginUser(const QString &userRealName)
{
    recordSettings->beginGroup("Greeter");
    recordSettings->setValue("lastLoginUser", userRealName);
    recordSettings->endGroup();
    recordSettings->sync();
}


/**
 * 获取系统版本号
 */
QString getSystemVersion()
{
    QSettings settings("/etc/lsb-release", QSettings::IniFormat);
    QString release = settings.value("DISTRIB_RELEASE").toString();
    QString description = settings.value("DISTRIB_DESCRIPTION").toString();
    if(description.right(3) == "LTS")
        release = release + " LTS";
    return release;
}

QString getSystemDistrib()
{
    QSettings settings("/etc/lsb-release", QSettings::IniFormat);
    QString distribId = settings.value("DISTRIB_ID").toString();
    return distribId;
}

/**
 * @brief logoGenerator
 * @param text
 * @return
 * 生成logo
 */
QPixmap logoGenerator(const QString &text)
{
//    if(getSystemDistrib() == "Kylin")
//        return QPixmap(":/resource/kylin-logo.png");

//    QString logoFile(IMAGE_DIR + "logo.png");
//    if(QFile(logoFile).exists())
//    {
//        QPixmap logo(IMAGE_DIR + "logo.png");
//        return logo;
//    }
    QPixmap logoBare;
    logoBare.load(":/resource/uk-logo.png");

    QFont font("ubuntu", 18);
    QFontMetrics fm(font);
    int textPixelSize = fm.width(text);

    QPixmap logo(logoBare.width() + textPixelSize + 3 + 30, logoBare.height());
    logo.fill(Qt::transparent);

    QRect logoBareRect(30 , 0, logoBare.width(), logoBare.height());
    QPainter painter;
    painter.begin(&logo);
    painter.drawPixmap(logoBareRect, logoBare, logoBare.rect());

    painter.setPen(Qt::white);
    painter.setFont(font);
    QTextOption option(Qt::AlignLeft | Qt::AlignVCenter);
    option.setWrapMode(QTextOption::WordWrap);
    QRect versionRect(logoBareRect.right()+3, 3, logo.width() - logoBare.width(), logo.height());
    painter.drawText(versionRect, text, option);

    return logo;
}


QPixmap Configuration::getLogo()
{
    /**
     * 先从配置文件中读取logo文件，如果不存在，则生成logo图片
     */
    QString logoFilePath = getValue("logo").toString();
    if(QFile(logoFilePath).exists())
        return QPixmap(logoFilePath);

    return logoGenerator(getSystemVersion());
}


QString Configuration::getDefaultBackgroundName()
{

    if(getSystemDistrib().contains("Ubuntu",Qt::CaseInsensitive)){
        QString filepath = "/usr/share/backgrounds/warty-final-ubuntukylin.jpg";
        QFileInfo file(filepath);
        if(file.exists())
            return "/usr/share/backgrounds/warty-final-ubuntukylin.jpg";
        else
            return NULL;
    }
    else if(getSystemDistrib().contains("Kylin",Qt::CaseInsensitive)){
        QFileInfo fileInfo("/usr/share/backgrounds/kylin/kylin-background.png");
        if(fileInfo.isFile())
        {
            return "/usr/share/backgrounds/kylin/kylin-background.png";
        }
        return "/usr/share/backgrounds/kylin-lightdm-default-background.png";
    }
    else{
        return NULL;
    }
}
