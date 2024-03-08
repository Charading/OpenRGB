#ifndef OPENRGBDMXSETTINGSPAGE_H
#define OPENRGBDMXSETTINGSPAGE_H

#include "ui_OpenRGBDMXSettingsPage.h"
#include <QWidget>

#include "OpenRGBDMXSettingsEntry.h"
#include "ResourceManager.h"
#include "SettingsManager.h"

#define NUM_PROTOCOLS 2

const std::string protocols[] =
{
    "open_dmx",
    "udmx"
};

namespace Ui {
class OpenRGBDMXSettingsPage;
}

class Ui::OpenRGBDMXSettingsPage : public QWidget
{
    Q_OBJECT;

public:

    explicit OpenRGBDMXSettingsPage(QWidget *parent = nullptr);
    ~OpenRGBDMXSettingsPage();

private slots:

    void changeEvent(QEvent *event);
    void on_AddDMXDeviceButton_clicked();

    void on_RemoveDMXDeviceButton_clicked();

    void on_SaveDMXConfigurationButton_clicked();    

private:
    Ui::OpenRGBDMXSettingsPageUi*           ui;
    std::vector<OpenRGBDMXSettingsEntry*>   entries;
};

#endif // OPENRGBDMXSETTINGSPAGE_H
