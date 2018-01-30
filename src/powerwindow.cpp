#include "powerwindow.h"
#include <QLabel>
#include <QPainter>
#include <QApplication>
#include <QDesktopWidget>
#include <QMouseEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QImage>
#include <QDebug>
#include <QException>
#include "globalv.h"
#include "exponentialblur.h"

PowerWindow::PowerWindow(bool hasOpenSessions, QWidget *parent)
    : QWidget(parent),
      m_hasOpenSessions(hasOpenSessions),
      m_power(new QLightDM::PowerInterface(this))
{
    initUI();
    QDesktopWidget *dw = QApplication::desktop();
    setGeometry((dw->width()-width())/2, (dw->height()-height())/2,
                width(), height());
}

void PowerWindow::initUI()
{
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground, true);

    this->resize(643, 290);

    m_centerWidget = new QWidget(this);
    m_centerWidget->setObjectName(QStringLiteral("m_centerWidget"));

    QVBoxLayout *vbox = new QVBoxLayout(m_centerWidget);
    vbox->setContentsMargins(20, 10, 20, 2);
    QString text = tr("Goodbye. Would you like to…");

    QPalette plt;
    plt.setColor(QPalette::WindowText, Qt::white);
    if(m_hasOpenSessions) {
        QString text2 = tr("Other users are currently logged in to this computer, "
                   "shutting down now will also close these other sessions.");
        text = QString("%1\n\n%2").arg(text2).arg(text);
        this->setFixedHeight(330);
    }

    m_centerWidget->setGeometry(QRect(24, 24, 594, height()-24*2));

    m_prompt = new QLabel(m_centerWidget);
    m_prompt->adjustSize();
    m_prompt->setText(text);
    m_prompt->setWordWrap(true);
    m_prompt->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    m_prompt->setPalette(plt);
    m_prompt->setFont(QFont("Ubuntu Light 12"));

    vbox->addWidget(m_prompt);

    QHBoxLayout *hbox = new QHBoxLayout();
    hbox->setSpacing(20);

    QVBoxLayout *vbox1 = new QVBoxLayout();
    if(m_power->canSuspend()){
        m_suspend = new QLabel(m_centerWidget);
        m_suspend->setFixedSize(168, 168);
        m_suspend->setObjectName(QStringLiteral("m_suspend"));
        m_suspend->setStyleSheet("QLabel{background:url(:/resource/hibernate.png); background-repeat:non-repeat}"
                                 "QLabel::hover{background:url(:/resource/hibernate_highlight.png); background-repeat:non-repeat}");
        m_suspend->installEventFilter(this);
        m_suspendLabel = new QLabel(m_centerWidget);
        m_suspendLabel->setAlignment(Qt::AlignCenter);
        m_suspendLabel->setFixedSize(168, 30);
        m_suspendLabel->setPalette(plt);
//        m_suspendLabel->hide();
        vbox1->addWidget(m_suspend);
        vbox1->addWidget(m_suspendLabel);
    } else if(m_power->canHibernate()) {
        m_hibernate = new QLabel(m_centerWidget);
        m_hibernate->setFixedSize(168, 168);
        m_hibernate->setObjectName(QStringLiteral("m_hibernate"));
        m_hibernate->setStyleSheet("QLabel{background:url(:/resource/hibernate.png); background-repeat:non-repeat}"
                                 "QLabel::hover{background:url(:/resource/hibernate_highlight.png); background-repeat:non-repeat}");
        m_hibernate->installEventFilter(this);
        m_hibernateLabel = new QLabel(m_centerWidget);
        m_hibernateLabel->setAlignment(Qt::AlignCenter);
        m_hibernateLabel->setFixedSize(168, 30);
        m_hibernateLabel->setPalette(plt);
//        m_hibernateLabel->hide();
        vbox1->addWidget(m_hibernate);
        vbox1->addWidget(m_hibernateLabel);
    }

    QVBoxLayout *vbox2 = new QVBoxLayout();
    m_restart = new QLabel(m_centerWidget);
    m_restart->setFixedSize(168, 168);
    m_restart->setObjectName(QStringLiteral("m_restart"));
    m_restart->setStyleSheet("QLabel{background:url(:/resource/restart.png); background-repeat:non-repeat}"
                             "QLabel::hover{background:url(:/resource/restart_highlight.png); background-repeat:non-repeat}");
    m_restart->installEventFilter(this);
    m_restartLabel = new QLabel(m_centerWidget);
    m_restartLabel->setAlignment(Qt::AlignCenter);
    m_restartLabel->setFixedSize(168, 30);
    m_restartLabel->setPalette(plt);
