#include "globalapplication.h"
#include <QMouseEvent>
#include <QPoint>
#include "globalv.h"
#include "screenmanager.h"

GlobalApplication::GlobalApplication(int argc, char **argv)
    : QApplication(argc, argv),
      m_screenManager(nullptr)
{

}

bool GlobalApplication::notify(QObject *obj, QEvent *event)
{
    if(event->type() == QEvent::MouseMove){
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
//        QDEBUG << "global mouse move";
        QPoint mp = mouseEvent->pos();
        if(this->m_screenManager){
            m_screenManager->onMouseMove(mp);
        }
    }
    return QApplication::notify(obj, event);
}

void GlobalApplication::setScreenManager(ScreenManager *sm)
{
    this->m_screenManager = sm;
}
