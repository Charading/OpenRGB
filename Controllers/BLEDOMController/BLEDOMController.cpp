#include "BLEDOMController.h"
#include <QDebug>
#include <QTimer>
#include <QtConcurrent/QtConcurrent>
#include "ResourceManager.h"
#include "SettingsManager.h"

// Based on
// https://github.com/FergusInLondon/ELK-BLEDOM/blob/master/PROTCOL.md
// https://github.com/TheSylex/ELK-BLEDOM-bluetooth-led-strip-controller/blob/main/src/device.rs
const QBluetoothUuid* BLEDOMController::LightService = new QBluetoothUuid(QStringLiteral("{0000fff0-0000-1000-8000-00805f9b34fb}"));
const QBluetoothUuid* BLEDOMController::ControlCharacteristic = new QBluetoothUuid(QStringLiteral("0000fff3-0000-1000-8000-00805f9b34fb"));

QBluetoothDeviceInfo BLEDOMController::DeviceFromData(QString addressStr, QString name, quint32 devClass)
{
    const QBluetoothAddress address = *new QBluetoothAddress(addressStr);
    QBluetoothDeviceInfo device = *new QBluetoothDeviceInfo(address, name, devClass);
    return device;
}

QList<QBluetoothDeviceInfo> BLEDOMController::GetSavedDevices()
{
    QList<QBluetoothDeviceInfo> result;
    json wiz_settings = ResourceManager::get()->GetSettingsManager()->GetSettings("BleDomDevices");
    if(wiz_settings.contains("devices"))
    {
        for(unsigned int device_idx = 0; device_idx < wiz_settings["devices"].size(); device_idx++)
        {
            result.append(BLEDOMController::DeviceFromData(
                          QString::fromStdString(wiz_settings["devices"][device_idx]["address"]),
                          QString::fromStdString(wiz_settings["devices"][device_idx]["name"]),
                          wiz_settings["devices"][device_idx]["class"]
                          ));
        }
    }
    return result;
}

BLEDOMController::BLEDOMController(QBluetoothDeviceInfo device)
{
    ready = false;
    controller = QLowEnergyController::createCentral(device);

    connect(controller, &QLowEnergyController::serviceDiscovered,
            this, &BLEDOMController::serviceDiscovered);
    connect(controller, &QLowEnergyController::discoveryFinished,
            this, &BLEDOMController::serviceScanDone);

    connect(controller, static_cast<void (QLowEnergyController::*)(QLowEnergyController::Error)>(&QLowEnergyController::error),
            this, [this, device](QLowEnergyController::Error error) {
                Q_UNUSED(error);
                qWarning("Cannot connect to remote device.");
                emit Errored(tr("Cannot connect to remote BLEDOM device %1.").arg(device.address().toString()));
                this->ready = false;
            });
    connect(controller, &QLowEnergyController::connected, this, [this]() {
        qDebug("Controller connected. Search services...");
        controller->discoverServices();
    });
    connect(controller, &QLowEnergyController::disconnected, this, [this]() {
        qInfo("LowEnergy controller disconnected");
        this->ready = false;
    });


}

BLEDOMController::~BLEDOMController()
{
    if(controller->state() != QLowEnergyController::ClosingState || controller->state() != QLowEnergyController::UnconnectedState)
    {
        controller->disconnectFromDevice();
    }


}

void BLEDOMController::serviceScanDone()
{
    qDebug("BLEDOM: Service Scan has Finished!");
    if(lightService == nullptr)
    {
        emit Errored(tr("This BLEDOM device doesn't seem to expose a compatible service."));
    }
}


void BLEDOMController::serviceDiscovered(const QBluetoothUuid &newService)
{
    qDebug() << "BLEDOM: Discovered Service - " << newService;
    if(newService == *BLEDOMController::LightService)
    {
        qDebug("Found Light Service");
        lightService = controller->createServiceObject(*BLEDOMController::LightService, this);
        connect(lightService, &QLowEnergyService::stateChanged, this, &BLEDOMController::serviceStateChanged);
        lightService->discoverDetails();
    }
}
void BLEDOMController::serviceStateChanged(QLowEnergyService::ServiceState s)
{
    switch (s) {
    case QLowEnergyService::DiscoveringServices:
        qDebug() << tr("Discovering services...");
        break;
    case QLowEnergyService::ServiceDiscovered:
    {
        qDebug() << (tr("Service discovered."));

        controlCharacteristic = lightService->characteristic(*BLEDOMController::ControlCharacteristic);
        if (!controlCharacteristic.isValid())
        {
            qInfo() << ("Control Port not found.");
            emit Errored(tr("This BLEDOM device doesn't seem to be compatible! The Control Port was not found."));
            break;
        }
        ready = true;
        emit Ready();
        break;
    }
    default:
        //nothing for now
        break;
    }


}


void BLEDOMController::Connect(){
    controller->connectToDevice();
}

QFuture<void> BLEDOMController::PerformTest()
{
    return QtConcurrent::run([this](){
        int count = 0;
        while(count < 6){
            count += 1;
            qDebug() << "Test Count is now " << count;
            QThread::msleep(500);
            SetPower(!_poweredOn);
        }
    });
}

std::string BLEDOMController::GetBluetoothAddress()
{
    return controller->remoteAddress().toString().toStdString();
}

void BLEDOMController::SetColor(unsigned char r, unsigned char g, unsigned char b)
{

    SendCommand(0x05, 0x03, r,g,b);


}

void BLEDOMController::SetPower(bool on)
{
    if(on)
    {
        qDebug() << "Powering On";
        SendCommand(0x04, 0xf0, 0, 0x01, 0xff);
        _poweredOn = true;
    } else {
        qDebug() << "Powering Off";
        SendCommand(0x04, 0,0,0,0xff);
        _poweredOn = false;
    }
}

void BLEDOMController::SetBrightness(unsigned char brightness)
{
    if(brightness == 0)
    {
        return SetPower(false);
    } else if (!_poweredOn)
    {
        SetPower(true);
    }
    SendCommand(0x01, brightness, 0, 0, 0);
}

void BLEDOMController::SetEffect(Effect effect)
{
    SendCommand(0x03, effect, 0x03, 0, 0);
}

void BLEDOMController::SetEffectSpeed(unsigned char effectSpeed)
{
    SendCommand(0x02, qMin<unsigned char>(0x64, effectSpeed), 0, 0, 0);
}

void BLEDOMController::SendCommand(unsigned char command, unsigned char param1, unsigned char param2, unsigned char param3, unsigned char param4)
{
    if(!controlCharacteristic.isValid())
    {
        qInfo() << "BLEDOM Connection has dropped. Attempting to reconnect";
        this->Connect();
        QObject* ctx = new QObject();
        connect(this, &BLEDOMController::Ready, ctx, [=]() {
            // destroy the context/receiver to disconnect

            unsigned char buffer[9] = {0x7E, 0x00, command, param1, param2, param3, param4, 0, 0xEF};
            QByteArray message = QByteArray::fromRawData((char*)buffer, 9);
            lightService->writeCharacteristic(controlCharacteristic, message);
            ctx->deleteLater();

        });
        return;
    }
    unsigned char buffer[9] = {0x7E, 0x00, command, param1, param2, param3, param4, 0, 0xEF};
    QByteArray message = QByteArray::fromRawData((char*)buffer, 9);
    lightService->writeCharacteristic(controlCharacteristic, message);

}





