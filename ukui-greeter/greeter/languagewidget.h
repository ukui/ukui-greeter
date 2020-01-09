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
#ifndef LANGUAGEWIDGET_H
#define LANGUAGEWIDGET_H

#include <QWidget>
#include "language.h"
#include "fakedialog.h"

class QListWidget;
class QListWidgetItem;
class QLabel;
class LanguageWidget : public FakeDialog
{
    Q_OBJECT
public:
    explicit LanguageWidget(QWidget *parent = nullptr);
    void setCurrentLanguage(const QString &language);

protected:
    void showEvent(QShowEvent *event);
    void closeEvent(QCloseEvent *event);

private:
    void initUI();

private Q_SLOTS:
    void onListCurrentItemChanged(QListWidgetItem *item);

Q_SIGNALS:
    void languageChanged(const Language &language);

private:
    LanguagesVector &languagesVector;
    QListWidget     *lwLanguages;
    QLabel          *lblPrompt;
};

#endif // LANGUAGEWIDGET_H
