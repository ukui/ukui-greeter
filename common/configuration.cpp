#include "configuration.h"
#include <QFile>
#include <QStandardPaths>
#include <QFont>
#include <QFontMetrics>
#include <QTextOption>
#include <QPainter>
#include <QDebug>

#define CONFIG_FILE "/etc/lightdm/ukui-greeter.conf"

Configuration* Configuration::instance_ = nullptr;

Configuration::Configuration(QObject *parent)
    : QObject(parent)
{
    QString recodfile = QStandardPaths::displayName(QStandardPaths::CacheLocation) + "/ukui-greeter.conf";
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
    if(getSystemDistrib() == "Kylin")
        return "background-kylin.png";
    else
        return "background-ubuntu.jpg";
}
