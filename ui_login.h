/********************************************************************************
** Form generated from reading UI file 'login.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGIN_H
#define UI_LOGIN_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Form
{
public:
    QLabel *backLB;
    QLabel *faceLB;
    QLabel *nameLB;
    QLabel *isLoginLB;
    QLabel *msgLB;

    void setupUi(QWidget *Form)
    {
        if (Form->objectName().isEmpty())
            Form->setObjectName(QStringLiteral("Form"));
        Form->resize(583, 162);
        backLB = new QLabel(Form);
        backLB->setObjectName(QStringLiteral("backLB"));
        backLB->setGeometry(QRect(0, 0, 30, 30));
        faceLB = new QLabel(Form);
        faceLB->setObjectName(QStringLiteral("faceLB"));
        faceLB->setGeometry(QRect(60, 0, 130, 130));
        nameLB = new QLabel(Form);
        nameLB->setObjectName(QStringLiteral("nameLB"));
        nameLB->setGeometry(QRect(220, 0, 200, 30));
        isLoginLB = new QLabel(Form);
        isLoginLB->setObjectName(QStringLiteral("isLoginLB"));
        isLoginLB->setGeometry(QRect(220, 35, 200, 20));
        msgLB = new QLabel(Form);
        msgLB->setObjectName(QStringLiteral("msgLB"));
        msgLB->setGeometry(QRect(220, 60, 300, 20));

        retranslateUi(Form);

        QMetaObject::connectSlotsByName(Form);
    } // setupUi

    void retranslateUi(QWidget *Form)
    {
        Form->setWindowTitle(QApplication::translate("Form", "Form", 0));
        backLB->setText(QString());
        faceLB->setText(QString());
        nameLB->setText(QString());
        isLoginLB->setText(QString());
        msgLB->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class Form: public Ui_Form {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGIN_H
