#include "OpenRGBDMXEffectSettings.h"
#include "ui_OpenRGBDMXEffectSettings.h"

using namespace Ui;

OpenRGBDMXEffectSettings::OpenRGBDMXEffectSettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OpenRGBDMXEffectSettingsUi)
{
    ui->setupUi(this);

    ui->ChannelSpinBox->setRange(0, 512);
}

OpenRGBDMXEffectSettings::~OpenRGBDMXEffectSettings()
{
    delete ui;
}
