#include "sessionwindow.h"
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QSvgWidget>
#include <QDebug>
#include <QLightDM/SessionsModel>
#include "globalv.h"

class IconLabel : public QWidget
{
public:
    IconLabel(int width, int height, QWidget* parent = 0)
        : QWidget(parent),
          m_iconLabel(new QSvgWidget(this)),
          m_textLabel(new QLabel(this)),
          m_checkLabel(new QLabel(this)),
          _isChecked(false)
    {
        m_iconLabel->setGeometry(0, (height-22)/2, 22, 22);
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
        _isChecked = isChecked;
    }

    bool checked()
    {
        return _isChecked;
    }

    void setContent(const QString& icon, const QString& text)
    {
        m_iconLabel->load(icon);
        m_textLabel->setText(text);
        m_textLabel->setFont(QFont("Ubutnu", 14));
        m_textLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    }

private:
    QSvgWidget *m_iconLabel;
    QLabel     *m_textLabel;
    QLabel     *m_checkLabel;
    bool       _isChecked;
};


SessionWindow::SessionWindow(QWidget *parent)
    : QWidget(parent),
      m_sessionsModel(new QLightDM::SessionsModel(QLightDM::SessionsModel::LocalSessions)),
      m_selectedIndex(-1)

{
    initUI();
}

void SessionWindow::initUI()
{
    if (objectName().isEmpty())
        setObjectName(QStringLiteral("this"));
    int sessionNum = m_sessionsModel->rowCount();
    int height = 55 + 30 * sessionNum + 20 * (sessionNum - 1);
    resize(520, height);
    QPalette plt;
    plt.setBrush(QPalette::Window, QBrush("#142D6F"));
    plt.setColor(QPalette::WindowText, Qt::white);
    setPalette(plt);

    m_backLabel = new QLabel(this);
    m_backLabel->setObjectName(QStringLiteral("m_backLabel"));
    m_backLabel->setGeometry(QRect(0, 0, 32, 32));
    m_backLabel->setPixmap(scaledPixmap(32, 32, ":/resource/arrow_left.png"));
    m_backLabel->installEventFilter(this);

    m_prompt = new QLabel(this);
    m_prompt->setObjectName(QStringLiteral("m_prompt"));
    m_prompt->setGeometry(QRect(220, 0, 350, 30));
    m_prompt->setFont(QFont("Ubuntu", 16));
    m_prompt->setText(tr("select the desktop environment"));

    for(int i = 0; i < sessionNum; i++) {
        QString sessionName = m_sessionsModel->index(i, 0).data().toString();
        IconLabel *sessionLabel = new IconLabel(300, 30, this);
        sessionLabel->setObjectName("session"+QString::number(i));
        sessionLabel->setContent(getSessionIcon(sessionName), sessionName);
        sessionLabel->setGeometry(220, 55+ 30*i + 10*i, 300, 30);
        sessionLabel->installEventFilter(this);
    }
}

bool SessionWindow::eventFilter(QObject *obj, QEvent *event)
{
    if(obj->objectName().contains("session")){
        IconLabel *sessionLabel = findChild<IconLabel*>(obj->objectName());
        if(event->type() == QEvent::MouseButtonPress){
            if(((QMouseEvent*)event)->button() == Qt::LeftButton){
                IconLabel *lastSessionLabel = findChild<IconLabel*>("session" + QString::number(m_selectedIndex));
                if(lastSessionLabel){
                    lastSessionLabel->setChecked(!lastSessionLabel->checked());
                }
                if(lastSessionLabel != obj) {
                    sessionLabel->setChecked(!sessionLabel->checked());
                }
                m_selectedIndex = sessionLabel->checked() ? obj->objectName().right(1).toInt() : -1;
                return true;
            }
        } else if(event->type() == QEvent::MouseButtonRelease){
            if(((QMouseEvent*)event)->button() == Qt::LeftButton){
                if(m_selectedIndex != -1)
                    saveAndBack();
                return true;
            }
        }
    }
    if(obj == m_backLabel) {
        if(event->type() == QEvent::MouseButtonPress) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if(mouseEvent->button() == Qt::LeftButton) {
                m_backLabel->setPixmap(scaledPixmap(32, 32, ":/resource/arrow_left_active.png"));
                return true;
            }
        }
        if(event->type() == QEvent::MouseButtonRelease) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if(mouseEvent->button() == Qt::LeftButton) {
                m_backLabel->setPixmap(scaledPixmap(32, 32, ":/resource/arrow_left.png"));

                saveAndBack();
                return true;
            }
        }
    }

    return QWidget::eventFilter(obj, event);
}

void SessionWindow::saveAndBack()
{
    IconLabel *sessionLabel = findChild<IconLabel*>("session" + QString::number(m_selectedIndex));
    if(sessionLabel)
        sessionLabel->setChecked(false);
    emit sessionSelected(m_sessionsModel->index(m_selectedIndex, 0).data().toString());
    emit back();
}

void SessionWindow::setSessionModel(QSharedPointer<QAbstractItemModel> model)
{
    if(model.isNull()) {
        return ;
    }
    m_sessionsModel = model;
    initUI();
}

void SessionWindow::setSession(const QString &sessionName)
{
    for(int i = 0; i < m_sessionsModel->rowCount(); i++) {
        QString session = m_sessionsModel->index(i, 0).data().toString();
        if(session.toLower() == sessionName.toLower()) {
            IconLabel *sessionLabel = findChild<IconLabel*>("session" + QString::number(i));
            sessionLabel->setChecked(true);
            m_selectedIndex = m_origIndex = i;
        }
    }
}

QString SessionWindow::getSessionIcon(const QString &session)
{
    QString sessionIcon;
    sessionIcon = IMAGE_DIR + QString("badges/%1_badge-symbolic.svg").arg(session.toLower());
    return sessionIcon;
}
