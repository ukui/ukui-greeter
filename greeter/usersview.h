#ifndef USERSVIEW_H
#define USERSVIEW_H

#include <QWidget>
#include "userentry.h"

#define ITEM_WIDTH (ENTRY_WIDTH + 40 * scale)
#define ITEM_HEIGHT ENTRY_HEIGHT
#define USERSVIEW_WIDTH (ITEM_WIDTH * 5 + 40 + 64*scale*2 + 4)
#define USERSVIEW_HEIGHT ITEM_HEIGHT

class QListWidget;
class QPushButton;
class QAbstractListModel;
class UsersView : public QWidget
{
    Q_OBJECT

public:
    explicit UsersView(QWidget *parent = 0);
    ~UsersView();
    void initUI();
    void setModel(QAbstractListModel *);
    void pageUp();
    void pageDown();

protected:
    void resizeEvent(QResizeEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void showEvent(QShowEvent *event);

private slots:
    void onUserPressed();
    void onUserClicked(const QString &);
    void onUserAdded(const QModelIndex&, int, int);
    void onUserRemoved(const QModelIndex&, int, int );
    void onUserChanged(const QModelIndex&, const QModelIndex&);
    void setCurrentRow(int row);

signals:
    void userSelected(const QModelIndex&);

private:
    void insertUserEntry(int row);

private:
    QAbstractListModel *usersModel;

    QListWidget *usersList;
    QPushButton *lastButton;
    QPushButton *nextButton;

    QPushButton *addButton;
    QPushButton *remButton;
    QPushButton *insertButton;
    QPushButton *zomInButton;
    QPushButton *zomOutButton;
};

#endif // USERSVIEW_H
