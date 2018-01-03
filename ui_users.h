/********************************************************************************
** Form generated from reading UI file 'users.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_USERS_H
#define UI_USERS_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Form
{
public:
    QLabel *preLabel;
    QLabel *nextLabel;
    QScrollArea *listArea;
    QWidget *scrollAreaWidgetContents;

    void setupUi(QWidget *Form)
    {
        if (Form->objectName().isEmpty())
            Form->setObjectName(QStringLiteral("Form"));
        Form->resize(1200, 300);
        Form->setMinimumSize(QSize(1200, 300));
        Form->setMaximumSize(QSize(1200, 300));
        preLabel = new QLabel(Form);
        preLabel->setObjectName(QStringLiteral("preLabel"));
        preLabel->setGeometry(QRect(0, 69, 64, 100));
        nextLabel = new QLabel(Form);
        nextLabel->setObjectName(QStringLiteral("nextLabel"));
        nextLabel->setGeometry(QRect(1050, 69, 64, 100));
        listArea = new QScrollArea(Form);
        listArea->setObjectName(QStringLiteral("listArea"));
        listArea->setGeometry(QRect(80, 60, 952, 201));
        listArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QStringLiteral("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 950, 199));
        listArea->setWidget(scrollAreaWidgetContents);

        retranslateUi(Form);

        QMetaObject::connectSlotsByName(Form);
    } // setupUi

    void retranslateUi(QWidget *Form)
    {
        Form->setWindowTitle(QApplication::translate("Form", "Form", 0));
        preLabel->setText(QApplication::translate("Form", "TextLabel", 0));
        nextLabel->setText(QApplication::translate("Form", "TextLabel", 0));
    } // retranslateUi

};

namespace Ui {
    class Form: public Ui_Form {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_USERS_H
