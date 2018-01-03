#ifndef TIPEDIT_H
#define TIPEDIT_H

#include <QLineEdit>

class TipEdit : public QLineEdit
{
    Q_OBJECT
    Q_PROPERTY(QString innerTip READ innerTip WRITE setInnerTip)
public:
    TipEdit(QWidget *parent=0);
    void paintEvent(QPaintEvent *);

    void drawTip();

    const QString& innerTip(){ return m_tip; }
    void setInnerTip(const QString &tip){ m_tip = tip;}

private:
    QString m_tip;
};

#endif // TIPEDIT_H
