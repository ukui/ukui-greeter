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
#include <QAction>
#include <QDebug>

LanguageWidget::LanguageWidget(QWidget *parent)
    : QMenu (parent),
      languagesVector(getLanguages())
{
    initUserMenu();
}

void LanguageWidget::setCurrentLanguage(const QString &language)
{
    for(auto action : this->actions())
    {
        if(action->text() == language)
            this->setActiveAction(action);
    }
}


void LanguageWidget::initUserMenu()
{

    connect(this, &QMenu::triggered,
                this, &LanguageWidget::onLanguageMenuTrigged);

    for(Language &lang : languagesVector)
    {
        QString text = lang.name + (lang.territory.isEmpty() ? "" : ("-" + lang.territory));
        QAction *action = new QAction (this);
        action->setText(text);
        action->setData(lang.code);
        addAction(action);
    }

}

void LanguageWidget::onLanguageAdded(QString lang)
{


}

void LanguageWidget::onLanguageDeleted(QString lang)
{
    for(auto action : this->actions())
    {
        if(action->text() == lang)
            this->removeAction(action);
    }
}

void LanguageWidget::onLanguageMenuTrigged(QAction *action)
{
    QString languageCode = action->data().toString();
    auto iter = std::find_if(languagesVector.begin(), languagesVector.end(),
                             [&](const Language &language) {
        return language.code == languageCode;
    });

    Q_EMIT languageChanged(*iter);
}
