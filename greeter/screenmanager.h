#ifndef SCREENMANAGER_H
#define SCREENMANAGER_H

#include <QObject>
#include <QSharedPointer>
#include "screenmodel.h"

class GreeterWindow;
class QPoint;
class ScreenManager : public QObject
{
    Q_OBJECT
public:
    explicit ScreenManager(QObject *parent = 0);
    void showWindow();

protected:
//    bool eventFilter(QObject *, QEvent *);

private slots:
    void onScreenChanged(const QModelIndex&, const QModelIndex&);
    void onScreenModelReset();

public slots:
    void onMouseMove(const QPoint&);

private:
    QSharedPointer<QAbstractListModel> m_model;
    QVector<QWidget *> m_windows;
    int m_activeScreen;
    GreeterWindow *m_greeterWnd;
};

#endif // SCREENMANAGER_H
