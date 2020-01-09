/* ui_mainwindow.h
 * Copyright (C) 2018 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301, USA.
**/
/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.9.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QPushButton *mode_1;
    QPushButton *mode_2;
    QPushButton *mode_3;
    QPushButton *mode_4;
    QLabel *bg_mode_1;
    QLabel *lb_mode_1;
    QLabel *split_1;
    QLabel *split_2;
    QLabel *split_3;
    QLabel *lb_mode_2;
    QLabel *lb_mode_3;
    QLabel *lb_mode_4;
    QLabel *bg_mode_2;
    QLabel *bg_mode_3;
    QLabel *bg_mode_4;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(744, 126);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        mode_1 = new QPushButton(centralWidget);
        mode_1->setObjectName(QStringLiteral("mode"));
        mode_1->setGeometry(QRect(5, 5, 180, 116));
        mode_2 = new QPushButton(centralWidget);
        mode_2->setObjectName(QStringLiteral("mode"));
        mode_2->setGeometry(QRect(190, 5, 180, 116));
        mode_3 = new QPushButton(centralWidget);
        mode_3->setObjectName(QStringLiteral("mode"));
        mode_3->setGeometry(QRect(375, 5, 180, 116));
        mode_4 = new QPushButton(centralWidget);
        mode_4->setObjectName(QStringLiteral("mode"));
        mode_4->setGeometry(QRect(560, 5, 180, 116));
        bg_mode_1 = new QLabel(centralWidget);
        bg_mode_1->setObjectName(QStringLiteral("bg_mode_1"));
        bg_mode_1->setGeometry(QRect(63, 43, 64, 64));
        lb_mode_1 = new QLabel(centralWidget);
        lb_mode_1->setObjectName(QStringLiteral("lb_mode"));
        lb_mode_1->setGeometry(QRect(63, 18, 64, 20));
        lb_mode_1->setAlignment(Qt::AlignCenter);
        split_1 = new QLabel(centralWidget);
        split_1->setObjectName(QStringLiteral("split"));
        split_1->setGeometry(QRect(187, 18, 1, 90));
        split_2 = new QLabel(centralWidget);
        split_2->setObjectName(QStringLiteral("split"));
        split_2->setGeometry(QRect(372, 18, 1, 90));
        split_3 = new QLabel(centralWidget);
        split_3->setObjectName(QStringLiteral("split"));
        split_3->setGeometry(QRect(557, 18, 1, 90));
        lb_mode_2 = new QLabel(centralWidget);
        lb_mode_2->setObjectName(QStringLiteral("lb_mode"));
        lb_mode_2->setGeometry(QRect(248, 18, 64, 20));
        lb_mode_2->setAlignment(Qt::AlignCenter);
        lb_mode_3 = new QLabel(centralWidget);
        lb_mode_3->setObjectName(QStringLiteral("lb_mode"));
        lb_mode_3->setGeometry(QRect(433, 18, 64, 20));
        lb_mode_3->setAlignment(Qt::AlignCenter);
        lb_mode_4 = new QLabel(centralWidget);
        lb_mode_4->setObjectName(QStringLiteral("lb_mode"));
        lb_mode_4->setGeometry(QRect(618, 18, 64, 20));
        lb_mode_4->setAlignment(Qt::AlignCenter);
        bg_mode_2 = new QLabel(centralWidget);
        bg_mode_2->setObjectName(QStringLiteral("bg_mode_2"));
        bg_mode_2->setGeometry(QRect(248, 43, 64, 64));
        bg_mode_3 = new QLabel(centralWidget);
        bg_mode_3->setObjectName(QStringLiteral("bg_mode_3"));
        bg_mode_3->setGeometry(QRect(433, 43, 64, 64));
        bg_mode_4 = new QLabel(centralWidget);
        bg_mode_4->setObjectName(QStringLiteral("bg_mode_4"));
        bg_mode_4->setGeometry(QRect(618, 43, 64, 64));
        MainWindow->setCentralWidget(centralWidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", Q_NULLPTR));
        mode_1->setText(QString());
        mode_2->setText(QString());
        mode_3->setText(QString());
        mode_4->setText(QString());
        bg_mode_1->setText(QString());
        lb_mode_1->setText(QString());
        split_1->setText(QString());
        split_2->setText(QString());
        split_3->setText(QString());
        lb_mode_2->setText(QString());
        lb_mode_3->setText(QString());
        lb_mode_4->setText(QString());
        bg_mode_2->setText(QString());
        bg_mode_3->setText(QString());
        bg_mode_4->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
