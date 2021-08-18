/* configuration.h
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
#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QObject>
#include <QSettings>
#include <QTranslator>
#include <QPixmap>

class Configuration : public QObject
{
    Q_OBJECT
private:
    explicit Configuration(QObject *parent = nullptr);

public:
    static Configuration *instance(QObject *parent = nullptr);

    QVariant getValue(const QString &);
    void setValue(const QString&, const QVariant &);
    bool hasValue(const QString &);

    QString getLastLoginUser();
    void saveLastLoginUser(const QString &);
    void saveLastLoginUser1(const QString &);
    bool getLastNumLock();
    void saveLastNumLock(bool value);
    QPixmap getLogo();
    QString getDefaultBackgroundName();
    bool getUseFirstDevice();
    QTranslator             *m_trans;

private:
    QSettings               *configSettings;
    QSettings               *recordSettings;

    static Configuration    *instance_;

};

#endif // CONFIGURATION_H
