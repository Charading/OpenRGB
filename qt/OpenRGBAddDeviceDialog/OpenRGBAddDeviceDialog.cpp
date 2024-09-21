#include "OpenRGBAddDeviceDialog.h"
#include "ui_OpenRGBAddDeviceDialog.h"

// Device entries
#include "OpenRGBDMXSettingsPage/OpenRGBDMXSettingsEntry.h"
#include "OpenRGBE131SettingsPage/OpenRGBE131SettingsEntry.h"
#include "OpenRGBElgatoKeyLightSettingsPage/OpenRGBElgatoKeyLightSettingsEntry.h"
#include "OpenRGBElgatoLightStripSettingsPage/OpenRGBElgatoLightStripSettingsEntry.h"
#include "OpenRGBKasaSmartSettingsPage/OpenRGBKasaSmartSettingsEntry.h"
#include "OpenRGBLIFXSettingsPage/OpenRGBLIFXSettingsEntry.h"
#include "OpenRGBPhilipsHueSettingsPage/OpenRGBPhilipsHueSettingsEntry.h"
#include "OpenRGBPhilipsWizSettingsPage/OpenRGBPhilipsWizSettingsEntry.h"
#include "OpenRGBQMKORGBSettingsPage/OpenRGBQMKORGBSettingsEntry.h"
#include "OpenRGBSerialSettingsPage/OpenRGBSerialSettingsEntry.h"
#include "OpenRGBYeelightSettingsPage/OpenRGBYeelightSettingsEntry.h"
#include "OpenRGBNanoleafSettingsPage/OpenRGBNanoleafSettingsEntry.h"

#include "OpenRGBFont.h"

#include <QDesktopServices>

using namespace Ui;

static void DetectionProgressChangedCallback(void* param)
{
    OpenRGBAddDeviceDialog* this_obj = (OpenRGBAddDeviceDialog*)(param);

    QMetaObject::invokeMethod(this_obj, "onDetectionProgressChanged", Qt::QueuedConnection);
}

static void DetectionEndedCallback(void* param)
{
    OpenRGBAddDeviceDialog* this_obj = (OpenRGBAddDeviceDialog*)(param);

    QMetaObject::invokeMethod(this_obj, "onDetectionEnded", Qt::QueuedConnection);
}

OpenRGBAddDeviceDialog::OpenRGBAddDeviceDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new OpenRGBAddDeviceDialogUi)
{
    ui->setupUi(this);

    detectorTableModel = new DetectorTableModel;
    detectorSortModel = new QSortFilterProxyModel;

    detectorSortModel->setSourceModel(detectorTableModel);
    ui->TableSupported->setModel(detectorSortModel);
    ui->TableSupported->verticalHeader()->setVisible(0);
    ui->TableSupported->setSortingEnabled(true);
    ui->TableSupported->sortByColumn(0, Qt::AscendingOrder);
    ui->TableSupported->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);

    ui->stackedWidget->setCurrentWidget(ui->PageDeviceType);

    ResourceManager* rm = ResourceManager::get();
    rm->RegisterDetectionEndCallback(&DetectionEndedCallback, this);
    rm->RegisterDetectionProgressCallback(&DetectionProgressChangedCallback, this);
}

OpenRGBAddDeviceDialog::~OpenRGBAddDeviceDialog()
{
    ResourceManager* rm = ResourceManager::get();
    rm->UnregisterDetectionEndCallback(&DetectionEndedCallback, this);
    rm->UnregisterDetectionProgressCallback(&DetectionProgressChangedCallback, this);

    delete ui;
    delete detectorSortModel;
    delete detectorTableModel;
    detectorSortModel = nullptr;
    detectorTableModel = nullptr;
}

void OpenRGBAddDeviceDialog::showSupported(int deviceType)
{
    ui->stackedWidget->slideInWgt(ui->PageSupportedList, SlidingStackedWidget::RIGHT2LEFT);
}

void OpenRGBAddDeviceDialog::setScanButtons(bool scan)
{
    ui->progressBar->setVisible(scan);

    ui->ButtonScanBack->setVisible(!scan);
    ui->ButtonScanRescan->setVisible(!scan);
    ui->ButtonScanNotFound->setVisible(!scan);
    if(scan)
    {
        ui->ButtonScanBeginning->hide();
        ui->ButtonScanClose->hide();
    }
}

