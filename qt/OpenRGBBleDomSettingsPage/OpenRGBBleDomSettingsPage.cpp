#include "OpenRGBBleDomSettingsPage.h"
#include "ui_OpenRGBBleDomSettingsPage.h"
#include "ResourceManager.h"
#include "SettingsManager.h"
#include <QMessageBox>
#include <QCheckBox>
#include <QDebug>
#include "../Controllers/BLEDOMController/BLEDOMController.h"
using namespace Ui;


OpenRGBBleDomSettingsPage::OpenRGBBleDomSettingsPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::OpenRGBBleDomSettingsPageUi)
{
    ui->setupUi(this);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);

    // Setup Bluetooth Discovery Agent
    m_deviceDiscoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
    m_deviceDiscoveryAgent->setLowEnergyDiscoveryTimeout(15000);
    connect(m_deviceDiscoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered,
            this, &OpenRGBBleDomSettingsPage::deviceDetected);

    connect(m_deviceDiscoveryAgent, static_cast<void (QBluetoothDeviceDiscoveryAgent::*)(QBluetoothDeviceDiscoveryAgent::Error)>(&QBluetoothDeviceDiscoveryAgent::error),
            this, &OpenRGBBleDomSettingsPage::scanError);

    connect(m_deviceDiscoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished,
            this, &OpenRGBBleDomSettingsPage::scanFinished);
    connect(m_deviceDiscoveryAgent, &QBluetoothDeviceDiscoveryAgent::canceled,
            this, &OpenRGBBleDomSettingsPage::scanFinished);

    // Initialize view from saved data
    QList<QBluetoothDeviceInfo> savedDevices = BLEDOMController::GetSavedDevices();
    foreach (QBluetoothDeviceInfo device, savedDevices) {
        AddDevice(device, true);
    }

}

void OpenRGBBleDomSettingsPage::AddDevice(QBluetoothDeviceInfo device, bool addAsChecked)
{
    devices.append(device);
    int index = devices.length() - 1;
    ui->tableWidget->insertRow ( ui->tableWidget->rowCount() );
    ui->tableWidget->setItem(index, 0, new QTableWidgetItem(device.name() + " " + device.address().toString()));
    QPushButton* testButton = new QPushButton(tr("Test"));
    connect(testButton, &QPushButton::clicked, testButton, [device]()
            {
                qDebug("Testing Device");
                BLEDOMController* controller = new BLEDOMController(device);
                controller->Connect();
                QObject* ctx = new QObject();
                connect(controller, &BLEDOMController::Ready, ctx, [=]() {
                    // destroy the context/receiver to disconnect

                    controller->PerformTest().waitForFinished();
                    ctx->deleteLater();
                    delete controller;
                });

            });
    ui->tableWidget->setCellWidget(index, 1, testButton);
    QWidget * uiCheckbox = new QWidget();
    QHBoxLayout* checkBoxLayout = new QHBoxLayout(this);
    QCheckBox* useCheckbox = new QCheckBox(this);
    if(addAsChecked) useCheckbox->setChecked(true);
    checkBoxLayout->setAlignment( Qt::AlignCenter );
    checkBoxLayout->addWidget(useCheckbox, 1, Qt::AlignHCenter);
    uiCheckbox->setLayout(checkBoxLayout);
    ui->tableWidget->setCellWidget(index, 2, uiCheckbox);
}


OpenRGBBleDomSettingsPage::~OpenRGBBleDomSettingsPage()
{
    delete m_deviceDiscoveryAgent;
    delete ui;
}

void OpenRGBBleDomSettingsPage::deviceDetected(const QBluetoothDeviceInfo device)
{
    qDebug() << "Found BLE Device: " << device.name();
    // Filter for BLEDOM.
    if((device.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration) && device.name().contains("BLEDOM")){
        qDebug() << "Found BLEDOM";
        AddDevice(device);
    }
}

void OpenRGBBleDomSettingsPage::scanError(QBluetoothDeviceDiscoveryAgent::Error error)
{
    QMessageBox::critical(this, tr("Scan Error"), tr("A Bluetooth error has been detected: %1").arg(error));
    ui->refreshBtn->setText("Refresh");
    ui->saveBtn->setDefault(false);
}

void OpenRGBBleDomSettingsPage::scanFinished()
{
    ui->refreshBtn->setText(tr("Refresh"));
    ui->saveBtn->setDefault(true);
}

void OpenRGBBleDomSettingsPage::on_refreshBtn_clicked()
{
    if(m_deviceDiscoveryAgent->isActive()){
        m_deviceDiscoveryAgent->stop();
    } else {
        ui->refreshBtn->setText(tr("Stop"));
        ui->saveBtn->setDefault(true);
        devices = {};
        ui->tableWidget->clearContents();
        ui->tableWidget->setRowCount(0);
        m_deviceDiscoveryAgent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
    }
}


void Ui::OpenRGBBleDomSettingsPage::on_saveBtn_clicked()
{
    json wiz_settings = ResourceManager::get()->GetSettingsManager()->GetSettings("BleDomDevices");
    wiz_settings["devices"].clear();

    for(unsigned int device_idx = 0; device_idx < devices.size(); device_idx++)
    {
        QWidget* selectionWidget = ui->tableWidget->cellWidget(device_idx, 2);
        auto cellChildren = selectionWidget->children();
        QCheckBox* uiUseCheckbox = (QCheckBox*)cellChildren[1];
        if(uiUseCheckbox->isChecked()){
            qDebug() << "BLEDOM: Using Device " << devices[device_idx].address();
            /*-------------------------------------------------*\
            | Required parameters                               |
            \*-------------------------------------------------*/
            QBluetoothDeviceInfo device = devices[device_idx];
            wiz_settings["devices"][device_idx]["address"] = device.address().toString().toStdString();
            wiz_settings["devices"][device_idx]["name"]    = device.name().toStdString();
            quint32 deviceClass = 0;
            deviceClass |= (device.minorDeviceClass() << 2);  // Set bits 2-7
            deviceClass |= (device.majorDeviceClass() << 8);  // Set bits 8-12
            deviceClass |= (device.serviceClasses() << 13);
            wiz_settings["devices"][device_idx]["class"]   = deviceClass;
        }
    }

    ResourceManager::get()->GetSettingsManager()->SetSettings("BleDomDevices", wiz_settings);
    ResourceManager::get()->GetSettingsManager()->SaveSettings();
    qDebug() << "BLEDOM Settings JSON" << QString::fromStdString(nlohmann::to_string(wiz_settings));
    QMessageBox::information(this, tr("Done"), tr("Press the \"%1\" button to initialize your new devices.").arg(tr("Rescan Devices")));
}


