#ifndef USERENTRY_H
#define USERENTRY_H

#include <QWidget>
#include <QLabel>


class UserEntry : public QWidget
{
    Q_OBJECT
public:
    UserEntry(const QString &facePath, const QString &name, bool isLogin, QWidget *parent);
    UserEntry(QWidget *parent);

    void setFace(const QString &facePath);
    const QString& userName();
    void setUserName(const QString &name);
    void setLogin(bool isLogin);
    static int X(){return 190;}
    static int Y(){return 240;}

protected:
    void paintEvent(QPaintEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);


private:
    void setupUi();
    void retranslateUi();
    void onClicked();

signals:
    void clicked(const QString&);

private:
    QLabel *m_faceLabel;
    QLabel *m_nameLabel;
    QLabel *m_loginLabel;

    QString m_face;
    QString m_name;
    bool m_login;
};





#endif // USERENTRY_H

