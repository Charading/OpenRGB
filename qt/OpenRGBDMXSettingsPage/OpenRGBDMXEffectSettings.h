#ifndef OPENRGBDMXEFFECTSETTINGS_H
#define OPENRGBDMXEFFECTSETTINGS_H

#include "ui_OpenRGBDMXEffectSettings.h"
#include <QWidget>

namespace Ui {
class OpenRGBDMXEffectSettings;
}

class Ui::OpenRGBDMXEffectSettings : public QWidget
{
    Q_OBJECT

public:
    explicit OpenRGBDMXEffectSettings(QWidget *parent = nullptr);
    ~OpenRGBDMXEffectSettings();
    Ui::OpenRGBDMXEffectSettingsUi *ui;
};

#endif // OPENRGBDMXEFFECTSETTINGS_H
