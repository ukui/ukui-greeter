#ifndef GLOBALV_H
#define GLOBALV_H
#include <QPixmap>
#include <QFile>

extern float scale;     //缩放比例
extern int fontSize;    //字体大小
extern QFont font;      //字体

extern QPixmap scaledPixmap(int width, int height, QString url);

#endif // GLOBALV_H

