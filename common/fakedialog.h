#ifndef FAKEDIALOG_H
#define FAKEDIALOG_H

#include <QWidget>

class QPushButton;

class FakeDialog : public QWidget
{
    Q_OBJECT
public:
    explicit FakeDialog(QWidget *parent = nullptr);
    void setDialogSize(int w, int h);
    QWidget *centerWidget();
    QWidget *dialog();

protected:
    void paintEvent(QPaintEvent *e);
    void resizeEvent(QResizeEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

private:
    void initUI();

private:
    int dialogWidth;
    int dialogHeight;

    QWidget         *m_dialog;
    QWidget         *m_centerWidget;
    QPushButton     *m_closeButton;
//    QPushButton     *m_okButton;
//    QPushButton     *m_cancelButton;
};

#endif // FAKEDIALOG_H
