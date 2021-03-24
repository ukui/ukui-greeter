#ifndef BACKGROUNDWINDOW_H
#define BACKGROUNDWINDOW_H

#include <QWidget>
#include <QSharedPointer>

#include <QScreen>
#include "common/configuration.h"

enum BackgroundMode
{
    DRAW_USER_BACKGROUND,
    DRAW_BACKGROUND,
    DRAW_COLOR,
    DRAW_DEFAULT
};

enum BackgroundType
{
    BACKGROUND_IMAGE,
    BACKGROUND_COLOR
};

struct Background
{
    BackgroundType type;
    QColor color;
    QString image;
};

struct Transition
{
    QSharedPointer<Background>  from;
    QSharedPointer<Background>  to;
    float       stage;
    bool        started;
};


//none-top level window
class BackGroundWindow : public QWidget
{
    Q_OBJECT
public:
    explicit BackGroundWindow(QWidget *parent = nullptr);
    ~BackGroundWindow();

    //get default m_backgroundMode,m_background
    void init();

    void setScreen(QScreen *screen = nullptr);
    QScreen* getScreen();

    void setBackground(QSharedPointer<Background> &);
    QSharedPointer<Background> getBackground();

protected:
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *event);

signals:
    void signalWidgetEntered();

private slots:
    void onTransition();
    void slotSizeChanged(const QRect &geometry);



private:
    void stopTransition();
    void startTransition(QSharedPointer<Background> &, QSharedPointer<Background> &);
    void drawTransitionAlpha(const QRect &rect);

    void drawBackground(QSharedPointer<Background> &, const QRect &, float alpha = 1.0);
    QPixmap * getBackground(const QString &path, const QRect &rect);
    //对每张背景图片的不同分辨率进行缓存，减少CPU占用率（这里分辨率格式：1080x960）
    QMap<QPair<QString, QString>, QPixmap*>   m_backgrounds;

    QScreen* m_Screen;
    QSharedPointer<Background> m_background;
    BackgroundMode   m_backgroundMode;
    Configuration   *m_configuration;
    QTimer          *m_timer;
    Transition       m_transition;
};

#endif // BACKGROUNDWINDOW_H
