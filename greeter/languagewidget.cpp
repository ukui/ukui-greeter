#include "languagewidget.h"
#include <QListWidget>
#include <QLabel>
#include <QDebug>

LanguageWidget::LanguageWidget(QWidget *parent)
    : BorderDialog(parent),
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

    lblPrompt = new QLabel(center());
    lblPrompt->setObjectName("lblLanguagePrompt");
    lblPrompt->setText(tr("Please select the language of session"));
    lblPrompt->setGeometry(0, 0, center()->width(), 40);
    lblPrompt->setAlignment(Qt::AlignCenter);

    lwLanguages = new QListWidget(center());
    lwLanguages->setObjectName("languageList");
    lwLanguages->setGeometry(lblPrompt->x() + 10, lblPrompt->y() + lblPrompt->height() + 20,
                             center()->width() - 20, center()->height() - lblPrompt->height() - 40);
    lwLanguages->setSpacing(3);

    for(Language &lang : languagesVector)
    {
        QString text = lang.name + (lang.territory.isEmpty() ? "" : ("-" + lang.territory));
        QListWidgetItem *item = new QListWidgetItem(text, lwLanguages);
        item->setData(Qt::UserRole, lang.code);
        lwLanguages->addItem(item);
    }

    connect(lwLanguages, &QListWidget::itemDoubleClicked,
            this, &LanguageWidget::onListCurrentItemChanged);
}

void LanguageWidget::close()
{
    QListWidgetItem *item = lwLanguages->currentItem();
    QString languageCode = item->data(Qt::UserRole).toString();

    Q_EMIT languageChanged(languageCode);

    BorderDialog::close();
}

void LanguageWidget::onListCurrentItemChanged(QListWidgetItem */*item*/)
{
    close();
}
