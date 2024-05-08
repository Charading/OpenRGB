#include "ui_OpenRGBDMXLedSettingsHeader.h"
#include "OpenRGBDMXLedSettingsHeader.h"

using namespace Ui;

OpenRGBDMXLedSettingsHeader::OpenRGBDMXLedSettingsHeader(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OpenRGBDMXLedSettingsHeaderUi)
{
    ui->setupUi(this);
}

OpenRGBDMXLedSettingsHeader::~OpenRGBDMXLedSettingsHeader()
{
    delete ui;
}




