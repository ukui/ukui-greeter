#ifndef USERENTRY_H
#define USERENTRY_H

#include <QWidget>
#include <QLabel>


class UserEntry : public QWidget
{
    Q_OBJECT
public:
    UserEntry(const QString &facePath, const QString &name, bool isLogin, QWidget *parent);
    UserEntry(QWidget *parent=0);

    void setFace(const QString &facePath);
    const QString& userName();
    void setUserName(const QString &name);
    void setLogin(bool isLogin);
    void setSelected(bool selected=true);
    bool selected();

protected:
    void paintEvent(QPaintEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);
    void resizeEvent(QResizeEvent *);

private:
    void onClicked();
    void initUI();

signals:
    void clicked(const QString&);

private:
    QLabel *m_faceLabel;
    QLabel *m_nameLabel;
    QLabel *m_loginLabel;

    QString m_face;
    QString m_name;
    bool m_login;
    bool m_selected;
};





#endif // USERENTRY_H

