#ifndef OPENRGBDIALOG2_H
#define OPENRGBDIALOG2_H

#include "ui_OpenRGBDialog2.h"

#include <vector>
#include "i2c_smbus.h"
#include "RGBController.h"
#include "RGBGroupController.h"
#include "OpenRGBDevicePage.h"
#include "ProfileManager.h"
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
    explicit OpenRGBDialog2(std::vector<i2c_smbus_interface *>& bus, std::vector<RGBController *>& control, ProfileManager* manager, NetworkServer* server, bool show_i2c_tools, QWidget *parent = 0);
    ~OpenRGBDialog2();

    void show();
    void setMode(unsigned char mode_val);

protected:
    std::vector<i2c_smbus_interface *>& busses;
    std::vector<RGBController *>&       controllers;
    ProfileManager*                     profile_manager;
    NetworkServer*                      network_server;

private:
    Ui::OpenRGBDialog2Ui                *ui;
    QSystemTrayIcon*                    trayIcon;
    QMenu*                              profileMenu;
    QMenu*                              contextMenu;
    QTabWidget*                         hiddenTabs; //QTabBar
    QLineEdit*                          qleGroupName;
    QListWidget*                        qliController;
    void RefreshProfileList();
    void contextMenuEvent(QContextMenuEvent *event);

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
