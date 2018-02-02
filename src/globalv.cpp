#include "globalv.h"

#include <QDebug>
#include <QRect>

QPixmap scaledPixmap(int width, int height, QString url)
{
    QFile imgFile(url);
    if(!imgFile.exists()){
        qDebug()<< "not find the pixmap file";
        return QPixmap();
    }
    QPixmap pixmap(url);
    return pixmap.scaled(width, height, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
}
