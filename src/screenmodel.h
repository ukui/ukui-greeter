#ifndef SCREENMODEL_H
#define SCREENMODEL_H

#include <QAbstractListModel>
#include <QVector>
#include <QRect>

class ScreenModel : public QAbstractListModel
{
public:
    ScreenModel(QObject *parent=0);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::UserRole) const;

private slots:
    void onScreenResized(int screen);
    void onScreenCountChanged(int newCount);
    void loadScreens();

private:
    QVector<QRect> m_screen;
};

#endif // SCREENMODEL_H
