#ifndef BORDERDIALOG_H
#define BORDERDIALOG_H

#include <QWidget>

class QPushButton;
class BorderDialog : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int opacity READ opacity WRITE setOpacity)
public:
    explicit BorderDialog(QWidget *parent = 0);
    virtual void close();

    void setBorderWidth(int width = 24);
    void setBorderColor(QColor color = QColor(0, 0, 0, 50));
    void setDialogSize(int width, int h);
    void setDialogColor(QColor color = QColor("#2D698E"));

protected:
    void initUI();
    QWidget *center() const;

private:
    void startBackground(int begin, int end, bool show);
    int opacity();
    void setOpacity(int);
    void setWidgetColor(QWidget *widget, QColor color);


protected:
    void paintEvent(QPaintEvent *);
    void showEvent(QShowEvent *event);
    void mousePressEvent(QMouseEvent *event);

signals:
    void aboutToClose();

private:
    qreal       m_opacity;

    QWidget     *m_centerWidget;
    QWidget     *m_borderWidget;
    QPushButton *m_close;

    int borderWidth;
};

#endif // BORDERDIALOG_H
