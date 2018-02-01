#include "greeterwindow.h"
#include "loginwindow.h"
#include "userwindow.h"
#include "usersmodel.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QMouseEvent>
#include <QVBoxLayout>
#include <QMenu>
#include <QDebug>
#include <QLightDM/SessionsModel>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>
#include "globalv.h"
#include "powerwindow.h"

GreeterWindow::GreeterWindow(QWidget *parent)
    : QWidget(parent),
      m_sessionsModel(new QLightDM::SessionsModel(QLightDM::SessionsModel::LocalSessions)),
      m_greeter(new GreeterWrapper()),
      m_userWnd(nullptr),
      m_loginWnd(nullptr),
      m_sessionWnd(nullptr),
      m_powerWnd(nullptr),
      m_languageMenu(nullptr)
{
    m_usersModel = QSharedPointer<UsersModel>(new UsersModel(m_greeter->hideUsersHint()));
    if(m_greeter->hasGuestAccountHint()){    //允许游客登录
        qDebug() << "allow guest";
        m_usersModel->setShowGuest(true);
    }
    if(m_greeter->showManualLoginHint()) {    //允许手动输入用户名
        qDebug() << "allow manual login";
        m_usersModel->setShowManualLogin(true);
    }

    connect(m_greeter.data(), SIGNAL(autologinTimerExpired()),this, SLOT(timedAutologin()));
    installEventFilter(this);
}

GreeterWindow::~GreeterWindow()
{
    //#TODO: 断开连接
}

void GreeterWindow::initUI()
{
    //    for(int i = 0; i < 3; i++)
    //    {
    //        QStandardItem *item = new QStandardItem("test" + QString::number(i));
    //        m_usersModel->extraRowModel()->appendRow(item);
    //    }



//    QWidget *widget = new QWidget(this);
//    widget->setGeometry(rect());
//    widget->setWindowFlags(Qt::WindowStaysOnBottomHint);

//    m_layout = new QStackedLayout(widget);
//    m_firstWnd = nullptr;

    /* 如果只用一个用户的话，直接进入登录界面 */
    if(m_usersModel->rowCount() != 1) {
//        m_firstWnd = new QWidget(this);
//        m_firstWnd->setGeometry(rect());

        m_userWnd = new UserWindow(this);
        m_userWnd->setModel(m_usersModel);
        QRect userRect((rect().width()-m_userWnd->width())/2, (rect().height()-m_userWnd->height())/2,
                       m_userWnd->width(), m_userWnd->height());
        m_userWnd->setGeometry(userRect);
        connect(m_userWnd, SIGNAL(loggedIn(QModelIndex)), this, SLOT(onLoggedIn(QModelIndex)));
//        m_layout->addWidget(m_firstWnd);
    }

//    m_secondWnd = new QWidget(this);
//    m_secondWnd->setGeometry(rect());

    m_loginWnd = new LoginWindow(m_greeter, this);
    m_loginWnd->setUsersModel(m_usersModel);
    QRect loginRect((rect().width()-m_loginWnd->width())/2, (rect().height()-m_loginWnd->height())/2,
                    m_loginWnd->width(), m_loginWnd->height());
    m_loginWnd->setGeometry(loginRect);
    m_loginWnd->hide();
    connect(m_loginWnd, SIGNAL(back()), this, SLOT(onBacktoUsers()));
    connect(m_loginWnd, SIGNAL(selectSession(QString)), this, SLOT(onSelectSession(QString)));
//    m_layout->addWidget(m_secondWnd);

//    if(m_firstWnd) {
//        m_layout->setCurrentWidget(m_firstWnd);
//    }

    m_languageLB = new QLabel(this);
    m_languageLB->setGeometry(this->width() - 180, 20, 39, 39);
    m_languageLB->setAlignment(Qt::AlignCenter);
    m_languageLB->setFont(QFont("Ubuntu", 16));
    QPalette plt;
    plt.setColor(QPalette::WindowText, QColor(255, 255, 255, 200));
    m_languageLB->setPalette(plt);
    m_languageLB->installEventFilter(this);
    m_languageLB->setStyleSheet("QLabel::hover{background-color:rgb(255, 255, 255, 50)}");
    QString defaultLanguage = qgetenv("LANG").constData();
    if(defaultLanguage.contains("zh_CN")) {
        m_languageLB->setText(tr("Zh"));
        m_greeter->setLang("zh_CN");
    } else {
        m_languageLB->setText(tr("En"));
        m_greeter->setLang("en_US");
    }

    m_keyboardLB = new QLabel(this);
    m_keyboardLB->setGeometry(this->width() - 120, 20, 39, 39);
    m_keyboardLB->setPixmap(QPixmap(":/resource/keyboard.png"));
    m_keyboardLB->installEventFilter(this);
    m_keyboardLB->setStyleSheet("QLabel::hover{background-color:rgb(255, 255, 255, 50)}");

    m_powerLB = new QLabel(this);
    m_powerLB->setGeometry(this->width() - 60, 20, 39, 39);
    m_powerLB->setPixmap(QPixmap(":/resource/power.png"));
    m_powerLB->installEventFilter(this);
    m_powerLB->setStyleSheet("QLabel::hover{background-color:rgb(255, 255, 255, 50)}");
//    m_powerLB->setWindowFlags(Qt::WindowStaysOnTopHint);
}

