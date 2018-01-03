#include "tipedit.h"
#include <QPainter>
#include <QDebug>

TipEdit::TipEdit(QWidget *parent)
    :QLineEdit(parent)
{

}

void TipEdit::paintEvent(QPaintEvent *event)
{
    QLineEdit::paintEvent(event);
    if(text().length() == 0 && m_tip.length() > 0)
        drawTip();
}


void TipEdit::drawTip()
{

    QRect csRect = cursorRect();
    QRect tipRect(csRect.right(), rect().top(), rect().right(), rect().bottom());

    QPainter painter(this);
    painter.setPen(QColor("#888"));
    QTextOption option(Qt::AlignLeft | Qt::AlignVCenter);
    option.setWrapMode(QTextOption::WordWrap);

    painter.drawText(tipRect, m_tip, option);
}
