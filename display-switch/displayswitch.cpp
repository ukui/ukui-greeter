#include "displayswitch.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QDebug>
#include <QDesktopWidget>
#include "displayservice.h"
#include "keyeventmonitor.h"

DisplaySwitch::DisplaySwitch(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow()),
      currentMode(0),
      keyEventMonitor(new KeyEventMonitor(this))
{
    keyEventMonitor->start();
    connect(keyEventMonitor, &KeyEventMonitor::displaySwitchSelect, this, &DisplaySwitch::onDisplaySwitchSelect);
    connect(keyEventMonitor, &KeyEventMonitor::displaySwitchConfirm, this, &DisplaySwitch::onDisplaySwitchConfirm);

    ui->setupUi(this);
    this->setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint | Qt::ToolTip);
    initUi();
}

void DisplaySwitch::initUi()
{
    ui->centralWidget->setAutoFillBackground(true);
    QPalette pc;
    QPixmap img_bg = QPixmap(":res/bg.png");
    pc.setBrush(QPalette::Window, QBrush(img_bg));
    ui->centralWidget->setPalette(pc);

    ui->lb_mode_1->setText("仅计算机");
    ui->lb_mode_2->setText("复制");
    ui->lb_mode_3->setText("扩展");
    ui->lb_mode_4->setText("仅投影仪");

    ui->mode_1->setFocusPolicy(Qt::NoFocus);
    ui->mode_2->setFocusPolicy(Qt::NoFocus);
    ui->mode_3->setFocusPolicy(Qt::NoFocus);
    ui->mode_4->setFocusPolicy(Qt::NoFocus);

    QDesktopWidget *dw = QApplication::desktop();
    QRect rect = dw->screen(dw->primaryScreen())->geometry();
    setGeometry(rect.left() + (rect.width() - width()) / 2,
                rect.top() + (rect.height() - height()) / 2,
                width(), height());
}

void DisplaySwitch::onDisplaySwitchSelect(/*const QRect &rect*/)
{
    show();

    ui->mode_1->hide();
    ui->mode_2->hide();
    ui->mode_3->hide();
    ui->mode_4->hide();
    currentMode++;
    if(currentMode > 4)
        currentMode = 1;
    switch(currentMode) {
    case 1:
        ui->mode_1->show();
        break;
    case 2:
        ui->mode_2->show();
        break;
    case 3:
        ui->mode_3->show();
        break;
    case 4:
        ui->mode_4->show();
        break;
    }
}

void DisplaySwitch::onDisplaySwitchConfirm()
{
    if(!isHidden()) {
        hide();
        DisplayService().switchDisplayMode(DisplayMode(currentMode));
        currentMode--;
    }
}

void DisplaySwitch::onPositionChanged(const QRect &rect)
{
    move(rect.left() + (rect.width() - width()) / 2,
               rect.top() + (rect.height() - height()) / 2);
}
