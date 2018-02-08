#ifndef GLOBALV_H
#define GLOBALV_H

#include <QPixmap>
#include <QFile>
#include <QSettings>
#include <QString>
#include <QLocale>

#define STR(s) #s
#define GET_STR(s) STR(s)

#define PREFIX QString(GET_STR(RESOURCE_PATH))
#define IMAGE_DIR PREFIX+"/images/"
#define QM_DIR PREFIX+"/translations/"

class QPoint;
class QRect;

extern float scale;     //缩放比例
extern int fontSize;    //字体大小
extern QFont font;      //字体
extern QString configFile; //配置文件路径
extern QLocale::Language language;

QPixmap scaledPixmap(int width, int height, QString url);
QPixmap logoGenerator(const QString &);
QString getSystemVersion();

#endif // GLOBALV_H

