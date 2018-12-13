#include "fakedialog.h"
#include <QPushButton>
#include <QPainter>
#include <QKeyEvent>
#include <QDebug>

#define BORDER_WIDTH 24

FakeDialog::FakeDialog(QWidget *parent) : QWidget(parent)
{
    initUI();
}

void FakeDialog::setDialogSize(int w, int h)
{
    dialogWidth = w;
    dialogHeight = h;

    resizeEvent(nullptr);
}

QWidget* FakeDialog::centerWidget()
{
    return m_centerWidget;
}

QWidget* FakeDialog::dialog()
{
    return m_dialog;
}

void FakeDialog::initUI()
{
    setObjectName("fakeDialogBackground");
    setAttribute(Qt::WA_TranslucentBackground, true);
    setGeometry(parentWidget()->rect());

    m_dialog = new QWidget(this);
    m_dialog->setObjectName(QStringLiteral("fakeDialog"));

    m_centerWidget = new QWidget(m_dialog);
    m_centerWidget->setObjectName(QStringLiteral("dialogCenterWidget"));

    m_closeButton = new QPushButton(m_dialog);
    m_closeButton->setObjectName(QStringLiteral("closeButton"));
    connect(m_closeButton, &QPushButton::clicked, this, &FakeDialog::close);
}

void FakeDialog::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.setPen(Qt::transparent);
    painter.setBrush(QColor(0, 0, 0, 100));
    painter.drawRect(rect());

    QWidget::paintEvent(e);
}

void FakeDialog::resizeEvent(QResizeEvent */*event*/)
{
    m_dialog->setGeometry((width() - dialogWidth) / 2,
                          (height() - dialogHeight) / 2,
                          dialogWidth, dialogHeight);
    m_centerWidget->setGeometry(BORDER_WIDTH, BORDER_WIDTH,
                                m_dialog->width() - BORDER_WIDTH * 2,
                                m_dialog->height() - BORDER_WIDTH * 2);
    m_closeButton->setGeometry(m_dialog->width() - BORDER_WIDTH, 0,
                               BORDER_WIDTH, BORDER_WIDTH);
}

void FakeDialog::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Escape)
    {
        close();
    }
}
