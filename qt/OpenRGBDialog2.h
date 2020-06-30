#ifndef OPENRGBDIALOG2_H
#define OPENRGBDIALOG2_H

#include "ui_OpenRGBDialog2.h"

#include "OpenRGBClientInfoPage.h"
#include "OpenRGBSoftwareInfoPage.h"
#include "OpenRGBSystemInfoPage.h"

#include <vector>
#include "i2c_smbus.h"
#include "RGBController.h"
#include "RGBGroupController.h"
#include "OpenRGBDevicePage.h"
#include "ProfileManager.h"
#include "NetworkClient.h"
#include "NetworkServer.h"

#include <QMainWindow>
#include <QTimer>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QListWidgetItem>

namespace Ui
{
    class OpenRGBDialog2;
}

class Ui::OpenRGBDialog2 : public QMainWindow
{
    Q_OBJECT

public:
    explicit OpenRGBDialog2(std::vector<i2c_smbus_interface *>& bus, std::vector<RGBController *>& control, ProfileManager* manager, QWidget *parent = 0);
    ~OpenRGBDialog2();

    void AddClient(NetworkClient* new_client);
    void AddClientTab();
    void AddI2CToolsPage();
    void AddServerTab(NetworkServer* network_server);

    void setMode(unsigned char mode_val);

protected:
    std::vector<i2c_smbus_interface *>& busses;
    std::vector<RGBController *>&       controllers;
    ProfileManager*                     profile_manager;

private:
    /*-------------------------------------*\
    | Page pointers                         |
    \*-------------------------------------*/
    OpenRGBClientInfoPage *ClientInfoPage;
    OpenRGBSystemInfoPage *SMBusToolsPage;
    OpenRGBSoftwareInfoPage *SoftInfoPage;

    /*-------------------------------------*\
    | System tray icon and menu             |
    \*-------------------------------------*/
    QSystemTrayIcon* trayIcon;
    QMenu* profileMenu;

    /*-------------------------------------*\
    | Group Controller interface            |
    \*-------------------------------------*/
    QMenu*                              contextMenu;
    QTabWidget*                         hiddenTabs; //QTabBar
    QLineEdit*                          qleGroupName;
    QListWidget*                        qliController;
    void contextMenuEvent(QContextMenuEvent *event);

    /*-------------------------------------*\
    | User interface                        |
    \*-------------------------------------*/
    Ui::OpenRGBDialog2Ui *ui;

    void ClearDevicesList();
    void UpdateDevicesList();
    void UpdateProfileList();

private slots:
    void on_Exit();
    void on_LightsOff();
    void on_QuickRed();
    void on_QuickYellow();
    void on_QuickGreen();
    void on_QuickCyan();
    void on_QuickBlue();
    void on_QuickMagenta();
    void on_QuickWhite();
    void on_ClientListUpdated();
    void on_SetAllDevices(unsigned char red, unsigned char green, unsigned char blue);
    void on_SaveSizeProfile();
    void on_ShowHide();
    void on_ProfileSelected();
    void on_ButtonSaveProfile_clicked();
    void on_ButtonLoadProfile_clicked();
    void on_ButtonDeleteProfile_clicked();
    void on_GroupController();
    void on_UngroupControllers();
    void on_GroupSelected();
};

#endif // OPENRGBDIALOG2_H
