#include "OpenRGBDMXSettingsPage.h"
#include "RGBController.h"
#include "ui_OpenRGBDMXSettingsPage.h"
//#include "ResourceManager.h"
//#include "SettingsManager.h"

using namespace Ui;

OpenRGBDMXSettingsPage::OpenRGBDMXSettingsPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OpenRGBDMXSettingsPageUi)
{
    ui->setupUi(this);

    json                dmx_settings;

    /*-------------------------------------------------*\
    | Get DMX settings from settings manager            |
    \*-------------------------------------------------*/
    dmx_settings = ResourceManager::get()->GetSettingsManager()->GetSettings("DMXDevices");

    /*-------------------------------------------------*\
    | If the DMX settings contains devices, process     |
    \*-------------------------------------------------*/
    //if(dmx_settings.contains("devices"))
    //{
    //    for(unsigned int device_idx = 0; device_idx < dmx_settings["devices"].size(); device_idx++)
    //    {
    //        OpenRGBDMXSettingsEntry* entry = new OpenRGBDMXSettingsEntry;
    //
    //        if(dmx_settings["devices"][device_idx].contains("name"))
    //        {
    //            entry->ui->NameEdit->setText(QString::fromStdString(dmx_settings["devices"][device_idx]["name"]));
    //        }
    //
    //        if(dmx_settings["devices"][device_idx].contains("port"))
    //        {
    //            entry->ui->PortEdit->setText(QString::fromStdString(dmx_settings["devices"][device_idx]["port"]));
    //        }
    //
    //        if(dmx_settings["devices"][device_idx].contains("red_channel"))
    //        {
    //            entry->ui->RedEdit->setText(QString::number((int)dmx_settings["devices"][device_idx]["red_channel"]));
    //        }
    //
    //        if(dmx_settings["devices"][device_idx].contains("green_channel"))
    //        {
    //            entry->ui->GreenEdit->setText(QString::number((int)dmx_settings["devices"][device_idx]["green_channel"]));
    //        }
    //
    //        if(dmx_settings["devices"][device_idx].contains("blue_channel"))
    //        {
    //            entry->ui->BlueEdit->setText(QString::number((int)dmx_settings["devices"][device_idx]["blue_channel"]));
    //        }
    //
    //        if(dmx_settings["devices"][device_idx].contains("brightness_channel"))
    //        {
    //            entry->ui->BrightnessEdit->setText(QString::number((int)dmx_settings["devices"][device_idx]["brightness_channel"]));
    //        }
    //
    //        if(dmx_settings["devices"][device_idx].contains("keepalive_time"))
    //        {
    //            entry->ui->KeepaliveTimeEdit->setText(QString::number((int)dmx_settings["devices"][device_idx]["keepalive_time"]));
    //        }
    //
    //        entries.push_back(entry);
    //
    //        QListWidgetItem* item = new QListWidgetItem;
    //
    //        item->setSizeHint(entry->sizeHint());
    //
    //        ui->DMXDeviceList->addItem(item);
    //        ui->DMXDeviceList->setItemWidget(item, entry);
    //        ui->DMXDeviceList->show();
    //    }
    //}

    for(unsigned char protocol_idx = 0; protocol_idx < NUM_PROTOCOLS; protocol_idx++)
    {
        std::string protocol = protocols[protocol_idx];

        /*-------------------------------------------------*\
        | If the DMX settings contains devices, process     |
        \*-------------------------------------------------*/
        if(dmx_settings.contains(protocol))
        {
            for(unsigned int device_idx = 0; device_idx < dmx_settings[protocol].size(); device_idx++)
            {
                OpenRGBDMXSettingsEntry* entry = new OpenRGBDMXSettingsEntry;

                entry->ui->ProtocolComboBox->setCurrentIndex(protocol_idx);

                if(dmx_settings[protocol][device_idx].contains("name"))
                {
                    entry->ui->NameEdit->setText(QString::fromStdString(dmx_settings[protocol][device_idx]["name"]));
                }

                if(dmx_settings[protocol][device_idx].contains("port"))
                {
                    entry->ui->PortEdit->setText(QString::fromStdString(dmx_settings[protocol][device_idx]["port"]));
                }

                for(unsigned char led_count = 0; led_count < dmx_settings[protocol][device_idx]["leds"].size(); led_count++)
                {
                    OpenRGBDMXLedSettings* led_settings = new OpenRGBDMXLedSettings;

                    led_settings->ui->RedSpinBox->setValue(dmx_settings[protocol][device_idx]["leds"][led_count]["red_channel"]);
                    led_settings->ui->GreenSpinBox->setValue(dmx_settings[protocol][device_idx]["leds"][led_count]["green_channel"]);
                    led_settings->ui->BlueSpinBox->setValue(dmx_settings[protocol][device_idx]["leds"][led_count]["blue_channel"]);

                    entry->leds.push_back(led_settings);
                }

                for(unsigned char effect_count = 0; effect_count < dmx_settings[protocol][device_idx]["effects"].size(); effect_count++)
                {
                    OpenRGBDMXEffectSettings* effect_settings = new OpenRGBDMXEffectSettings;

                    std::string name       = dmx_settings[protocol][device_idx]["effects"][effect_count]["name"];
                    unsigned char channel  = dmx_settings[protocol][device_idx]["effects"][effect_count]["channel"];

                    effect_settings->ui->NameEdit->setText(QString::fromStdString(name));
                    effect_settings->ui->ChannelSpinBox->setValue(channel);

                    entry->effects.push_back(effect_settings);
                }

                if(dmx_settings[protocol][device_idx].contains("brightness_channel"))
                {
                    entry->ui->BrightnessSpinBox->setValue((dmx_settings[protocol][device_idx]["brightness_channel"]));
                }

                if(dmx_settings[protocol][device_idx].contains("keepalive_time"))
                {
                    entry->ui->KeepaliveTimeEdit->setText(QString::number((int)dmx_settings[protocol][device_idx]["keepalive_time"]));
                }

                entries.push_back(entry);

                entry->setup();

                QListWidgetItem* item = new QListWidgetItem;

                item->setSizeHint(entry->sizeHint());

                ui->DMXDeviceList->addItem(item);
                ui->DMXDeviceList->setItemWidget(item, entry);
                ui->DMXDeviceList->show();
            }
        }
    }
}