//    m_restartLabel->hide();
    vbox2->addWidget(m_restart);
    vbox2->addWidget(m_restartLabel);

    QVBoxLayout *vbox3 = new QVBoxLayout();
    m_shutdown = new QLabel(m_centerWidget);
    m_shutdown->setFixedSize(168, 168);
    m_shutdown->setObjectName(QStringLiteral("m_shutdown"));
    m_shutdown->setStyleSheet("QLabel{background:url(:/resource/shutdown.png); background-repeat:non-repeat}"
                              "QLabel::hover{background:url(:/resource/shutdown_highlight.png); background-repeat:non-repeat}");
    m_shutdown->installEventFilter(this);
    m_shutdownLabel = new QLabel(m_centerWidget);
    m_shutdownLabel->setAlignment(Qt::AlignCenter);
    m_shutdownLabel->setFixedSize(168, 30);
    m_shutdownLabel->setPalette(plt);
//    m_shutdownLabel->hide();
    vbox3->addWidget(m_shutdown);
    vbox3->addWidget(m_shutdownLabel);

    hbox->addLayout(vbox1);
    hbox->addLayout(vbox2);
    hbox->addLayout(vbox3);

    vbox->addLayout(hbox);
    vbox->addStretch();

    m_close = new QLabel(this);
    m_close->setObjectName(QStringLiteral("m_close"));
    m_close->setGeometry(QRect(0, 0, 24, 24));
    m_close->setStyleSheet("QLabel{background:url(:/resource/dialog_close.png); background-repeat:non-repeat}"
                           "QLabel::hover{background:url(:/resource/dialog_close_highlight.png); background-repeat:non-repeat}");
    m_close->installEventFilter(this);
}

void PowerWindow::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.setBrush(QColor(180, 180, 250, 30));
    painter.drawRect(rect());

    QRect center = QRect(24, 24, width()-24 * 2, height()-24*2);
    QImage image(width()-24 * 2, height()-24*2, QImage::Format_ARGB32);
    image.fill(QColor(46, 39, 101, 200));
//    painter.setBrush(QColor(46, 39, 101, 200));
//    painter.drawRect(QRect(24, 24, width()-24 * 2, height()-24*2));
    ExponentialBlur().translate(image, 10);
    QPixmap pixmap;
    pixmap = pixmap.fromImage(image);
    painter.drawImage(center, image, image.rect());
//    painter.drawPixmap(center, pixmap);
    QWidget::paintEvent(e);
}

bool PowerWindow::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == m_close) {
        if(event->type() == QEvent::MouseButtonPress) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if(mouseEvent->button() == Qt::LeftButton) {
                m_close->setStyleSheet("QLabel{background:url(:/resource/dialog_close_press.png)}");
                return true;
            }
        }
        if(event->type() == QEvent::MouseButtonRelease) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if(mouseEvent->button() == Qt::LeftButton) {
                m_close->setStyleSheet("QLabel{background:url(:/resource/dialog_close.png)}"
                                       "QLabel::hover{background:url(:/resource/dialog_close_highlight.png)}");
                this->close();
                return true;
            }
        }
    } else if(obj == m_suspend) {
        if(event->type() == QEvent::Enter) {
            m_suspendLabel->setText(tr("suspend"));
        } else if(event->type() == QEvent::Leave) {
            m_suspendLabel->setText("");
        } else if(event->type() == QEvent::MouseButtonRelease){
            qDebug() << "suspend";
            try{
                m_power->suspend();
                close();
            }catch(QException &e) {

            }
        }
    }else if(obj == m_hibernate) {
        if(event->type() == QEvent::Enter) {
            m_hibernateLabel->setText(tr("hibernate"));
        } else if(event->type() == QEvent::Leave) {
            m_hibernateLabel->setText("");
        } else if(event->type() == QEvent::MouseButtonRelease){
            qDebug() << "hibernate";
            try{
                m_power->hibernate();
                close();
            }catch(QException &e) {

            }
        }
    } else if(obj == m_restart) {
        if(event->type() == QEvent::Enter) {
            m_restartLabel->setText(tr("restart"));
        }else if(event->type() == QEvent::Leave) {
            m_restartLabel->setText("");
        }else if(event->type() == QEvent::MouseButtonRelease){
            qDebug() << "restart";
            try{
                m_power->restart();
                close();
            }catch(QException &e) {

            }
        }
    } else if(obj == m_shutdown) {
        if(event->type() == QEvent::Enter) {
            m_shutdownLabel->setText(tr("shutdown"));
        }else if(event->type() == QEvent::Leave) {
            m_shutdownLabel->setText("");
        }else if(event->type() == QEvent::MouseButtonRelease){
            try{
                qDebug() << "shutdown";
                m_power->shutdown();
                close();
            }catch(QException &e) {

            }
        }
    }
    return QWidget::eventFilter(obj, event);
}