void OpenRGBAddDeviceDialog::home()
{
    deviceType = -1;
    scanFromPage = -1;
    helpFromPage = -1;
    configureFromPage = -1;
    ui->stackedWidget->slideInWgt(ui->PageDeviceType, SlidingStackedWidget::LEFT2RIGHT);
}

void OpenRGBAddDeviceDialog::rescan()
{
    if(scanFromPage == -1)
    {
        // Save the index of the page we came from
        scanFromPage = ui->stackedWidget->currentIndex();

        // Cache the names of all devices that were found so far
        cachedDevices.clear();
        std::vector<RGBController*> controllers = ResourceManager::get()->GetRGBControllers();
        for(int i = 0; i < (int)controllers.size(); ++i)
        {
            cachedDevices.push_back(controllers[i]->name);
        }
    }
    ui->LabelScan->setText(tr("<h2>Re-scanning devices...</h2>"));
    setScanButtons(true);

    ui->stackedWidget->slideInWgt(ui->PageScan, SlidingStackedWidget::RIGHT2LEFT);

    ResourceManager::get()->DetectDevices();
}

void OpenRGBAddDeviceDialog::help()
{
    if(helpFromPage == -1)
    {
        helpFromPage = ui->stackedWidget->currentIndex();
    }

    // Show the "Nothing we can do" page
    // The usefulness on it heavily depends on the contents of the "Issue Text" field
    // The caller must fill it with appropriate text (device IDs, connection type, manufacturer, etc.)
    if(ui->helpIssueText->toPlainText().isEmpty())
    {
        ui->helpIssueText->hide();
        ui->ButtonClipboard->hide();
        ui->LabelGitlab->setText(tr("We would really appreciate if you could file an issue on <a href=\"gitlab\">Gitlab</a>, providing the device name and a link to a description on manufacturer's website."));
    }
    else
    {
        ui->helpIssueText->show();
        ui->ButtonClipboard->show();
        ui->LabelGitlab->setText(tr("We would really appreciate if you could send us this info, by filing an issue on Gitlab. "
                                    "You can do that by clicking the button below (you need to be logged in to Gitlab in your "
                                    "default web-browser), or by navigating to the <a href=\"gitlab\">Project's page on Gitlab</a>, "
                                    "then \"Issues\", clicking \"New issue\", and pasting in the text from the field above."));
    }

    ui->stackedWidget->slideInWgt(ui->PageHelp, SlidingStackedWidget::RIGHT2LEFT);
}

void OpenRGBAddDeviceDialog::configure(QWidget* entry)
{
    configureFromPage = ui->stackedWidget->currentIndex();

    delete ui->ConfigureEntry;
    ui->ConfigureEntry = entry;
    ui->PageConfigureLayout->insertWidget(1, entry);
    ui->stackedWidget->slideInWgt(ui->PageConfigure, SlidingStackedWidget::RIGHT2LEFT);
}

// ===== DEVICE TYPE =====

void OpenRGBAddDeviceDialog::on_ButtonUSB_clicked()
{
    deviceType = 1; // USB
    ui->ButtonQMK->show();
    showSupported();
}

void OpenRGBAddDeviceDialog::on_ButtonMotherboard_clicked()
{
    deviceType = 2; // Motherboard
    ui->ButtonQMK->hide();
    showSupported();
}

void OpenRGBAddDeviceDialog::on_ButtonGPU_clicked()
{
    deviceType = 3; // GPU
    ui->ButtonQMK->hide();
    showSupported();
}

void OpenRGBAddDeviceDialog::on_ButtonRAM_clicked()
{
    deviceType = 4; // RAM
    ui->ButtonQMK->hide();
    showSupported();
}

void OpenRGBAddDeviceDialog::on_ButtonLaptop_clicked()
{
    deviceType = 5; // Laptop
    ui->ButtonQMK->hide();
    showSupported();
}

void OpenRGBAddDeviceDialog::on_ButtonNetwork_clicked()
{
    deviceType = 6; // Network
    ui->stackedWidget->slideInWgt(ui->PageNetworkType, SlidingStackedWidget::RIGHT2LEFT);
}

void OpenRGBAddDeviceDialog::on_ButtonSerial_clicked()
{
    deviceType = 7; // Serial
    configure(new OpenRGBSerialSettingsEntry);
}

void OpenRGBAddDeviceDialog::on_ButtonDMX_clicked()
{
    deviceType = 8; // DMX
    configure(new OpenRGBDMXSettingsEntry);
}

