#ifndef USERENTRY_H
#define USERENTRY_H

#include <QWidget>
#include <QLabel>

#define SHADOW_WIDTH (scale > 0.5 ? 10 : 5)
#define FACE_WIDDTH (130 * scale)
#define IMG_WIDTH (128 * scale)
#define ENTRY_HEIGHT (FACE_WIDDTH + SHADOW_WIDTH * 2 + 45)
#define ENTRY_WIDTH (130 * scale + SHADOW_WIDTH * 2)

extern float scale;

class UserEntry : public QWidget
{
    Q_OBJECT
public:
    UserEntry(const QString &name, const QString &facePath, bool isLogin, QWidget *parent);
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
    void pressed();

private:
    QLabel *m_faceLabel;
    QLabel *m_nameLabel;
    QLabel *m_loginLabel;

    QString m_face;
    QString m_name;
    bool m_login;
    int id;
    static int count;
    static int selectedId;
};



#endif // USERENTRY_H

