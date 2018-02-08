/**
 * @brief   生成logo
 *
 * @author  yanghao <yanghao@kylinos.cn>
 *
 * @date    2018-02-08:14:37
 *
 **/

#include <QPixmap>
#include <QFontMetrics>
#include <QDebug>
#include <QPainter>
#include <QRegularExpression>
#include "globalv.h"

/**
 * @brief getSystemVersion
 * @return
 * 获取系统版本号
 */
QString getSystemVersion()
{
    QSettings settings("/etc/lsb-release", QSettings::IniFormat);
    QString version = settings.value("DISTRIB_DESCRIPTION").toString();
    int n = version.length() - version.indexOf(QRegularExpression("\\d"));
    version = version.right(n);
    return version;
}

/**
 * @brief logoGenerator
 * @param text
 * @return
 * 生成logo
 */
QPixmap logoGenerator(const QString &text)
{
    QPixmap logoBare;
    logoBare.load(":/resource/logo-bare.png");

    QFont font("ubuntu", 18);
    QFontMetrics fm(font);
    int textPixelSize = fm.width(text);

    QPixmap logo(logoBare.width() + textPixelSize + 3, logoBare.height());
    logo.fill(Qt::transparent);

    QPainter painter;
    painter.begin(&logo);
    painter.drawPixmap(logoBare.rect(), logoBare, logoBare.rect());

    painter.setPen(Qt::white);
    painter.setFont(font);
    QTextOption option(Qt::AlignLeft | Qt::AlignVCenter);
    option.setWrapMode(QTextOption::WordWrap);
    QRect versionRect(logoBare.rect().right()+3, 3, logo.width() - logoBare.width(), logo.height());
    painter.drawText(versionRect, text, option);

    return logo;
}