void OpenRGBAddDeviceDialog::on_ButtonQMK_clicked()
{
    deviceType = 9; // QMK
    configure(new OpenRGBQMKORGBSettingsEntry);
}

// ===== USB TYPE (UNUSED) =====

void OpenRGBAddDeviceDialog::on_ButtonUSBTypeBack_clicked()
{
    ui->stackedWidget->slideInWgt(ui->PageDeviceType, SlidingStackedWidget::LEFT2RIGHT);
}

// ===== NETWORK =====

void OpenRGBAddDeviceDialog::on_ButtonE131_clicked()
{
    delete ui->ConfigureEntry;
    ui->ConfigureEntry = new OpenRGBE131SettingsEntry;
    ui->stackedWidget->slideInWgt(ui->PageConfigure, SlidingStackedWidget::RIGHT2LEFT);
}

void OpenRGBAddDeviceDialog::on_ButtonKasaSmart_clicked()
{
    configure(new OpenRGBKasaSmartSettingsEntry);
}

void OpenRGBAddDeviceDialog::on_ButtonLIFX_clicked()
{
    configure(new OpenRGBLIFXSettingsEntry);
}

void OpenRGBAddDeviceDialog::on_ButtonPhilipsHue_clicked()
{
    configure(new OpenRGBPhilipsHueSettingsEntry);
}

void OpenRGBAddDeviceDialog::on_ButtonPhilipsWiz_clicked()
{
    configure(new OpenRGBPhilipsWizSettingsEntry);
}

void OpenRGBAddDeviceDialog::on_ButtonYeelight_clicked()
{
    configure(new OpenRGBYeelightSettingsEntry);
}

void OpenRGBAddDeviceDialog::on_ButtonNanloeaf_clicked()
{
    configure(new OpenRGBNanoleafSettingsEntry);
}

void OpenRGBAddDeviceDialog::on_ButtonElgatoKeyLight_clicked()
{
    configure(new OpenRGBElgatoKeyLightSettingsEntry);
}

void OpenRGBAddDeviceDialog::on_ButtonElgatoLightStrip_clicked()
{
    configure(new OpenRGBElgatoLightStripSettingsEntry);
}

void OpenRGBAddDeviceDialog::on_ButtonNetworkBack_clicked()
{
    ui->stackedWidget->slideInWgt(ui->PageDeviceType, SlidingStackedWidget::LEFT2RIGHT);
}

void OpenRGBAddDeviceDialog::on_ButtonNetworkNotFound_clicked()
{
    ui->helpIssueText->setPlainText("");
    help();
}

// ===== SUPPORTED =====

void OpenRGBAddDeviceDialog::on_ButtonSupportedBack_clicked()
{
    ui->stackedWidget->slideInWgt(ui->PageDeviceType, SlidingStackedWidget::LEFT2RIGHT);
}

void OpenRGBAddDeviceDialog::on_ButtonSupportedNotFound_clicked()
{
    help();
}

void OpenRGBAddDeviceDialog::on_ButtonSupportedRescan_clicked()
{
    detectorTableModel->applySettings();
    rescan();
}

// ===== CONFIGURE =====

void OpenRGBAddDeviceDialog::on_ButtonConfigureBack_clicked()
{
    ui->stackedWidget->slideInIdx(configureFromPage, SlidingStackedWidget::LEFT2RIGHT);
    configureFromPage = -1;
}

void OpenRGBAddDeviceDialog::on_ButtonConfigureScan_clicked()
{
    rescan();
}

// ===== SCAN =====

void OpenRGBAddDeviceDialog::on_ButtonScanBack_clicked()
{
    ui->stackedWidget->slideInIdx(scanFromPage, SlidingStackedWidget::LEFT2RIGHT);
    scanFromPage = -1;
}

void OpenRGBAddDeviceDialog::on_ButtonScanRescan_clicked()
{
    rescan();
}

void OpenRGBAddDeviceDialog::on_ButtonScanNotFound_clicked()
{
    if(deviceType == 1 || deviceType == 2 || deviceType == 5)
    {
        // For those types where the device CAN be present in IDs, we list IDs
        ui->stackedWidget->slideInWgt(ui->PageIDs, SlidingStackedWidget::RIGHT2LEFT);
    }
    else
    {
        // Otherwise, we show "Nothing we can do" page
        ui->helpIssueText->setPlainText("");
        help();
    }
}

