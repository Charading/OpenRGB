#ifndef OPENRGBCLIENTINFOPAGE_H
#define OPENRGBCLIENTINFOPAGE_H

#include "RGBController.h"
#include "ui_OpenRGBClientInfoPage.h"
#include "NetworkClient.h"

#include <QFrame>
#include <thread>

namespace Ui {
class OpenRGBClientInfoPage;
}

class Ui::OpenRGBClientInfoPage : public QFrame
{
    Q_OBJECT

public:
    explicit OpenRGBClientInfoPage(QWidget *parent = nullptr);
    ~OpenRGBClientInfoPage();

    void AddClient(NetworkClient* new_client);

    void FanUpdateThreadFunction();

public slots:
    void UpdateInfo();

private slots:
    void on_ClientConnectButton_clicked();
    void onClientDisconnectButton_clicked(QObject * arg);

private:
    void UpdateFanReadings();

    Ui::OpenRGBClientInfoPageUi *ui;

    bool fan_thread_running;
    std::thread* FanUpdateThread;

signals:
    void ClientListUpdated();
};

#endif // OPENRGBCLIENTINFOPAGE_H
