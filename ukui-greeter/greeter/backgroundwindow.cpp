#include "backgroundwindow.h"
#include <QPainter>
#include <QTimer>
#include <QDebug>
#include "globalv.h"
#include <QMouseEvent>

QT_BEGIN_NAMESPACE
extern void qt_blurImage(QPainter *p, QImage &blurImage, qreal radius, bool quality, bool alphaOnly, int transposed = 0);
QT_END_NAMESPACE

#define BLUR_RADIUS 300

QPixmap * blurPixmap(QPixmap *pixmap)
{
    QPainter painter(pixmap);
    QImage srcImg = pixmap->toImage();
    qt_blurImage(&painter, srcImg, BLUR_RADIUS, false, false);
    painter.end();
    return pixmap;
}

BackGroundWindow::BackGroundWindow(QWidget *parent)
    : QWidget(parent),
      m_timer(nullptr)
{
    m_Screen = nullptr;
    m_backgroundMode = DRAW_USER_BACKGROUND;
    m_configuration = Configuration::instance();
    m_background = nullptr;
    //setWindowFlags(Qt::X11BypassWindowManagerHint);
    setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnBottomHint);
    init();

    m_timer = new QTimer(this);
    m_transition.started = false;

    //setWindowTitle("baackground title");
    connect(m_timer, &QTimer::timeout, this, &BackGroundWindow::onTransition);
    connect(m_Screen,&QScreen::geometryChanged,this,&BackGroundWindow::slotSizeChanged);
}

BackGroundWindow::~BackGroundWindow()
{
    if(m_timer != nullptr) {
        delete m_timer;
        m_timer = nullptr;
    }
    m_backgrounds.clear();
}


void BackGroundWindow::init()
{
    //背景图片 优先级：用户桌面背景、背景图片、背景颜色
    bool drawUserBackground = m_configuration->getValue("draw-user-background").toBool();
    if(drawUserBackground) {
        m_backgroundMode = DRAW_USER_BACKGROUND;
    } else {
        m_background = QSharedPointer<Background>(new Background);
        QString backgroundPath = m_configuration->getValue("background").toString();
        if(!backgroundPath.isEmpty()) {
            m_backgroundMode = DRAW_BACKGROUND;

            m_background->type = BACKGROUND_IMAGE;
            m_background->image = backgroundPath;
        } else {
            QString color = m_configuration->getValue("background-color").toString();
            if(!color.isEmpty()) {
                m_backgroundMode = DRAW_COLOR;

                m_background->type = BACKGROUND_COLOR;
                m_background->color = color;
            } else {
                m_backgroundMode = DRAW_DEFAULT;

                m_background->type = BACKGROUND_IMAGE;
                m_background->image = m_configuration->getDefaultBackgroundName();
            }
        }
    }
    qDebug() << "default background:" << m_configuration->getDefaultBackgroundName();
    if(m_background)
        qDebug() << m_background->type << m_background->image;
}

void BackGroundWindow::setScreen(QScreen *screen)
{
    if(screen == nullptr)
        qWarning()<<"BackGroundWindow::screen is a null pointer!";
    m_Screen = screen;
    setGeometry(screen->geometry());
    show();
    update();
}

QScreen* BackGroundWindow::getScreen()
{
    return m_Screen;
}

void BackGroundWindow::setBackground(QSharedPointer<Background> &background)
{
    if(background)
        qDebug() <<"BackGroundWindow::setBackground" <<background->image;

    if(m_backgroundMode != DRAW_USER_BACKGROUND)
        return;

    stopTransition();

    if(background) {
        if(background->image.isEmpty())
            background->image = m_configuration->getDefaultBackgroundName();
    }

    if(m_background && background &&
       m_background->image == background->image){
        repaint();
        return;
    }

    //如果是第一次绘制背景，则不需要渐变
    //现在由于绘制模糊背景，不需要渐变了
    if(!m_background.isNull())
        startTransition(m_background, background);

    m_background = background;
    update();
}

QSharedPointer<Background> BackGroundWindow::getBackground()
{
    return m_background;
}

void BackGroundWindow::startTransition(QSharedPointer<Background> &from,
                                 QSharedPointer<Background> &to)
{
    stopTransition();

    m_transition.from = from;
    m_transition.to = to;
    m_transition.stage = 0.0;
    m_transition.started = true;

    m_timer->start(20);
}

void BackGroundWindow::drawTransitionAlpha(const QRect &rect)
{
    drawBackground(m_transition.from, rect, 1.0 - m_transition.stage);
    drawBackground(m_transition.to, rect, m_transition.stage);
}

void BackGroundWindow::paintEvent(QPaintEvent *e)
{
    if(!m_Screen) {
        qDebug()<<"background window mScreen is null!";
        return QWidget::paintEvent(e);
    }
    //在屏幕上绘制背景
    //QRect rect = m_Screen->geometry();
    QRect rect(0,0,m_Screen->geometry().width(),m_Screen->geometry().height());
    if(m_transition.started)
        drawTransitionAlpha(rect);
    else {
        drawBackground(m_background, rect);
    }

    return QWidget::paintEvent(e);
}

void BackGroundWindow::resizeEvent(QResizeEvent *event)
{
    repaint();
}

void BackGroundWindow::onTransition()
{
    m_transition.stage += 0.1;//= (1 - cos(M_PI * m_transition.stage)) / 2;

    if(m_transition.stage >= 1.0)
        stopTransition();

    repaint();
    //update();
}

void BackGroundWindow::slotSizeChanged(const QRect &geometry)
{
    setGeometry(geometry);
    update();
}

void BackGroundWindow::stopTransition()
{
    if(m_timer && m_timer->isActive())
        m_timer->stop();
    m_transition.stage = 1.0;
    m_transition.started = false;
}

void BackGroundWindow::drawBackground(QSharedPointer<Background> &background, const QRect &rect, float alpha)
{
    if(background == nullptr)
        return;

    QPainter painter(this);
    painter.setOpacity(alpha);

    switch(background->type) {
    case BACKGROUND_IMAGE:
    {

        QPixmap *pixmap = getBackground(background->image, rect);

        if(pixmap->isNull())
        {
            QString color = m_configuration->getValue("background-color").toString();
            QColor cor;
            if(!color.isEmpty())
                cor = color;
            else
                cor = "#035290";
            painter.setBrush(cor);
            painter.drawRect(rect);
        }
        else
        {
            painter.drawPixmap(rect, *pixmap);
        }
        break;
    }
    case BACKGROUND_COLOR:
    {
        painter.setBrush(background->color);
        painter.drawRect(rect);
        break;
    }
    }

}

QPixmap *BackGroundWindow::getBackground(const QString &path, const QRect &rect)
{
    QString resolution = QString("%1x%2").arg(rect.width()).arg(rect.height());
    QPair<QString, QString> key(path, resolution);

    if(!m_backgrounds.contains(key)){
        QPixmap *pixmap =  new QPixmap(scaledPixmap(width(), height()+2, path));
        m_backgrounds[key] = blurPixmap(pixmap);
    }
    return m_backgrounds[key];
}
