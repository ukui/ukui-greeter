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