bool GreeterWindow::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == m_keyboardLB) {
        //软键盘
        if(event->type() == QEvent::MouseButtonRelease) {

        }
    } else if(obj == m_powerLB) {
        //电源对话框
        if(event->type() == QEvent::MouseButtonRelease) {
            showPowerWnd();
            return true;
        }
    } else if(obj == m_languageLB) {
        //选择语言
        if(event->type() == QEvent::MouseButtonRelease) {
            showLanguageMenu();
            return true;
        }
    }else if(obj == m_languageMenu) {
        if(event->type() == QEvent::Close) {
            //当菜单关闭时需要重绘label，否则指针悬浮在label上产生的背景不会消失
            repaint(m_languageLB->geometry());
        }
    }
    //打开了电源对话框，点击对话框之外的地方关闭对话框
    if(m_powerWnd && event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        QPoint pos = mouseEvent->pos();
        if(!m_powerWnd->geometry().contains(pos)) {
            m_powerWnd->close();
            return true;
        }
    }
    return QWidget::eventFilter(obj, event);
}


void GreeterWindow::onLoggedIn(const QModelIndex &index)
{

    QString name = index.data().toString();
    if(name == tr("Guest")){
        //直接登录
    }
    m_loginWnd->setUserIndex(index);
//    m_layout->setCurrentWidget(m_secondWnd);

    switchWnd(1);
}

void GreeterWindow::onBacktoUsers()
{
//    if(m_firstWnd) {
//        m_layout->setCurrentWidget(m_firstWnd);
//    }
    switchWnd(0);
}

void GreeterWindow::onBacktoLogin()
{
//    if(m_secondWnd) {
//        m_layout->setCurrentWidget(m_secondWnd);
//    }
    switchWnd(1);
}

void GreeterWindow::onSelectSession(const QString &sessionName)
{
    if(!m_sessionWnd) {
//        m_thirdWnd = new QWidget(this);
//        m_thirdWnd->setGeometry(rect());
        m_sessionWnd = new SessionWindow(this);
        m_sessionWnd->setGeometry((rect().width()-m_sessionWnd->width())/2,
                                  (rect().height()-m_sessionWnd->height())/2,
                                  m_sessionWnd->width(), m_sessionWnd->height());
        connect(m_sessionWnd, SIGNAL(sessionSelected(QString)), m_loginWnd, SLOT(onSessionSelected(QString)));
        connect(m_sessionWnd, SIGNAL(back()), this, SLOT(onBacktoLogin()));
//        m_layout->addWidget(m_thirdWnd);
    }
    m_sessionWnd->setSession(sessionName);
//    m_layout->setCurrentWidget(m_thirdWnd);

    switchWnd(2);
}

void GreeterWindow::switchWnd(int index)
{
    m_userWnd->hide();
    m_loginWnd->hide();
    if(m_sessionWnd) {
        m_sessionWnd->hide();
    }

    switch (index) {
    case 0:
        m_userWnd->show();
        break;
    case 1:
        m_loginWnd->show();
        break;
    case 2:
        m_sessionWnd->show();
        break;
    default:
        break;
    }
}

void GreeterWindow::timedAutologin()
{
    if(m_greeter->isAuthenticated()) {
        /* 选择了用户 */

    } else {
        m_greeter->authenticateAutologin();
    }
}

void GreeterWindow::showPowerWnd()
{
    //创建一个黑色透明背景的窗口
    m_blackbgWnd = new QWidget(this);
    QPalette plt;
    plt.setColor(QPalette::Background, QColor(0, 0, 0, 150));
    m_blackbgWnd->setAutoFillBackground(true);
    m_blackbgWnd->setPalette(plt);
    m_blackbgWnd->setGeometry(0, 0, width(), height());
    m_blackbgWnd->show();

    //判断是否已经有用户登录
    bool hasOpenSessions = false;
    for(int i = 0; i < m_usersModel->rowCount(); i++) {
        hasOpenSessions = m_usersModel->index(i, 0).data(QLightDM::UsersModel::LoggedInRole).toBool();
        if(hasOpenSessions)
            break;
    }
    m_powerWnd = new PowerWindow(hasOpenSessions, this);
    connect(m_powerWnd, SIGNAL(aboutToClose()), m_blackbgWnd, SLOT(close()));
    m_powerWnd->show();
}

void GreeterWindow::showLanguageMenu()
{
    if(!m_languageMenu) {
        //目前只允许language设置为中文或者英文
        QAction *m_en = new QAction(tr("English"), this);
        m_en->setCheckable(true);
        QAction *m_zh = new QAction(tr("Chinese"), this);
        m_zh->setCheckable(true);
        QActionGroup *actgroup = new QActionGroup(this);
        actgroup->addAction(m_en);
        actgroup->addAction(m_zh);
        actgroup->setExclusive(true);
        m_languageMenu = new QMenu(this);
        m_languageMenu->addActions(actgroup->actions());
        m_languageMenu->installEventFilter(this);
        m_languageMenu->setStyleSheet("QMenu{background-color:#333333}");
        connect(m_languageMenu, SIGNAL(triggered(QAction*)), this, SLOT(onMenuItemClicked(QAction*)));

        if(m_greeter->lang() == "zh_CN")
            m_zh->setChecked(true);
        else
            m_en->setChecked(true);
    }
    m_languageMenu->popup(mapToGlobal(m_languageLB->geometry().bottomLeft()));
}

void GreeterWindow::onMenuItemClicked(QAction *action)
{
    QString text = action->text();
    if(text == tr("English")){
        m_languageLB->setText(tr("En"));
        m_greeter->setLang("en_US");
    } else if(text == tr("Chinese")) {
        m_languageLB->setText(tr("Zh"));
        m_greeter->setLang("zh_CN");
    }
}
