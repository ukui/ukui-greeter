/**
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
#include "languagewidget.h"
#include <QListWidget>
#include <QLabel>
#include <QDebug>

LanguageWidget::LanguageWidget(QWidget *parent)
    : FakeDialog(parent),
      languagesVector(getLanguages())
{
    initUI();
}

void LanguageWidget::setCurrentLanguage(const QString &language)
{
    for(int i = 0; i < lwLanguages->count(); i++)
    {
        QListWidgetItem *item = lwLanguages->item(i);
        if(item->data(Qt::UserRole).toString() == language)
        {
            lwLanguages->setCurrentItem(item);
        }
    }
}


void LanguageWidget::initUI()
{
    setDialogSize(600, 400);

    lblPrompt = new QLabel(centerWidget());
    lblPrompt->setObjectName("lblLanguagePrompt");
    lblPrompt->setText(tr("Please select the language of session"));
    lblPrompt->setGeometry(0, 10, centerWidget()->width(), 40);
    lblPrompt->setAlignment(Qt::AlignCenter);

    lwLanguages = new QListWidget(centerWidget());
    lwLanguages->setObjectName("languageList");
    lwLanguages->setGeometry(lblPrompt->x() + 10, lblPrompt->y() + lblPrompt->height() + 20,
                             centerWidget()->width() - 20, centerWidget()->height() - lblPrompt->height() - 40);
    lwLanguages->setSpacing(3);

    int i = 0;
    for(Language &lang : languagesVector)
    {
        QString text = lang.name + (lang.territory.isEmpty() ? "" : ("-" + lang.territory));
        QLabel *label = new QLabel(text, this);
        label->setObjectName(QStringLiteral("languageLabel"));
        QListWidgetItem *item = new QListWidgetItem(lwLanguages);
        item->setData(Qt::UserRole, lang.code);
        lwLanguages->insertItem(i++, item);
        lwLanguages->setItemWidget(item, label);
    }

    connect(lwLanguages, &QListWidget::itemDoubleClicked,
            this, &LanguageWidget::onListCurrentItemChanged);
}
void LanguageWidget::showEvent(QShowEvent *event)
{
    FakeDialog::showEvent(event);
  //  lwLanguages->setFocus();
}

void LanguageWidget::closeEvent(QCloseEvent *event)
{
    QListWidgetItem *item = lwLanguages->currentItem();
    if(item)
    {
        QString languageCode = item->data(Qt::UserRole).toString();

        auto iter = std::find_if(languagesVector.begin(), languagesVector.end(),
                                 [&](const Language &language) {
            return language.code == languageCode;
        });

        Q_EMIT languageChanged(*iter);
    }

    FakeDialog::closeEvent(event);
}

void LanguageWidget::onListCurrentItemChanged(QListWidgetItem */*item*/)
{
    close();
}
