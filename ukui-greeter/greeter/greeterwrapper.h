/* greeterwrapper.h
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
#ifndef GREETERWRAPPER_H
#define GREETERWRAPPER_H

#include <QLightDM/Greeter>

enum{
    rootWinPicture,
    rootWinColor
};
class GreeterWrapper : public QLightDM::Greeter
{
    Q_OBJECT
public:
    explicit GreeterWrapper(QObject *parent=0);
    void setLang(const QString&);
    QString lang();
    void setSession(const QString&);
    QString session();
    void startSession();
    QString userName();
    void setUserName(const QString &);
    void setrootWindow();
    //type 0代表绘制背景图片，1代表绘制背景颜色
    void setrootWindowBackground(int type,unsigned int color,QString filename);
    QString getEnsureShareDir(QString username);

private:
    QString m_language;
    QString m_session;
    QString m_userName;
    QString m_rootWindowBackground;
    int drawBackgroundType;
    unsigned int drawBackgroundColor;

signals:
    void aboutToLogin();
    void startSessionFailed();
    void authenticationSucess();
};

#endif // GREETERWRAPPER_H
