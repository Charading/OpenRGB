#ifndef OPENRGBDMXSETTINGSENTRY_H
#define OPENRGBDMXSETTINGSENTRY_H

#include "ui_OpenRGBDMXSettingsEntry.h"
#include "ui_OpenRGBDMXSettingsPage.h"
#include <QWidget>

#include "OpenRGBDMXLedSettings.h"
#include "OpenRGBDMXLedSettingsHeader.h"
#include "OpenRGBDMXEffectSettings.h"

namespace Ui {
class OpenRGBDMXSettingsEntry;
}

class Ui::OpenRGBDMXSettingsEntry : public QWidget
{
    Q_OBJECT

public:
    explicit OpenRGBDMXSettingsEntry(QWidget *parent = nullptr);
    ~OpenRGBDMXSettingsEntry();
    //Ui::OpenRGBDMXSettingsEntryUi *ui;

    Ui::OpenRGBDMXSettingsEntryUi*                           ui;
    std::vector<OpenRGBDMXLedSettings*>                    leds;
    std::vector<OpenRGBDMXEffectSettings*>              effects;

    void setup();

private slots:
    void changeEvent(QEvent *event);

    void on_AddLedButton_clicked();

    void on_AddEffectButton_clicked();

    void on_ProtocolComboBox_currentTextChanged(const QString &arg1);

    void on_RemoveButton_clicked();
};

#endif // OPENRGBDMXSETTINGSENTRY_H
