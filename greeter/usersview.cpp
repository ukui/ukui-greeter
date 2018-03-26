#include "usersview.h"
#include <QListWidget>
#include <QDebug>
#include <QPushButton>
#include <QKeyEvent>
#include <QSettings>
#include <QStandardPaths>
#include <QLightDM/UsersModel>

UsersView::UsersView(QWidget *parent) :
    QWidget(parent)
{
    resize(USERSVIEW_WIDTH, USERSVIEW_HEIGHT);
    initUI();
}

UsersView::~UsersView()
{
}


void UsersView::initUI()
{
    usersList = new QListWidget(this);
    usersList->setObjectName(QStringLiteral("usersList"));
    usersList->setFlow(QListWidget::LeftToRight);
    usersList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    usersList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    usersList->setSelectionMode(QListWidget::NoSelection);
    usersList->setFocusPolicy(Qt::NoFocus);
    usersList->setContentsMargins(10, 0, 10, 0);

    lastButton = new QPushButton(this);
    lastButton->setObjectName(QStringLiteral("lastButton"));
    lastButton->setVisible(false);
    lastButton->setIcon(QIcon(":/resource/prev.png"));

    nextButton = new QPushButton(this);
    nextButton->setObjectName(QStringLiteral("nextButton"));
    nextButton->setVisible(false);
    nextButton->setIcon(QIcon(":/resource/next.png"));

    connect(lastButton, &QPushButton::clicked, this, &UsersView::pageUp);
    connect(nextButton, &QPushButton::clicked, this, &UsersView::pageDown);
}

void UsersView::setModel(QAbstractListModel *model)
{
    usersModel = model;
    connect(usersModel, &QAbstractListModel::rowsInserted, this, &UsersView::onUserAdded);
    connect(usersModel, &QAbstractListModel::rowsRemoved, this, &UsersView::onUserRemoved);
    connect(usersModel, &QAbstractListModel::dataChanged, this, &UsersView::onUserChanged);

    //选中上一次登录的用户
    QString recordFile = QStandardPaths::displayName(QStandardPaths::CacheLocation) + "/ukui-greeter.conf";
    QSettings settings(recordFile, QSettings::IniFormat);
    QString lastLoginUser = settings.value("lastLoginUser").toString();
    qDebug() << "lastLoginUser: "<< lastLoginUser;
    for(int i = 0; i < usersModel->rowCount(); i++)
        insertUserEntry(i);
    for(int i = 0; i < usersModel->rowCount(); i++)
        if(lastLoginUser == usersModel->index(i, 0).data(Qt::DisplayRole).toString())
            setCurrentRow(i);
}

void UsersView::resizeEvent(QResizeEvent *event)
{
    int buttonTop = scale > 0.5 ? 10 : 5;
    lastButton->setGeometry(0, buttonTop, 64 * scale, 130 * scale);
    lastButton->setIconSize(QSize(lastButton->width(), lastButton->height()));

    int leftNum = 5 - (usersList->count() >= 5 ? 5 : usersList->count());
    usersList->move(lastButton->geometry().right() + 40*scale + leftNum * ITEM_WIDTH / 2, 0);
    int num = usersList->count() <= 5 ? usersList->count() : 5;
    usersList->resize(ITEM_WIDTH * num + 2, ITEM_HEIGHT);
    for(int i = 0; i < usersList->count(); i++) {
        QListWidgetItem *item = usersList->item(i);
        item->setSizeHint(QSize(ITEM_WIDTH, ITEM_HEIGHT));
        UserEntry *entry = static_cast<UserEntry*>(usersList->itemWidget(item));
        entry->setFixedSize(ENTRY_WIDTH, ENTRY_HEIGHT);
    }
    nextButton->setGeometry(lastButton->geometry().right() + 40*scale + ITEM_WIDTH * 5 + 2,
                            buttonTop, 64 * scale, 130 * scale);
    nextButton->setIconSize(QSize(nextButton->width(), nextButton->height()));

    QWidget::resizeEvent(event);
}

void UsersView::keyReleaseEvent(QKeyEvent *event)
{
    switch(event->key()) {
    case Qt::Key_Down:
    case Qt::Key_Right:
        qDebug() << "current row: " << usersList->currentRow();
        if(usersList->currentRow() < usersList->count()-1)
            setCurrentRow(usersList->currentRow() + 1);
        qDebug() << "current row: " << usersList->currentRow();
        break;
    case Qt::Key_Up:
    case Qt::Key_Left:
        if(usersList->currentRow() > 0)
            setCurrentRow(usersList->currentRow() - 1);
        break;
    case Qt::Key_PageDown:
        pageDown();
        break;
    case Qt::Key_PageUp:
        pageUp();
        break;
    case Qt::Key_Return:
    {
        QString userName = usersModel->index(usersList->currentRow(), 0)
                .data(Qt::DisplayRole).toString();
        onUserClicked(userName);
        break;
    }
    default:
        QWidget::keyReleaseEvent(event);
    }

}

