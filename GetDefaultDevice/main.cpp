#include <QApplication>
#include <QSettings>
#include <QDebug>
#define UKUI_BIOMETRIC_CONFIG_PATH  ".biometric_auth/ukui_biometric.conf"
#define UKUI_BIOMETRIC_SYS_CONFIG_PATH  "/etc/biometric-auth/ukui-biometric.conf"

int main(int argc, char *argv[])
{
    QCoreApplication::setSetuidAllowed(true);
    QCoreApplication a(argc, argv);

    QString userName = argv[1];

    QString configPath = QString("/home/%1/" UKUI_BIOMETRIC_CONFIG_PATH).arg(userName);
    QSettings settings(configPath, QSettings::IniFormat);
//    qDebug() << "configure path: " << settings.fileName();

    QString defaultDevice = settings.value("DefaultDevice").toString();
    if(defaultDevice.isEmpty())
    {
        QSettings sysSettings(UKUI_BIOMETRIC_SYS_CONFIG_PATH, QSettings::IniFormat);
        defaultDevice = sysSettings.value("DefaultDevice").toString();
    }
 //   qDebug()<<defaultDevice;
        char*  ch;
        QByteArray ba = defaultDevice.toLatin1(); // must
        ch=ba.data();
        printf("%s",ch);
}

