#include "userentry.h"
#include <QPainter>
#include <QBrush>
#include <QFile>
#include <QApplication>
#include <QMouseEvent>
#include <QVBoxLayout>
#include <QDebug>
#include "globalv.h"


UserEntry::UserEntry(const QString &name, const QString &facePath, bool isLogin, QWidget *parent)
    : QWidget(parent),
      m_selected(false)
{
    initUI();

    m_faceLabel->installEventFilter(this);
    setFace(facePath);
    setUserName(name);
    setLogin(isLogin);
}

UserEntry::UserEntry(QWidget *parent) : UserEntry("", "", false, parent)
{
}

void UserEntry::initUI()
{
    if (objectName().isEmpty())
        setObjectName(QString::fromUtf8("Entry"));   
    setFixedSize(190*scale, 130*scale+70);//不能用resize，否则放到layout中时，如果layout加有stretch，则该widget显示不出来
    m_faceLabel = new QLabel(this);
    m_faceLabel->setObjectName(QString::fromUtf8("m_faceLabel"));
    QRect faceRect(30*scale, 10, 130*scale, 130*scale);
    m_faceLabel->setGeometry(faceRect);
    m_faceLabel->setStyleSheet("QLabel{border:2px solid white}");
//    m_faceLabel->setAlignment(Qt::AlignCenter);

    m_nameLabel = new QLabel(this);
    m_nameLabel->setObjectName(QString::fromUtf8("m_nameLabel"));
    QRect nameRect(30*scale, faceRect.bottom()+12, 130*scale, 20);
    m_nameLabel->setGeometry(nameRect);
    m_nameLabel->setAlignment(Qt::AlignCenter);
    m_nameLabel->setFont(QFont("Ubuntu", fontSize));

    m_loginLabel = new QLabel(this);
    m_loginLabel->setObjectName(QString::fromUtf8("m_loginLabel"));
    QRect loginRect(30*scale, nameRect.bottom()+5, 130*scale, 20);
    m_loginLabel->setGeometry(loginRect);
    m_loginLabel->setAlignment(Qt::AlignCenter);
    m_loginLabel->setFont(QFont("Ubuntu", fontSize));

    QPalette plt;
    plt.setColor(QPalette::WindowText, Qt::white);

    m_nameLabel->setPalette(plt);
    m_loginLabel->setPalette(plt);
}

void UserEntry::paintEvent(QPaintEvent *event)
{
    if(this->selected())    //绘制边框
    {
        QPainter painter(this);
        QRect rect = m_faceLabel->geometry();
        QRect border(rect.left()-10, rect.top()-10, rect.width()+20, rect.height()+20);
        painter.fillRect(border, QColor(255, 255, 255, 30));
    }
    return QWidget::paintEvent(event);
}

bool UserEntry::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == m_faceLabel){
        if(event->type() == QEvent::MouseButtonPress){
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if(mouseEvent->button() == Qt::LeftButton){
//                this->setFocus();
                this->setSelected();
                return true;
            }
        }
        if(event->type() == QEvent::MouseButtonRelease){
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if(mouseEvent->button() == Qt::LeftButton){
                emit clicked(m_name);
                return true;
            }
        }
    }
    return QWidget::eventFilter(obj, event);
}

void UserEntry::onClicked()
{
//    this->setFocus();
    this->setSelected();
    emit clicked(m_name);
}

void UserEntry::setFace(const QString &facePath)
{
    this->m_face = facePath;
    QFile faceFile(facePath);
    if(!faceFile.exists())
        this->m_face = ":/resource/default_face.png";
    QPixmap pixmap(this->m_face);
    pixmap = pixmap.scaled(128*scale, 128*scale, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    this->m_faceLabel->setPixmap(pixmap);
}

const QString& UserEntry::userName()
{
    return this->m_name;
}

void UserEntry::setUserName(const QString &name)
{
    if(m_name != name)
    {
        m_name = name;
        this->m_nameLabel->setText(m_name);
    }
}

void UserEntry::setLogin(bool isLogin)
{
    if(m_login != isLogin)
    {
        m_login = isLogin;
        this->m_loginLabel->setText(m_login ? tr("logged in") : "");
    }
}

void UserEntry::setSelected(bool selected)
{
    this->m_selected = selected;
    update();
}

bool UserEntry::selected()
{
    return this->m_selected;
}
