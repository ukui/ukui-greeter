#ifndef GLOBALV_H
#define GLOBALV_H

#include <QPixmap>
#include <QFile>
#include <QSettings>
#include <QString>
#include <QLocale>

#define STR(s) #s
#define GET_STR(s) STR(s)

#ifdef USE_QRC
    #define RESOURCE_DIR (":/resource/")
#else
    #define RESOURCE_DIR GET_STR(RESOURCE)
#endif

class QPoint;
class QRect;

extern float scale;     //缩放比例
extern int fontSize;    //字体大小
extern QFont font;      //字体
extern QString configFile; //配置文件路径
extern QLocale::Language language;

extern QPixmap scaledPixmap(int width, int height, QString url);


#endif // GLOBALV_H

