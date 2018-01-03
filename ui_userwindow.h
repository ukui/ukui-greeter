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
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Form
{
public:
    QWidget *verticalLayoutWidget;
    QVBoxLayout *m_vlayout;
    QHBoxLayout *m_hlayout;
    QLabel *m_pageIndicator;

    void setupUi(QWidget *Form)
    {
        if (Form->objectName().isEmpty())
            Form->setObjectName(QStringLiteral("Form"));
        Form->resize(1200, 300);
        verticalLayoutWidget = new QWidget(Form);
        verticalLayoutWidget->setObjectName(QStringLiteral("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(-1, -1, 1201, 311));
        m_vlayout = new QVBoxLayout(verticalLayoutWidget);
        m_vlayout->setObjectName(QStringLiteral("m_vlayout"));
        m_vlayout->setContentsMargins(0, 0, 0, 0);
        m_hlayout = new QHBoxLayout();
        m_hlayout->setObjectName(QStringLiteral("m_hlayout"));

        m_vlayout->addLayout(m_hlayout);

        m_pageIndicator = new QLabel(verticalLayoutWidget);
        m_pageIndicator->setObjectName(QStringLiteral("m_pageIndicator"));
        m_pageIndicator->setMaximumSize(QSize(1200, 30));
        m_pageIndicator->setAlignment(Qt::AlignCenter);

        m_vlayout->addWidget(m_pageIndicator, 0, Qt::AlignHCenter|Qt::AlignVCenter);


        retranslateUi(Form);

        QMetaObject::connectSlotsByName(Form);
    } // setupUi

    void retranslateUi(QWidget *Form)
    {
        Form->setWindowTitle(QApplication::translate("Form", "Form", 0));
        m_pageIndicator->setText(QApplication::translate("Form", "TextLabel", 0));
    } // retranslateUi

};

namespace Ui {
    class Form: public Ui_Form {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_USERWINDOW_H
