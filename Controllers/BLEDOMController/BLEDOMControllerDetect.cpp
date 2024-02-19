#include "Detector.h"
#include "BLEDOMController.h"
#include "RGBController.h"
#include "RGBController_BLEDOM.h"
#include "LogManager.h"
#include <QEventLoop>
#include <QMessageBox>
#include <QTimer>
void DetectBLEDOMControllers()
{
    QList<QBluetoothDeviceInfo> savedDevices = BLEDOMController::GetSavedDevices();
    foreach(QBluetoothDeviceInfo device, savedDevices){
        QEventLoop loop;
        bool resolved = false;
        BLEDOMController* controller = new BLEDOMController(device);
        QTimer timeoutTimer;
        QObject* ctx = new QObject();
        QObject::connect(controller, &BLEDOMController::Ready, ctx, [=, &loop, &resolved]() {

            LOG_INFO("BLEDOM Connected.");
            RGBController_BLEDOM* rgb_controller = new RGBController_BLEDOM(controller);

            ResourceManager::get()->RegisterRGBController(rgb_controller);

            ctx->deleteLater();
            resolved = true;
            loop.quit();

        });
        QObject::connect(controller, &BLEDOMController::Errored, ctx, [&loop, ctx, controller, &resolved](QString error) {


            LOG_WARNING("Connection to BLEDOM failed. %s", error.toStdString().c_str());
            delete controller;
            ctx->deleteLater();
            resolved = true;
            loop.quit();

        });


        controller->Connect();
        QTimer::singleShot(20000, ctx, [&loop, ctx, controller, &resolved](){
            if(!resolved){
                LOG_WARNING("Connection to BLEDOM failed. Device Timed out.");
                delete controller;
                ctx->deleteLater();
                loop.quit();
            }
        });
        loop.exec();
    }
}


REGISTER_DETECTOR("BLEDOM", DetectBLEDOMControllers);