void UsersView::showEvent(QShowEvent *event)
{
    this->setFocus();

    QWidget::showEvent(event);
}

void UsersView::insertUserEntry(int row)
{
    QModelIndex index = usersModel->index(row, 0);
    UserEntry *entry = new UserEntry(index.data(Qt::DisplayRole).toString(),
                                     index.data(QLightDM::UsersModel::ImagePathRole).toString(),
                                     index.data(QLightDM::UsersModel::LoggedInRole).toBool(),
                                     this);
    connect(entry, &UserEntry::pressed, this, &UsersView::onUserPressed);
    connect(entry, &UserEntry::clicked, this, &UsersView::onUserClicked);
    QListWidgetItem *item = new QListWidgetItem();
    item->setSizeHint(QSize(ITEM_WIDTH, ITEM_HEIGHT));
    usersList->insertItem(row, item);
    usersList->setItemWidget(item, entry);
    setCurrentRow(row);

    if(usersList->count() <= 5) {
        usersList->resize(ITEM_WIDTH * usersList->count() + 2, ITEM_HEIGHT);
        usersList->move(usersList->geometry().left() - ITEM_WIDTH/2, 0);    //左移半个身位
    } else {
        lastButton->setVisible(true);
        nextButton->setVisible(true);
    }
}


void UsersView::onUserPressed()
{
    QString objName = sender()->objectName();
    for(int i = 0; i < usersList->count(); i++){
        QWidget *entry = usersList->itemWidget(usersList->item(i));
        if(entry->objectName() == objName){
            setCurrentRow(i);
        }
    }
    update();
}

void UsersView::onUserClicked(const QString& userName)
{
    qDebug() << userName << " selected";
//    QString recordFile = QStandardPaths::displayName(QStandardPaths::CacheLocation) + "/ukui-greeter.conf";
//    QSettings settings(recordFile, QSettings::IniFormat);
//    settings.setValue("lastLoginUser", userName);
//    settings.sync();
    for(int i = 0; i < usersModel->rowCount(); i++){
        if(usersModel->index(i, 0).data(Qt::DisplayRole).toString() == userName) {
            QModelIndex index = usersModel->index(i);
            Q_EMIT userSelected(index);
        }
    }

}

void UsersView::onUserAdded(const QModelIndex &parent, int left, int right)
{
    Q_UNUSED(parent);

    for(int i = left; i <= right; i++){
        insertUserEntry(i);
    }
}

void UsersView::onUserRemoved(const QModelIndex &parent, int left, int right)
{
    Q_UNUSED(parent);

    for(int i = left; i <= right; i++){
        usersList->takeItem(i);
        setCurrentRow(usersList->currentRow());
        if(usersList->count() <= 5) {
            lastButton->setVisible(false);
            nextButton->setVisible(false);
        }
        if(usersList->count() < 5) {
            usersList->resize(ITEM_WIDTH * usersList->count() + 2, ITEM_HEIGHT);
            usersList->move(usersList->geometry().left() + ITEM_WIDTH / 2, 0);
        }
    }
}

void UsersView::onUserChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    int begin = topLeft.row();
    int end = bottomRight.row();
    for(int i = begin; i <= end; i++) {
        UserEntry *entry = static_cast<UserEntry*>(usersList->itemWidget(usersList->item(i)));
        QModelIndex index = usersModel->index(i, 0);
        entry->setUserName(index.data(Qt::DisplayRole).toString());
        entry->setFace(index.data(QLightDM::UsersModel::ImagePathRole).toString());
        entry->setLogin(index.data(QLightDM::UsersModel::LoggedInRole).toBool());
    }
}

void UsersView::setCurrentRow(int row)
{
    if(row < 0 || row >= usersList->count())
        return;
    usersList->setCurrentRow(row);
    UserEntry *entry = static_cast<UserEntry*>(usersList->itemWidget(usersList->currentItem()));
    entry->setSelected();
}

void UsersView::pageUp()
{
    if(usersList->currentRow() >= 5)
        setCurrentRow(usersList->currentRow() - 5);
    else
        setCurrentRow(0);
}

void UsersView::pageDown()
{
    if(usersList->count() - usersList->currentRow() >= 5)
        setCurrentRow(usersList->currentRow() + 5);
    else
        setCurrentRow(usersList->count() - 1);
}
