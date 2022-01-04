#include "OpenRGBDeviceInfoPage.h"

using namespace Ui;

OpenRGBDeviceInfoPage::OpenRGBDeviceInfoPage(RGBController *dev, QWidget *parent) :
    QFrame(parent),
    ui(new Ui::OpenRGBDeviceInfoPageUi)
{
    controller = dev;

    ui->setupUi(this);

    ui->TypeValue->setText(device_type_to_str(dev->type).c_str());

    ui->NameValue->setText(QString::fromStdString(dev->name));
    ui->VendorValue->setText(QString::fromStdString(dev->vendor));
    ui->DescriptionValue->setText(QString::fromStdString(dev->description));
    ui->VersionValue->setText(QString::fromStdString(dev->version));
    ui->LocationValue->setText(QString::fromStdString(dev->location));
    ui->SerialValue->setText(QString::fromStdString(dev->serial));

    if (dev->type == DEVICE_TYPE_KEYBOARD)
    {
        ui->LayoutLabel->setVisible(true);
        ui->LayoutValue->setText(QString::fromStdString(dev->keyboard_layout));
    }
    else
    {
        ui->LayoutLabel->setVisible(false);
        ui->LayoutValue->setVisible(false);
    }

    if (dev->device_variant != "")
    {
        ui->VariantLabel->setVisible(true);
        ui->VariantValue->setText(QString::fromStdString(dev->device_variant));
    }
    else
    {
        ui->VariantLabel->setVisible(false);
        ui->VariantValue->setVisible(false);
    }
}

OpenRGBDeviceInfoPage::~OpenRGBDeviceInfoPage()
{
    delete ui;
}

RGBController* OpenRGBDeviceInfoPage::GetController()
{
    return controller;
}
