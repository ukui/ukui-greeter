/********************************************************************************
** Form generated from reading UI file 'userwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_USERWINDOW_H
#define UI_USERWINDOW_H

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
    QWidget *m_userList;
    QLabel *m_prevLabel;
    QLabel *m_nextLabel;
    QLabel *m_pageIndicator;

    void setupUi(QWidget *Form)
    {
        if (Form->objectName().isEmpty())
            Form->setObjectName(QStringLiteral("Form"));
        Form->resize(1200, 350);
        Form->setMaximumSize(QSize(1200, 400));
        m_userList = new QWidget(Form);
        m_userList->setObjectName(QStringLiteral("m_userList"));
        m_userList->setGeometry(QRect(150, 25, 901, 251));
        m_prevLabel = new QLabel(Form);
        m_prevLabel->setObjectName(QStringLiteral("m_prevLabel"));
        m_prevLabel->setGeometry(QRect(80, 70, 64, 128));
        m_nextLabel = new QLabel(Form);
        m_nextLabel->setObjectName(QStringLiteral("m_nextLabel"));
        m_nextLabel->setGeometry(QRect(1050, 70, 64, 128));
        m_pageIndicator = new QLabel(Form);
        m_pageIndicator->setObjectName(QStringLiteral("m_pageIndicator"));
        m_pageIndicator->setGeometry(QRect(450, 300, 300, 20));
        m_pageIndicator->setAlignment(Qt::AlignCenter);

        retranslateUi(Form);

        QMetaObject::connectSlotsByName(Form);
    } // setupUi

    void retranslateUi(QWidget *Form)
    {
        Form->setWindowTitle(QApplication::translate("Form", "Form", 0));
        m_prevLabel->setText(QApplication::translate("Form", "TextLabel", 0));
        m_nextLabel->setText(QApplication::translate("Form", "TextLabel", 0));
        m_pageIndicator->setText(QApplication::translate("Form", "TextLabel", 0));
    } // retranslateUi

};

namespace Ui {
    class Form: public Ui_Form {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_USERWINDOW_H
