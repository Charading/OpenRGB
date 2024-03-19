#include "OpenRGBDMXSettingsEntry.h"
#include "RGBController.h"
#include "ui_OpenRGBDMXSettingsEntry.h"

using namespace Ui;

OpenRGBDMXSettingsEntry::OpenRGBDMXSettingsEntry(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OpenRGBDMXSettingsEntryUi)
{
    ui->setupUi(this);

    OpenRGBDMXLedSettingsHeader* entry = new OpenRGBDMXLedSettingsHeader;

    QListWidgetItem* item = new QListWidgetItem;

    item->setSizeHint(entry->sizeHint());

    ui->LedWidget->addItem(item);
    ui->LedWidget->setItemWidget(item, entry);
    ui->LedWidget->show();

    ui->ProtocolComboBox->addItem("Open DMX");
    ui->ProtocolComboBox->addItem("uDMX");

    ui->BrightnessSpinBox->setRange(0, 512);

    ui->UniverseComboBox->setEnabled(false);
}

OpenRGBDMXSettingsEntry::~OpenRGBDMXSettingsEntry()
{
    delete ui;
}

void OpenRGBDMXSettingsEntry::changeEvent(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);
    }
}

void Ui::OpenRGBDMXSettingsEntry::on_AddLedButton_clicked()
{
    OpenRGBDMXLedSettings* entry = new OpenRGBDMXLedSettings;
    leds.push_back(entry);

    QListWidgetItem* item = new QListWidgetItem;

    item->setSizeHint(entry->sizeHint());

    ui->LedWidget->insertItem(leds.size(), item);
    ui->LedWidget->setItemWidget(item, entry);
    ui->LedWidget->show();

    entry->ui->LedLabel->setText("LED " + QString::number(leds.size()));
}

void Ui::OpenRGBDMXSettingsEntry::on_AddEffectButton_clicked()
{
    OpenRGBDMXEffectSettings* entry = new OpenRGBDMXEffectSettings;
    effects.push_back(entry);

    QListWidgetItem* item = new QListWidgetItem;

    item->setSizeHint(entry->sizeHint());

    ui->LedWidget->insertItem(leds.size() + effects.size(), item);
    ui->LedWidget->setItemWidget(item, entry);
    ui->LedWidget->show();

    entry->ui->EffectLabel->setText("Effect " + QString::number(effects.size()));

}

void Ui::OpenRGBDMXSettingsEntry::on_RemoveButton_clicked()
{
    int cur_row = ui->LedWidget->currentRow();

    if(cur_row <= 0)
    {
        return;
    }

    QListWidgetItem* item = ui->LedWidget->takeItem(cur_row);

    ui->LedWidget->removeItemWidget(item);
    delete item;

    if(cur_row <= leds.size())
    {
        delete leds[cur_row - 1];
        leds.erase(leds.begin() + (cur_row - 1));

        while (cur_row <= leds.size())
        {
            leds[cur_row - 1]->ui->LedLabel->setText("LED " + QString::number(cur_row));
            cur_row ++;
        }
    }
    else
    {
        cur_row -= leds.size();

        delete effects[cur_row - 1];
        effects.erase(effects.begin() + (cur_row - 1));

        while (cur_row <= effects.size())
        {
            effects[cur_row - 1]->ui->EffectLabel->setText("Effect " + QString::number(cur_row));
            cur_row ++;
        }
    }
}

void Ui::OpenRGBDMXSettingsEntry::setup()
{
    for(unsigned char led_count = 0; led_count < leds.size(); led_count++)
    {
        OpenRGBDMXLedSettings* led_settings = leds[led_count];

        QListWidgetItem* item = new QListWidgetItem;

        item->setSizeHint(led_settings->sizeHint());

        ui->LedWidget->insertItem(leds.size(), item);
        ui->LedWidget->setItemWidget(item, led_settings);
        ui->LedWidget->show();

        led_settings->ui->LedLabel->setText("LED " + QString::number(led_count + 1));
    }

    for(unsigned char effect_count = 0; effect_count < effects.size(); effect_count++)
    {
        OpenRGBDMXEffectSettings* effect_settings = effects[effect_count];

        QListWidgetItem* item = new QListWidgetItem;

        item->setSizeHint(effect_settings ->sizeHint());

        ui->LedWidget->insertItem(leds.size() + effects.size(), item);
        ui->LedWidget->setItemWidget(item, effect_settings);
        ui->LedWidget->show();

        effect_settings ->ui->EffectLabel->setText("Effect " + QString::number(effect_count + 1));
    }
}

void Ui::OpenRGBDMXSettingsEntry::on_ProtocolComboBox_currentTextChanged(const QString &arg1)
{
    if(ui->ProtocolComboBox->currentText() != "Open DMX")
    {
        ui->PortEdit->setEnabled(false);
        ui->KeepaliveTimeEdit->setEnabled(false);
    }
    else
    {
        ui->PortEdit->setEnabled(true);
        ui->KeepaliveTimeEdit->setEnabled(true);
    }
}
