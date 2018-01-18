#include "sessionwindow.h"
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include "globalv.h"
#include <QLightDM/SessionsModel>

class IconLabel : public QWidget
{
public:
    IconLabel(int width, int height, QWidget* parent = 0)
        : QWidget(parent),
          m_iconLabel(new QLabel(this)),
          m_textLabel(new QLabel(this)),
          m_checkLabel(new QLabel(this))
    {
        m_iconLabel->setGeometry(0, 0, height, height);
        m_textLabel->setGeometry(height+5, 0, width-height-5, height);
        m_textLabel->setStyleSheet("QLabel{border-radius:5; color: white}"
                              "QLabel::hover{ background-color: rgba(255, 255, 255, 0.3) }");
        m_checkLabel->setGeometry(m_textLabel->rect().right(), 0, height, height);
    }

    void setChecked(bool isChecked)
    {
        if(isChecked) {
            m_checkLabel->setPixmap(scaledPixmap(22, 16, ":/resource/checked.png"));
        } else {
            m_checkLabel->setPixmap(QPixmap());
        }
    }

    void setContent(const QPixmap& icon, const QString& text)
    {
        m_iconLabel->setPixmap(icon);
        m_textLabel->setText(text);
        m_textLabel->setFont(QFont("Ubutnu", 14));
        m_textLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    }

private:
    QLabel *m_iconLabel;
    QLabel *m_textLabel;
    QLabel *m_checkLabel;
};


SessionWindow::SessionWindow(QWidget *parent)
    : QWidget(parent),
      m_selectedIndex(0),
      m_sessionsModel(new QLightDM::SessionsModel(QLightDM::SessionsModel::LocalSessions))
{
    initUI();
}

void SessionWindow::initUI()
{
    if (objectName().isEmpty())
        setObjectName(QStringLiteral("this"));
    int sessionNum = m_sessionsModel->rowCount();
    int height = 55 + 30 * sessionNum + 20 * (sessionNum - 1);
    resize(405, height);
    QPalette plt;
    plt.setBrush(QPalette::Window, QBrush("#142D6F"));
    plt.setColor(QPalette::WindowText, Qt::white);
    setPalette(plt);

    m_backLabel = new QLabel(this);
    m_backLabel->setObjectName(QStringLiteral("m_backLabel"));
    m_backLabel->setGeometry(QRect(0, 5, 32, 32));
    m_backLabel->setPixmap(scaledPixmap(32, 32, ":/resource/arrow_left.png"));

    m_prompt = new QLabel(this);
    m_prompt->setObjectName(QStringLiteral("m_prompt"));
    m_prompt->setGeometry(QRect(52, 0, 350, 30));
    m_prompt->setFont(QFont("Ubuntu", 16));
    m_prompt->setText(tr("select the desktop environment"));//

    for(int i = 0; i < sessionNum; i++) {
        QString sessionName = m_sessionsModel->index(i, 0).data().toString();
        IconLabel *sessionLabel = new IconLabel(300, 30, this);
        sessionLabel->setObjectName("session"+QString::number(i));
        sessionLabel->setContent(getSessionIcon(sessionName), sessionName);
        sessionLabel->setGeometry(52, 55+ 30*i + 10*i, 300, 30);
        sessionLabel->installEventFilter(this);
    }
}

bool SessionWindow::eventFilter(QObject *obj, QEvent *event)
{
    if(obj->objectName().contains("session")){
        IconLabel *sessionLabel = findChild<IconLabel*>(obj->objectName());
        if(event->type() == QEvent::MouseButtonRelease){
            IconLabel *lastSessionLabel = findChild<IconLabel*>("session" + QString::number(m_selectedIndex));
            if(lastSessionLabel){
                lastSessionLabel->setChecked(false);
            }
            sessionLabel->setChecked(true);
            m_selectedIndex = obj->objectName().right(1).toInt();
        }
    }

    return QWidget::eventFilter(obj, event);
}

void SessionWindow::setSessionModel(QSharedPointer<QAbstractItemModel> model)
{
    if(model.isNull()) {
        return ;
    }
    m_sessionsModel = model;
    initUI();
}

QPixmap SessionWindow::getSessionIcon(const QString &sessionName)
{
    QString sessionIcon;
    if(sessionName.toLower() == "ubuntu") {
        sessionIcon = ":/resource/ubuntu_badge.png";
    } else if (sessionName.toLower() == "gnome") {
        sessionIcon = ":/resource/ubuntu_badge.png";
    } else if (sessionName.toLower() == "kde") {
        sessionIcon = ":/resource/ubuntu_badge.png";
    } else {
        sessionIcon = ":/resource/unknown_badge.png";
    }
    return scaledPixmap(22, 22, sessionIcon);
}
