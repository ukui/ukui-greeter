#ifndef SESSIONWINDOW_H
#define SESSIONWINDOW_H

#include <QWidget>
#include <QSharedPointer>
#include <QAbstractItemModel>

class QLabel;
class IconLabel;
class QListWidget;
class QListWidgetItem;
class SessionWindow : public QWidget
{
    Q_OBJECT
public:
    explicit SessionWindow(const QString &session, QWidget *parent = 0);
    bool eventFilter(QObject *, QEvent *);
    void keyReleaseEvent(QKeyEvent *);
    void setSessionModel(QSharedPointer<QAbstractItemModel> model);
    void setSession(const QString&session);

signals:
    void back();
    void sessionSelected(const QString& sessionName);

private slots:
    void saveAndBack();

private:
    void initUI();
    QString getSessionIcon(const QString&);
    void addSessionLabels();
    void testAddSessionLabels();

    QLabel      *m_backLabel;
    QLabel      *m_prompt;
    QListWidget *m_sessionsList;
    QString     m_defaultSession;
    QSharedPointer<QAbstractItemModel> m_sessionsModel;
};

#endif // SESSIONWINDOW_H
