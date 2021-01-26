#include "OpenRGBSystemInfoPage.h"
#include "ResourceManager.h"
#include "i2c_tools.h"
#include <QDebug>
using namespace Ui;

static void UpdateBusListCallback(void * this_ptr)
{
    OpenRGBSystemInfoPage * this_obj = (OpenRGBSystemInfoPage *)this_ptr;

    QMetaObject::invokeMethod(this_obj, "UpdateBusList", Qt::QueuedConnection);
}

void Ui::OpenRGBSystemInfoPage::CreateReadThread()
{
    std::thread([=]()
    {
        while (true) {
            if (ConstantRead)
            {
                if (ConstBus == nullptr){ConstantRead = false; ui->ConstantRead->setCheckState(Qt::Unchecked); qDebug() << "No Bus"; continue;}
                if (Constaddr > 0xFF){ConstantRead = false; ui->ConstantRead->setCheckState(Qt::Unchecked); qDebug() << "No Address"; continue;}
                if (ConstReg > 0xFF){ConstantRead = false; ui->ConstantRead->setCheckState(Qt::Unchecked); qDebug() << "No Register"; continue;}
                if (ConstSize > 0xFF){ConstantRead = false; ui->ConstantRead->setCheckState(Qt::Unchecked); qDebug() << "No Size"; continue;}
                i2c_read(ConstBus, Constaddr, ConstReg, ConstSize);
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            else
            {
                std::this_thread::sleep_for(std::chrono::seconds(2));
            }
        }
    }).detach();
}

void Ui::OpenRGBSystemInfoPage::on_SwitchConstToCurrent_clicked()
{
    int current_index = ui->SMBusAdaptersBox->currentIndex();

    if(current_index < 0)
    {
        current_index = 0;
    }

    if((int)(busses.size()) > current_index)
    {
        ConstBus = busses[current_index];
    }
    Constaddr             = ui->ReadAddressBox->value();
    ConstReg              = ui->ReadRegisterBox->value();
    ConstSize             = ui->ReadSizeBox->value();
}

OpenRGBSystemInfoPage::OpenRGBSystemInfoPage(std::vector<i2c_smbus_interface *>& bus, QWidget *parent) :
    QFrame(parent),
    ui(new Ui::OpenRGBSystemInfoPageUi),
    busses(bus)
{
    ui->setupUi(this);

    /*-----------------------------------------------------*\
    | Use a monospace font for the text box                 |
    \*-----------------------------------------------------*/
    QFont MonoFont("monospace");
    MonoFont.setStyleHint(QFont::Monospace);
    ui->SMBusDataText->setFont(MonoFont);

    /*-----------------------------------------------------*\
    | Register I2C bus list change callback                 |
    \*-----------------------------------------------------*/
    ResourceManager::get()->RegisterI2CBusListChangeCallback(UpdateBusListCallback, this);

    /*-----------------------------------------------------*\
    | Update the bus list                                   |
    \*-----------------------------------------------------*/
    UpdateBusList();

    ui->SMBusDetectionModeBox->addItem("Auto");
    ui->SMBusDetectionModeBox->addItem("Quick");
    ui->SMBusDetectionModeBox->addItem("Read");

    ui->SMBusDetectionModeBox->setCurrentIndex(0);

    Ui::OpenRGBSystemInfoPage::CreateReadThread();
}

OpenRGBSystemInfoPage::~OpenRGBSystemInfoPage()
{
    delete ui;
}

void Ui::OpenRGBSystemInfoPage::UpdateBusList()
{
    /*-----------------------------------------------------*\
    | Fill in the combo boxes with device information       |
    \*-----------------------------------------------------*/
    ui->SMBusAdaptersBox->clear();

    for (std::size_t i = 0; i < busses.size(); i++)
    {
        ui->SMBusAdaptersBox->addItem(busses[i]->device_name);
    }

    ui->SMBusAdaptersBox->setCurrentIndex(0);
}

void Ui::OpenRGBSystemInfoPage::on_DetectButton_clicked()
{
    int current_index = ui->SMBusAdaptersBox->currentIndex();

    if(current_index < 0)
    {
        current_index = 0;
    }

    if((int)(busses.size()) > current_index)
    {
        i2c_smbus_interface* bus = busses[current_index];

        switch(ui->SMBusDetectionModeBox->currentIndex())
        {
        case 0:
            ui->SMBusDataText->setPlainText(i2c_detect(bus, MODE_AUTO).c_str());
            break;

        case 1:
            ui->SMBusDataText->setPlainText(i2c_detect(bus, MODE_QUICK).c_str());
            break;

        case 2:
            ui->SMBusDataText->setPlainText(i2c_detect(bus, MODE_READ).c_str());
            break;
        }
    }
}

void Ui::OpenRGBSystemInfoPage::on_DumpButton_clicked()
{
    int current_index = ui->SMBusAdaptersBox->currentIndex();

    if(current_index < 0)
    {
        current_index = 0;
    }
    
    if((int)(busses.size()) > current_index)
    {
        i2c_smbus_interface* bus = busses[current_index];
        unsigned char address = ui->DumpAddressBox->value();

        ui->SMBusDataText->setPlainText(i2c_dump(bus, address).c_str());
    }
}

void Ui::OpenRGBSystemInfoPage::on_ReadButton_clicked()
{
    int current_index = ui->SMBusAdaptersBox->currentIndex();

    if(current_index < 0)
    {
        current_index = 0;
    }

    if((int)(busses.size()) > current_index)
    {
        i2c_smbus_interface* bus = busses[current_index];
        unsigned char address = ui->ReadAddressBox->value();
        unsigned char regaddr = ui->ReadRegisterBox->value();
        unsigned char size    = ui->ReadSizeBox->value();

        ui->SMBusDataText->setPlainText(i2c_read(bus, address, regaddr, size).c_str());
    }
}

void Ui::OpenRGBSystemInfoPage::on_ConstantRead_clicked()
{
    if (ui->ConstantRead->isChecked())
    {
        ConstantRead = true;
    }
    else
    {
        ConstantRead = false;
    }
}
