#ifndef LANGUAGEWIDGET_H
#define LANGUAGEWIDGET_H

#include <QWidget>
#include "language.h"
#include "borderdialog.h"

class QListWidget;
class QListWidgetItem;
class QLabel;
class LanguageWidget : public BorderDialog
{
    Q_OBJECT
public:
    explicit LanguageWidget(QWidget *parent = nullptr);
    void setCurrentLanguage(const QString &language);
    void close();

private:
    void initUI();

private Q_SLOTS:
    void onListCurrentItemChanged(QListWidgetItem *item);

Q_SIGNALS:
    void languageChanged(const QString &languageCode);

private:
    LanguagesVector &languagesVector;
    QListWidget     *lwLanguages;
    QLabel          *lblPrompt;
};

#endif // LANGUAGEWIDGET_H