OpenRGBDMXSettingsPage::~OpenRGBDMXSettingsPage()
{
    delete ui;
}

void OpenRGBDMXSettingsPage::changeEvent(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);
    }
}

void Ui::OpenRGBDMXSettingsPage::on_AddDMXDeviceButton_clicked()
{
    OpenRGBDMXSettingsEntry* entry = new OpenRGBDMXSettingsEntry;
    entries.push_back(entry);

    QListWidgetItem* item = new QListWidgetItem;

    item->setSizeHint(entry->sizeHint());

    ui->DMXDeviceList->addItem(item);
    ui->DMXDeviceList->setItemWidget(item, entry);
    ui->DMXDeviceList->show();
}

void Ui::OpenRGBDMXSettingsPage::on_RemoveDMXDeviceButton_clicked()
{
    int cur_row = ui->DMXDeviceList->currentRow();

    if(cur_row < 0)
    {
        return;
    }

    QListWidgetItem* item = ui->DMXDeviceList->takeItem(cur_row);

    ui->DMXDeviceList->removeItemWidget(item);
    delete item;

    delete entries[cur_row];
    entries.erase(entries.begin() + cur_row);
}

void Ui::OpenRGBDMXSettingsPage::on_SaveDMXConfigurationButton_clicked()
{
    json                dmx_settings;

    /*-------------------------------------------------*\
    | Get DMX settings from settings manager            |
    \*-------------------------------------------------*/
    dmx_settings = ResourceManager::get()->GetSettingsManager()->GetSettings("DMXDevices");

    //dmx_settings["devices"].clear();
    dmx_settings.clear();

    unsigned char device_count[NUM_PROTOCOLS];

    memset(device_count, 0x00, NUM_PROTOCOLS);

    for(unsigned int device_idx = 0; device_idx < entries.size(); device_idx++)
    {
        /*-------------------------------------------------*\
        | Required parameters                               |
        \*-------------------------------------------------*/
        //dmx_settings["devices"][device_idx]["name"]                     = entries[device_idx]->ui->NameEdit->text().toStdString();
        //dmx_settings["devices"][device_idx]["port"]                     = entries[device_idx]->ui->PortEdit->text().toStdString();
        //dmx_settings["devices"][device_idx]["red_channel"]              = entries[device_idx]->ui->RedEdit->text().toUInt();
        //dmx_settings["devices"][device_idx]["green_channel"]            = entries[device_idx]->ui->GreenEdit->text().toUInt();
        //dmx_settings["devices"][device_idx]["blue_channel"]             = entries[device_idx]->ui->BlueEdit->text().toUInt();
        unsigned char current_protocol = entries[device_idx]->ui->ProtocolComboBox->currentIndex();
        std::string protocol = protocols[current_protocol];

        dmx_settings[protocol][device_count[current_protocol]]["name"] = entries[device_idx]->ui->NameEdit->text().toStdString();

        if(entries[device_idx]->ui->PortEdit->text() != "")
        {
            dmx_settings[protocol][device_count[current_protocol]]["port"] = entries[device_idx]->ui->PortEdit->text().toStdString();
        }

        if(entries[device_idx]->ui->UniverseComboBox->currentIndex() != -1)
        {
            dmx_settings[protocol][device_count[current_protocol]]["universe"] = entries[device_idx]->ui->UniverseComboBox->currentIndex() + 1;
        }

        std::vector<OpenRGBDMXLedSettings*> LedSettings = entries[device_idx]->leds;
        std::vector<OpenRGBDMXEffectSettings*> EffectSettings = entries[device_idx]->effects;

        unsigned short red_channel, green_channel, blue_channel;
        unsigned short effect_channel, brightness_channel;

        for(unsigned char led_count = 0; led_count < LedSettings.size(); led_count++)
        {
            red_channel   = LedSettings[led_count]->ui->RedSpinBox->value();
            green_channel = LedSettings[led_count]->ui->GreenSpinBox->value();
            blue_channel  = LedSettings[led_count]->ui->BlueSpinBox->value();

            if(red_channel  && green_channel && blue_channel)
            {
                dmx_settings[protocol][device_count[current_protocol]]["leds"][led_count]["red_channel"]   = red_channel;
                dmx_settings[protocol][device_count[current_protocol]]["leds"][led_count]["green_channel"] = green_channel;
                dmx_settings[protocol][device_count[current_protocol]]["leds"][led_count]["blue_channel"]  = blue_channel;
            }
        }

        for(unsigned char effect_idx = 0; effect_idx < EffectSettings.size(); effect_idx++)
        {
            effect_channel = EffectSettings[effect_idx]->ui->ChannelSpinBox->value();

            if(effect_channel)
            {
                dmx_settings[protocol][device_count[current_protocol]]["effects"][effect_idx]["name"]    =
                    EffectSettings[effect_idx]->ui->NameEdit->text().toStdString();

                dmx_settings[protocol][device_count[current_protocol]]["effects"][effect_idx]["channel"] = effect_channel;
            }
        }

        /*-------------------------------------------------*\
        | Optional parameters                               |
        \*-------------------------------------------------*/
        //if(entries[device_idx]->ui->BrightnessEdit->text() != "")
        //{
        //    dmx_settings["devices"][device_idx]["brightness_channel"]   = entries[device_idx]->ui->BrightnessEdit->text().toUInt();
        //}

        //if(entries[device_idx]->ui->KeepaliveTimeEdit->text() != "")
        //{
        //    dmx_settings["devices"][device_idx]["keepalive_time"]       = entries[device_idx]->ui->KeepaliveTimeEdit->text().toUInt();
        //}
        brightness_channel = entries[device_idx]->ui->BrightnessSpinBox->value();

        if(brightness_channel)
        {
            dmx_settings[protocol][device_count[current_protocol]]["brightness_channel"] = brightness_channel;
        }

        if(entries[device_idx]->ui->KeepaliveTimeEdit->text() != "")
        {
            dmx_settings[protocol][device_count[current_protocol]]["keepalive_time"] = entries[device_idx]->ui->KeepaliveTimeEdit->text().toUInt();
        }

        device_count[current_protocol]++;
    }

    ResourceManager::get()->GetSettingsManager()->SetSettings("DMXDevices", dmx_settings);
    ResourceManager::get()->GetSettingsManager()->SaveSettings();
}
