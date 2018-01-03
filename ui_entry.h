/********************************************************************************
** Form generated from reading UI file 'entry.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ENTRY_H
#define UI_ENTRY_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Entry
{
public:
    QLabel *m_faceLabel;
    QLabel *m_nameLabel;
    QLabel *m_loginLabel;

    void setupUi(QWidget *Form)
    {
        if (Form->objectName().isEmpty())
            Form->setObjectName(QString::fromUtf8("Form"));
        Form->resize(190, 240);
        m_faceLabel = new QLabel(Form);
        m_faceLabel->setObjectName(QString::fromUtf8("m_faceLabel"));
        m_faceLabel->setGeometry(QRect(30, 30, 130, 130));
        m_faceLabel->setStyleSheet(QString::fromUtf8("border-color: rgb(255, 255, 255);"));
        m_faceLabel->setAlignment(Qt::AlignCenter);
        m_nameLabel = new QLabel(Form);
        m_nameLabel->setObjectName(QString::fromUtf8("m_nameLabel"));
        m_nameLabel->setGeometry(QRect(30, 185, 130, 20));
        m_nameLabel->setAlignment(Qt::AlignCenter);
        m_loginLabel = new QLabel(Form);
        m_loginLabel->setObjectName(QString::fromUtf8("m_loginLabel"));
        m_loginLabel->setGeometry(QRect(30, 210, 130, 20));
        m_loginLabel->setAlignment(Qt::AlignCenter);

        retranslateUi(Form);

        QMetaObject::connectSlotsByName(Form);
    } // setupUi

    void retranslateUi(QWidget *Form)
    {
        Form->setWindowTitle(QApplication::translate("Form", "Form", 0, QApplication::UnicodeUTF8));
        m_faceLabel->setText(QApplication::translate("Form", "TextLabel", 0, QApplication::UnicodeUTF8));
        m_nameLabel->setText(QApplication::translate("Form", "yh", 0, QApplication::UnicodeUTF8));
        m_loginLabel->setText(QApplication::translate("Form", "\345\267\262\347\231\273\345\275\225", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Entry: public Ui_Entry {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ENTRY_H
