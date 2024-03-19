#include "OpenRGBDMXLedSettings.h"
#include "ui_OpenRGBDMXLedSettings.h"

using namespace Ui;

OpenRGBDMXLedSettings::OpenRGBDMXLedSettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OpenRGBDMXLedSettingsUi)
{
    ui->setupUi(this);

    ui->RedSpinBox->setRange  (0, 512);
    ui->GreenSpinBox->setRange(0, 512);
    ui->BlueSpinBox->setRange (0, 512);

}

OpenRGBDMXLedSettings::~OpenRGBDMXLedSettings()
{
    delete ui;
}
