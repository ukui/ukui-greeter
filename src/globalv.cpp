#include "globalv.h"

#include <QDebug>

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
