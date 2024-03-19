#ifndef OPENRGBDMXLEDSETTINGS_H
#define OPENRGBDMXLEDSETTINGS_H

#include "ui_OpenRGBDMXLedSettings.h"
#include <QWidget>

namespace Ui {
class OpenRGBDMXLedSettings;
}

class Ui::OpenRGBDMXLedSettings : public QWidget
{
    Q_OBJECT

public:
    explicit OpenRGBDMXLedSettings(QWidget *parent = nullptr);
    ~OpenRGBDMXLedSettings();
    Ui::OpenRGBDMXLedSettingsUi *ui;
};

#endif // OPENRGBDMXLEDSETTINGS_H
