#include "userentry.h"
#include <QPainter>
#include <QBrush>
#include <QFile>
#include <QApplication>
#include <QMouseEvent>
#include <QDebug>


UserEntry::UserEntry(const QString &facePath, const QString &name, bool isLogin, QWidget *parent)
    :QWidget(parent)//, m_face(facePath), m_name(name), m_login(isLogin)
{
    setupUi();
    m_faceLabel->installEventFilter(this);
    setFace(facePath);
    setUserName(name);
    setLogin(isLogin);
}

void UserEntry::setupUi()
{
    if (objectName().isEmpty())
        setObjectName(QString::fromUtf8("Entry"));
    setFixedSize(190, 240);     //不能用resize，否则放到layout中时，如果layout加有stretch，则该widget显示不出来
    m_faceLabel = new QLabel(this);
    m_faceLabel->setObjectName(QString::fromUtf8("m_faceLabel"));
    m_faceLabel->setGeometry(QRect(30, 30, 130, 130));
    m_faceLabel->setStyleSheet(QString::fromUtf8("border-color: rgb(255, 255, 255);"));
    m_faceLabel->setAlignment(Qt::AlignCenter);
    m_nameLabel = new QLabel(this);
    m_nameLabel->setObjectName(QString::fromUtf8("m_nameLabel"));
    m_nameLabel->setGeometry(QRect(30, 185, 130, 20));
    m_nameLabel->setAlignment(Qt::AlignCenter);
    m_loginLabel = new QLabel(this);
    m_loginLabel->setObjectName(QString::fromUtf8("m_loginLabel"));
    m_loginLabel->setGeometry(QRect(30, 210, 130, 20));
    m_loginLabel->setAlignment(Qt::AlignCenter);

    retranslateUi();
}

void UserEntry::retranslateUi()
{
    m_faceLabel->setPixmap(QPixmap("/usr/share/kylin-greeter/default_face.png"));
    m_faceLabel->setStyleSheet("QLabel{border:2px solid white}");

    QPalette plt;
    plt.setColor(QPalette::WindowText, Qt::black);

    m_nameLabel->setPalette(plt);
    m_loginLabel->setPalette(plt);
}

UserEntry::UserEntry(QWidget *parent) : UserEntry("", "", false, parent)
{
}

void UserEntry::paintEvent(QPaintEvent *event)
{

    if(this->hasFocus())
    {
        QPainter painter(this);
        QBrush brush;
        brush.setStyle(Qt::Dense6Pattern);
        QColor color(Qt::white);
        //color.setAlpha(0.7);
        brush.setColor(color);
        painter.setBrush(brush);
        painter.drawRect(QRect(23, 23, 144, 144));
    }
    return QWidget::paintEvent(event);
}

bool UserEntry::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == m_faceLabel){
        if(event->type() == QEvent::MouseButtonPress){
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if(mouseEvent->button() == Qt::LeftButton){
                this->setFocus();
                emit clicked(m_name);
                return true;
            }
        }
    }
    return QWidget::eventFilter(obj, event);
}

void UserEntry::onClicked()
{
    qDebug() << "clicked";
    this->setFocus();
    emit clicked(m_name);
}

void UserEntry::setFace(const QString &facePath)
{
    this->m_face = facePath;
    QFile faceFile(facePath);
    if(!faceFile.exists())
        this->m_face = ":/resource/default_face.png";
    QPixmap pixmap(this->m_face);
    pixmap = pixmap.scaled(128, 128, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
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
        this->m_loginLabel->setText(m_login ? "已登录" : "");
    }
}

