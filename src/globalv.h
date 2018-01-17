#ifndef GLOBALV_H
#define GLOBALV_H
#include <QPixmap>
#include <QFile>
#include <QDebug>

class QPoint;
class QRect;

extern float scale;     //缩放比例
extern int fontSize;    //字体大小
extern QFont font;      //字体

extern QPixmap scaledPixmap(int width, int height, QString url);
extern bool isPointInRect(const QPoint&, const QRect&);
extern bool isLessThan(const QRect&, const QRect&);

#define QDEBUG qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<": "

#endif // GLOBALV_H

