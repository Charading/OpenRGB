#ifndef BLEDOMCONTROLLER_H
#define BLEDOMCONTROLLER_H

#include <QBluetoothAddress>
#include <QLowEnergyController>
#include <QBluetoothUuid>
#include <QFuture>


class BLEDOMController: public QObject
{
    Q_OBJECT
public:
    enum Effect : unsigned char {
        jump_red_green_blue = 0x87,
        jump_red_green_blue_yellow_cyan_magenta_white = 0x88,
        crossfade_red = 0x8b,
        crossfade_green = 0x8c,
        crossfade_blue = 0x8d,
        crossfade_yellow = 0x8e,
        crossfade_cyan = 0x8f,
        crossfade_magenta = 0x90,
        crossfade_white = 0x91,
        crossfade_red_green = 0x92,
        crossfade_red_blue = 0x93,
        crossfade_green_blue = 0x94,
        crossfade_red_green_blue = 0x89,
        crossfade_red_green_blue_yellow_cyan_magenta_white = 0x8a,
        blink_red = 0x96,
        blink_green = 0x97,
        blink_blue = 0x98,
        blink_yellow = 0x99,
        blink_cyan = 0x9a,
        blink_magenta = 0x9b,
        blink_white = 0x9c,
        blink_red_green_blue_yellow_cyan_magenta_white = 0x95,
    };

    static const QBluetoothUuid* LightService;
    static const QBluetoothUuid* ControlCharacteristic;
    static QBluetoothDeviceInfo DeviceFromData(QString addressStr, QString name, quint32 devClass);
    static QList<QBluetoothDeviceInfo> GetSavedDevices();
    BLEDOMController(QBluetoothDeviceInfo device);
    ~BLEDOMController();
    void Connect();
    QFuture<void> PerformTest();

    std::string GetBluetoothAddress();
    void SetColor(unsigned char r, unsigned char g, unsigned char b);
    void SetPower(bool isOn);
    void SetBrightness(unsigned char brightness);
    void SetEffect(Effect effect);
    void SetEffectSpeed(unsigned char effectSpeed);
signals:
    void Ready();
    void Errored(QString error);

private:
    QLowEnergyController* controller;
    QLowEnergyService* lightService;
    QLowEnergyCharacteristic controlCharacteristic;
    bool _poweredOn = true;
    void SendCommand(unsigned char command, unsigned char param1 = 0, unsigned char param2 = 0, unsigned char param3 = 0, unsigned char param4 = 0);
    bool ready;
private slots:
    void serviceDiscovered(const QBluetoothUuid &newService);
    void serviceScanDone();

    void serviceStateChanged(QLowEnergyService::ServiceState s);
};

#endif // BLEDOMCONTROLLER_H
