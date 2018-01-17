#include "globalv.h"

//#include <QPoint>
#include <QRect>

QPixmap scaledPixmap(int width, int height, QString url)
{
    QFile imgFile(url);
    if(!imgFile.exists()){
        qDebug()<< "not find this file";
        return QPixmap();
    }
    QPixmap pixmap(url);
    return pixmap.scaled(width, height, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
}

bool isPointInRect(const QPoint& point, const QRect& rect)
{
    if(point.x() >= rect.left() && point.x() <= rect.right() &&
       point.y() >= rect.top() && point.y() <= rect.bottom()) {
        return true;
    }
    return false;
}

bool isLessThan(const QRect& rect1, const QRect& rect2)
{
    if(rect1.width() < rect2.width() ||
       rect1.height() < rect2.height()){
        return true;
    }
    return false;
}
