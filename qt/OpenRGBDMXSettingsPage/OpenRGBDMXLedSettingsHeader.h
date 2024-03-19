#ifndef OPENRGBDMXLEDSETTINGSHEADER_H
#define OPENRGBDMXLEDSETTINGSHEADER_H

#include "ui_OpenRGBDMXLedSettingsHeader.h"
#include <QWidget>

namespace Ui {
class OpenRGBDMXLedSettingsHeader;
}

class Ui::OpenRGBDMXLedSettingsHeader : public QWidget
{
    Q_OBJECT

public:
    explicit OpenRGBDMXLedSettingsHeader(QWidget *parent = nullptr);
    ~OpenRGBDMXLedSettingsHeader();

private slots:

private:
    Ui::OpenRGBDMXLedSettingsHeaderUi *ui;
};

#endif // OPENRGBDMXLEDSETTINGSHEADER_H
