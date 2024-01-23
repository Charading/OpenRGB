#include "BLEDOMController.h"
#include <QTimer>
#include <QtConcurrent/QtConcurrent>
#include "ResourceManager.h"
#include "SettingsManager.h"
#include "LogManager.h"

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
    json bledom_settings = ResourceManager::get()->GetSettingsManager()->GetSettings("BleDomDevices");
    if(bledom_settings.contains("devices"))
    {
        for(unsigned int device_idx = 0; device_idx < bledom_settings["devices"].size(); device_idx++)
        {
            result.append(BLEDOMController::DeviceFromData(
                          QString::fromStdString(bledom_settings["devices"][device_idx]["address"]),
                          QString::fromStdString(bledom_settings["devices"][device_idx]["name"]),
                          bledom_settings["devices"][device_idx]["class"]
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
                LOG_WARNING("Cannot connect to remote device.");
                emit Errored(tr("Cannot connect to remote BLEDOM device %1.").arg(device.address().toString()));
                this->ready = false;
            });
    connect(controller, &QLowEnergyController::connected, this, [this]() {
        LOG_DEBUG("Controller connected. Search services...");
        controller->discoverServices();
    });
    connect(controller, &QLowEnergyController::disconnected, this, [this]() {
        LOG_INFO("LowEnergy controller disconnected");
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
    LOG_DEBUG("BLEDOM: Service Scan has Finished!");
    if(lightService == nullptr)
    {
        emit Errored(tr("This BLEDOM device doesn't seem to expose a compatible service."));
    }
}


void BLEDOMController::serviceDiscovered(const QBluetoothUuid &newService)
{
    LOG_DEBUG("BLEDOM: Discovered Service - %s", newService.toString().toStdString().c_str());
    if(newService == *BLEDOMController::LightService)
    {
        LOG_DEBUG("Found Light Service");
        lightService = controller->createServiceObject(*BLEDOMController::LightService, this);
        connect(lightService, &QLowEnergyService::stateChanged, this, &BLEDOMController::serviceStateChanged);
        lightService->discoverDetails();
    }
}
void BLEDOMController::serviceStateChanged(QLowEnergyService::ServiceState s)
{
    switch (s) {
    case QLowEnergyService::DiscoveringServices:
        LOG_DEBUG("BLEDOM: Discovering services...");
        break;
    case QLowEnergyService::ServiceDiscovered:
    {
        LOG_DEBUG("Service discovered.");

        controlCharacteristic = lightService->characteristic(*BLEDOMController::ControlCharacteristic);
        if (!controlCharacteristic.isValid())
        {
            LOG_INFO("Control Port not found.");
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
        LOG_DEBUG("Powering On");
        SendCommand(0x04, 0xf0, 0, 0x01, 0xff);
        _poweredOn = true;
    } else {
        LOG_DEBUG("Powering Off");
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
        LOG_INFO("BLEDOM Connection has dropped. Attempting to reconnect");
        LOG_INFO("Controller State is: %i", controller->state());

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