void OpenRGBAddDeviceDialog::on_ButtonScanBeginning_clicked()
{
    home();
}

void OpenRGBAddDeviceDialog::on_ButtonScanClose_clicked()
{
    close();
}

// ===== HELP =====

void OpenRGBAddDeviceDialog::on_ButtonHelpBack_clicked()
{
    ui->stackedWidget->slideInIdx(helpFromPage, SlidingStackedWidget::LEFT2RIGHT);
    helpFromPage = -1;
}

void OpenRGBAddDeviceDialog::on_ButtonHelpIssue_clicked()
{
    QByteArray issueText = ui->helpIssueText->toPlainText().toUtf8();
    QString issueUrl("https://gitlab.com/CalcProgrammer1/OpenRGB/-/issues/new");
    QByteArray issueTitle = "[New Device][Helper]";
    if(!title.empty())
    {
        issueTitle.append(" ");
        issueTitle.append(title.c_str());
    }
    if(!issueText.isEmpty())
    {
        issueUrl.append("?issue[title]=");
        issueUrl.append(issueTitle.toPercentEncoding());
        issueUrl.append("&issue[description]=");
        issueUrl.append(issueText.toPercentEncoding());
    }
    QDesktopServices::openUrl (QUrl(issueUrl));
}

void Ui::OpenRGBAddDeviceDialog::on_ButtonHelpBeginning_clicked()
{
    home();
}

void OpenRGBAddDeviceDialog::on_LabelGitlab_linkActivated(const QString &link)
{
    QString issueUrl("https://gitlab.com/CalcProgrammer1/OpenRGB/-/issues/new");
    QDesktopServices::openUrl (QUrl(issueUrl));
}

void OpenRGBAddDeviceDialog::on_Filter_textChanged(const QString &arg1)
{
#ifdef _QT6
    detectorSortModel->setFilterRegularExpression(QRegularExpression(arg1 , QRegularExpression::CaseInsensitiveOption));
#else
    detectorSortModel->setFilterRegExp(QRegExp(arg1, Qt::CaseInsensitive));
#endif
}


void OpenRGBAddDeviceDialog::onDetectionProgressChanged()
{
    ui->progressBar->setValue(ResourceManager::get()->GetDetectionPercent());
    ui->progressBar->setFormat(QString::fromStdString(ResourceManager::get()->GetDetectionString()));

    if(ResourceManager::get()->GetDetectionPercent() != 100)
    {
        setScanButtons(true);
    };
}

void OpenRGBAddDeviceDialog::onDetectionEnded()
{
    setScanButtons(false);
    // Set text

    std::vector<RGBController*> controllers = ResourceManager::get()->GetRGBControllers();
    if(controllers.empty())
    {
        ui->LabelScan->setText(tr("<h2>Still nothing O_O</h2>"
                                  "Scan reported no devices"));
    }
    else
    {
        QString text = tr("<h2>Scan complete</h2>");
        bool foundNew = false;
        for(int i = 0; i < (int)controllers.size(); ++i)
        {
            std::vector<std::string>::iterator it = std::find(cachedDevices.begin(), cachedDevices.end(), controllers[i]->name);
            if(it == cachedDevices.end())
            {
                if(!foundNew)
                {
                    text.append(tr("New devices that were found:<ul>"));
                }
                foundNew = true;

                text.append("<li>");
                text.append(OpenRGBFont::GetIconFromDeviceType(controllers[i]->type));
                text.append(" ");
                text.append(controllers[i]->name.c_str());
                text.append("</li>");
            }
            if(!foundNew)
            {
                text.append(tr("No new devices were found. Please go back and check the parameters, or file a new device issue."));
            }
            else
            {
                text.append("</ul>");

                // Detection has gained results - show buttons to "complete" the hunt
                ui->ButtonScanBeginning->show();
                ui->ButtonScanClose->show();
            }
            ui->LabelScan->setText(text);

        }
    }
}

void Ui::OpenRGBAddDeviceDialog::on_ButtonIDsBack_clicked()
{
    ui->stackedWidget->slideInIdx(scanFromPage, SlidingStackedWidget::LEFT2RIGHT);
    scanFromPage = -1; // We can only get to Hardware IDs if scan has failed, so we return to before the scan
}

void Ui::OpenRGBAddDeviceDialog::on_ButtonIDsNotFound_clicked()
{
    help();
}
