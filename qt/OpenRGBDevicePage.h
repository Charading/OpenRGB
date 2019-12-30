#ifndef OPENRGBDEVICEPAGE_H
#define OPENRGBDEVICEPAGE_H

#include "ui_OpenRGBDevicePage.h"
#include "RGBController.h"

#include <QFrame>

namespace Ui {
class OpenRGBDevicePage;
}

class Ui::OpenRGBDevicePage : public QFrame
{
    Q_OBJECT

public:
    explicit OpenRGBDevicePage(RGBController *dev, QWidget *parent = nullptr);
    ~OpenRGBDevicePage();

    void SetDevice(unsigned char red, unsigned char green, unsigned char blue);

private slots:
    void on_ButtonRed_clicked();
    void on_ButtonYellow_clicked();
    void on_ButtonGreen_clicked();
    void on_ButtonCyan_clicked();
    void on_ButtonBlue_clicked();
    void on_ButtonMagenta_clicked();
    void on_ZoneBox_currentIndexChanged();
    void on_LEDBox_currentIndexChanged();
    void on_ModeBox_currentIndexChanged();
    void on_SetDeviceButton_clicked();
    void on_SetZoneButton_clicked();
    void on_SetLEDButton_clicked();
    void on_RedSpinBox_valueChanged();
    void on_HueSpinBox_valueChanged();
    void on_GreenSpinBox_valueChanged();
    void on_SatSpinBox_valueChanged();
    void on_BlueSpinBox_valueChanged();
    void on_ValSpinBox_valueChanged();

    void on_SetAllButton_clicked();

private:
    Ui::OpenRGBDevicePageUi *ui;
    RGBController *device;

    bool UpdatingColor = false;

    void updateRGB();

    void updateHSV();

signals:
    void SetAllDevices(unsigned char red, unsigned char green, unsigned char blue);
};

#endif // OPENRGBDEVICEPAGE_H
