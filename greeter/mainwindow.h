#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QSharedPointer>
#include "screenmodel.h"

class GreeterWindow;
class MainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    void paintEvent(QPaintEvent *);
    void mouseMoveEvent(QMouseEvent *);

signals:

public slots:
    void onScreenResized(const QModelIndex&, const QModelIndex&);
    void onScreenCountChanged();

private:
    QSharedPointer<ScreenModel> m_screenModel;
    GreeterWindow *m_greeterWnd;
    int m_activeScreen;
    QPixmap m_logo;
};

#endif // MAINWINDOW_H
