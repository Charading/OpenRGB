#ifndef OPENRGBBLEDOMSETTINGSPAGE_H
#define OPENRGBBLEDOMSETTINGSPAGE_H

#include "ui_OpenRGBBleDomSettingsPage.h"
#include <QWidget>
#include <QBluetoothDeviceDiscoveryAgent>


namespace Ui {
class OpenRGBBleDomSettingsPage;
}

class Ui::OpenRGBBleDomSettingsPage : public QWidget
{
    Q_OBJECT

public:
    explicit OpenRGBBleDomSettingsPage(QWidget *parent = nullptr);
    ~OpenRGBBleDomSettingsPage();

private slots:
    // Bluetooth
    void deviceDetected(const QBluetoothDeviceInfo device);
    void scanError(QBluetoothDeviceDiscoveryAgent::Error error);
    void scanFinished();
    // Ui
    void on_refreshBtn_clicked();

    void on_saveBtn_clicked();

private:
    Ui::OpenRGBBleDomSettingsPageUi *ui;

    QBluetoothDeviceDiscoveryAgent *m_deviceDiscoveryAgent;
    QList<QBluetoothDeviceInfo> devices;
    void AddDevice(QBluetoothDeviceInfo device, bool addAsChecked = false);
};

#endif // OPENRGBBLEDOMSETTINGSPAGE_H
