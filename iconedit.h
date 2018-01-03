#ifndef ICONEDIT_H
#define ICONEDIT_H

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QPixmap>
#include "tipedit.h"

class IconButton : public QPushButton
{
    Q_OBJECT
public:
    IconButton(QLineEdit *edit);
    IconButton(QLineEdit *edit, const QIcon &icon);

    void resize(const QSize &size);

private:
    QSize m_size;
};

class IconEdit : public QWidget
{
    Q_OBJECT
public:
    IconEdit(QWidget *parent = 0);
    IconEdit(const QIcon& icon, QWidget *parent = 0);

    virtual void keyReleaseEvent ( QKeyEvent * event );

    void setIcon(const QString &filename);
    void setIcon(const QIcon &icon);
    void resize(const QSize& size);
    void clear();
    void setPrompt(const QString &);
    const QString& text();

signals:
    void clicked(const QString &);

public slots:
    void clicked_cb();

private:

    TipEdit      *m_edit;
    IconButton   *m_iconButton;
};

#endif // ICONEDIT_H
