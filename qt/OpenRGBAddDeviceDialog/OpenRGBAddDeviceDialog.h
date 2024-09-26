#ifndef OPENRGBADDDEVICEDIALOG_H
#define OPENRGBADDDEVICEDIALOG_H

/*---------------------------------------------------------*\
| OpenRGBConsolePage.h                                      |
|                                                           |
|   Device adding helper window & troubleshooting wizard    |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-only                   |
\*---------------------------------------------------------*/

#include "RGBController.h"
#include "DetectorTableModel.h"

#include <ui_OpenRGBAddDeviceDialog.h>
#include <QDialog>
#include <QSortFilterProxyModel>

namespace Ui {
class OpenRGBAddDeviceDialog;
}

class Ui::OpenRGBAddDeviceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OpenRGBAddDeviceDialog(QWidget *parent = nullptr);
    ~OpenRGBAddDeviceDialog();

private slots:
    void on_ButtonUSB_clicked();
    void on_ButtonMotherboard_clicked();
    void on_ButtonGPU_clicked();
    void on_ButtonRAM_clicked();
    void on_ButtonLaptop_clicked();
    void on_ButtonNetwork_clicked();
    void on_ButtonSerial_clicked();
    void on_ButtonDMX_clicked();
    void on_ButtonQMK_clicked();

    void on_ButtonUSBTypeBack_clicked();

    void on_ButtonE131_clicked();
    void on_ButtonKasaSmart_clicked();
    void on_ButtonLIFX_clicked();
    void on_ButtonPhilipsHue_clicked();
    void on_ButtonPhilipsWiz_clicked();
    void on_ButtonYeelight_clicked();
    void on_ButtonNanloeaf_clicked();
    void on_ButtonElgatoKeyLight_clicked();
    void on_ButtonElgatoLightStrip_clicked();
    void on_ButtonNetworkBack_clicked();
    void on_ButtonNetworkNotFound_clicked();

    void on_Filter_textChanged(const QString &arg1);
    void on_ButtonSupportedBack_clicked();
    void on_ButtonSupportedNotFound_clicked();
    void on_ButtonSupportedRescan_clicked();

    void on_ButtonConfigureBack_clicked();
    void on_ButtonConfigureScan_clicked();

    void on_ButtonScanBack_clicked();
    void on_ButtonScanRescan_clicked();
    void on_ButtonScanNotFound_clicked();
    void on_ButtonScanBeginning_clicked();
    void on_ButtonScanClose_clicked();

    void on_ButtonHelpBack_clicked();
    void on_ButtonHelpIssue_clicked();
    void on_ButtonHelpBeginning_clicked();
    void on_LabelGitlab_linkActivated(const QString &link);

    void onDetectionProgressChanged();
    void onDetectionEnded();

    void on_ButtonIDsBack_clicked();

    void on_ButtonIDsNotFound_clicked();


private:
    Ui::OpenRGBAddDeviceDialogUi *ui;
    void home();
    void rescan();
    void help();
    void showSupported(int deviceType = -1);
    void setScanButtons(bool scan);
    void configure(QWidget* entry);

    DetectorTableModel*    detectorTableModel;
    QSortFilterProxyModel* detectorSortModel;
    int deviceType = -1;
    int scanFromPage = -1;
    int helpFromPage = -1;
    int configureFromPage = -1;

    std::string title; // Cached device name & manufacturer from Hardware IDs
    std::vector<std::string> cachedDevices;
};

#endif // OPENRGBADDDEVICEDIALOG_H
