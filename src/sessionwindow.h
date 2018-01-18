#ifndef SESSIONWINDOW_H
#define SESSIONWINDOW_H

#include <QWidget>
#include <QSharedPointer>
#include <QAbstractItemModel>

class QLabel;
class IconLabel;
class SessionWindow : public QWidget
{
    Q_OBJECT
public:
    explicit SessionWindow(QWidget *parent = 0);
    void setSessionModel(QSharedPointer<QAbstractItemModel> model);
    bool eventFilter(QObject *, QEvent *);

signals:
    void back();
    void sessionChanged(const QString& sessionName);

private:
    void initUI();
    QPixmap getSessionIcon(const QString&);

    QLabel *m_backLabel;
    QLabel *m_prompt;
    QSharedPointer<QAbstractItemModel> m_sessionsModel;
    int m_selectedIndex;
};

#endif // SESSIONWINDOW_H
