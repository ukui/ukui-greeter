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
    bool eventFilter(QObject *, QEvent *);
    void setSessionModel(QSharedPointer<QAbstractItemModel> model);
    void setSession(const QString&);

signals:
    void back();
    void sessionSelected(const QString& sessionName);

private:
    void initUI();
    QPixmap getSessionIcon(const QString&);

    QLabel *m_backLabel;
    QLabel *m_prompt;
    QSharedPointer<QAbstractItemModel> m_sessionsModel;
    int m_selectedIndex;
    int m_origIndex;
};

#endif // SESSIONWINDOW_H
