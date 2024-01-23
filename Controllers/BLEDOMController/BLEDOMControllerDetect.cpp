#include "Detector.h"
#include "BLEDOMController.h"
#include "RGBController.h"
#include "RGBController_BLEDOM.h"
#include <QEventLoop>
#include <QMessageBox>
void DetectBLEDOMControllers()
{
    QList<QBluetoothDeviceInfo> savedDevices = BLEDOMController::GetSavedDevices();
    foreach(QBluetoothDeviceInfo device, savedDevices){
        QEventLoop loop;
        BLEDOMController* controller = new BLEDOMController(device);

        QObject* ctx = new QObject();
        QObject::connect(controller, &BLEDOMController::Ready, ctx, [=, &loop]() {

            qInfo() << "BLEDOM Connected.";
            RGBController_BLEDOM* rgb_controller = new RGBController_BLEDOM(controller);

            ResourceManager::get()->RegisterRGBController(rgb_controller);

            ctx->deleteLater();
            loop.quit();

        });
        QObject::connect(controller, &BLEDOMController::Errored, ctx, [&loop, ctx](QString error) {


            qWarning() << "Connection to BLEDOM failed. " << error;
            ctx->deleteLater();
            loop.quit();

        });

        controller->Connect();
        loop.exec();
    }
}


REGISTER_DETECTOR("BLEDOM", DetectBLEDOMControllers);
