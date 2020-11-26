#include "OpenRGBDialog2.h"
#include "OpenRGBDevicePage.h"
#include "OpenRGBDeviceInfoPage.h"
#include "OpenRGBServerInfoPage.h"
#include "OpenRGBProfileSaveDialog.h"
#include "ResourceManager.h"
#include <QLabel>
#include <QTabBar>
#include <QMessageBox>
#include <QCloseEvent>
#include <QListWidget>

#ifdef _WIN32
#include <QSettings>
#endif

using namespace Ui;

static QString GetIconString(device_type type, bool dark)
{
    /*-----------------------------------------------------*\
    | Return the icon filename string for the given device  |
    | type value                                            |
    \*-----------------------------------------------------*/
    QString filename;
    switch(type)
    {
    case DEVICE_TYPE_MOTHERBOARD:
        filename = "motherboard";
        break;
    case DEVICE_TYPE_DRAM:
        filename = "dram";
        break;
    case DEVICE_TYPE_GPU:
        filename = "gpu";
        break;
    case DEVICE_TYPE_COOLER:
        filename = "fan";
        break;
    case DEVICE_TYPE_LEDSTRIP:
        filename = "ledstrip";
        break;
    case DEVICE_TYPE_KEYBOARD:
        filename = "keyboard";
        break;
    case DEVICE_TYPE_MOUSE:
        filename = "mouse";
        break;
    case DEVICE_TYPE_MOUSEMAT:
        filename = "mousemat";
        break;
    case DEVICE_TYPE_HEADSET:
        filename = "headset";
        break;
    case DEVICE_TYPE_HEADSET_STAND:
        filename = "headsetstand";
        break;
    case DEVICE_TYPE_GAMEPAD:
        filename = "gamepad";
        break;
    case DEVICE_TYPE_LIGHT:
        filename = "light";
        break;
    case DEVICE_TYPE_GROUP:
        filename = "group";
        break;
    default:
        filename = "unknown";
        break;
    }
    if(dark)
    {
        filename += "_dark";
    }
    filename += ".png";
    return filename;
}

static void UpdateDeviceListCallback(void * this_ptr)
{
    OpenRGBDialog2 * this_obj = (OpenRGBDialog2 *)this_ptr;

    QMetaObject::invokeMethod(this_obj, "onDeviceListUpdated", Qt::QueuedConnection);
}

static void UpdateDetectionProgressCallback(void * this_ptr)
{
    OpenRGBDialog2 * this_obj = (OpenRGBDialog2 *)this_ptr;

    QMetaObject::invokeMethod(this_obj, "onDetectionProgressUpdated", Qt::QueuedConnection);
}

OpenRGBDialog2::OpenRGBDialog2(std::vector<i2c_smbus_interface *>& bus, std::vector<RGBController *>& control, QWidget *parent) : QMainWindow(parent), busses(bus), controllers(control), ui(new OpenRGBDialog2Ui)
{
    ui->setupUi(this);

    /*-----------------------------------------------------*\
    | Set window icon                                       |
    \*-----------------------------------------------------*/
    QIcon logo(":OpenRGB.png");
    setWindowIcon(logo);

    /*-----------------------------------------------------*\
    | Register detection progress callback with resource    |
    | manager                                               |
    \*-----------------------------------------------------*/
    ResourceManager::get()->RegisterDetectionProgressCallback(UpdateDetectionProgressCallback, this);
    ResourceManager::get()->RegisterDeviceListChangeCallback(UpdateDeviceListCallback, this);

    /*-----------------------------------------------------*\
    | Initialize page pointers                              |
    \*-----------------------------------------------------*/
    ClientInfoPage  = NULL;
    SMBusToolsPage  = NULL;
    SoftInfoPage    = NULL;

    onDetectionProgressUpdated();

    ui->DetectionProgressBar->setRange(0, 100);
    ui->DetectionProgressBar->setValue(0);
    ui->DetectionProgressBar->setTextVisible(true);
    ui->DetectionProgressBar->setFormat("");
    ui->DetectionProgressBar->setAlignment(Qt::AlignCenter);

    /*-----------------------------------------------------*\
    | Set up tray icon menu                                 |
    \*-----------------------------------------------------*/
    QMenu* trayIconMenu = new QMenu( this );

    trayIcon = new QSystemTrayIcon(this);

    QAction* actionShowHide = new QAction("Show/Hide", this);
    connect(actionShowHide, SIGNAL(triggered()), this, SLOT(on_ShowHide()));
    trayIconMenu->addAction(actionShowHide);

    profileMenu = new QMenu("Profiles", this);

    trayIconMenu->addMenu(profileMenu);

    QMenu* quickColorsMenu = new QMenu("Quick Colors", this);

    QAction* actionQuickRed = new QAction("Red", this);
    connect(actionQuickRed, SIGNAL(triggered()), this, SLOT(on_QuickRed()));
    quickColorsMenu->addAction(actionQuickRed);

    QAction* actionQuickYellow = new QAction("Yellow", this);
    connect(actionQuickYellow, SIGNAL(triggered()), this, SLOT(on_QuickYellow()));
    quickColorsMenu->addAction(actionQuickYellow);

    QAction* actionQuickGreen = new QAction("Green", this);
    connect(actionQuickGreen, SIGNAL(triggered()), this, SLOT(on_QuickGreen()));
    quickColorsMenu->addAction(actionQuickGreen);

    QAction* actionQuickCyan = new QAction("Cyan", this);
    connect(actionQuickCyan, SIGNAL(triggered()), this, SLOT(on_QuickCyan()));
    quickColorsMenu->addAction(actionQuickCyan);

    QAction* actionQuickBlue = new QAction("Blue", this);
    connect(actionQuickBlue, SIGNAL(triggered()), this, SLOT(on_QuickBlue()));
    quickColorsMenu->addAction(actionQuickBlue);
    
    QAction* actionQuickMagenta = new QAction("Magenta", this);
    connect(actionQuickMagenta, SIGNAL(triggered()), this, SLOT(on_QuickMagenta()));
    quickColorsMenu->addAction(actionQuickMagenta);

    QAction* actionQuickWhite = new QAction("White", this);
    connect(actionQuickWhite, SIGNAL(triggered()), this, SLOT(on_QuickWhite()));
    quickColorsMenu->addAction(actionQuickWhite);

    trayIconMenu->addMenu(quickColorsMenu);

    QAction* actionLightsOff = new QAction("Lights Off", this);
    connect(actionLightsOff, SIGNAL(triggered()), this, SLOT(on_LightsOff()));
    trayIconMenu->addAction(actionLightsOff);

    QAction* actionExit = new QAction( "Exit", this );
    connect( actionExit, SIGNAL( triggered() ), this, SLOT( on_Exit() ));
    trayIconMenu->addAction(actionExit);

    trayIcon->setIcon(logo);
    trayIcon->setToolTip("OpenRGB");
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->show();
    darkTheme = palette().window().color().value() < 127; // Adjust

    //This is to support Grouping / Ungrouping tabs functionality
    hiddenTabs = new QTabWidget(this);
    hiddenTabs->hide();
    contextMenu = new QMenu(this);
    QAction* actionGroupRGBController = new QAction("Group controller", this);
    connect(actionGroupRGBController, SIGNAL(triggered()), this, SLOT(on_GroupController()));
    contextMenu->addAction(actionGroupRGBController);
    QAction* actionUngroupRGBController = new QAction("Ungroup controllers", this);
    connect(actionUngroupRGBController, SIGNAL(triggered()), this, SLOT(on_UngroupControllers()));
    contextMenu->addAction(actionUngroupRGBController);

#ifdef _WIN32
    /*-------------------------------------------------*\
    | Windows dark theme settings                       |
    \*-------------------------------------------------*/
    json            theme_settings;

    /*-------------------------------------------------*\
    | Get prefered theme from settings manager          |
    \*-------------------------------------------------*/
    theme_settings = ResourceManager::get()->GetSettingsManager()->GetSettings("Setting_Theme");

    /*-------------------------------------------------*\
    | Read the theme key and adjust accordingly         |
    \*-------------------------------------------------*/
    std::string current_theme = "light";

    if(theme_settings.contains("theme"))
    {
        current_theme = theme_settings["theme"];
    }

    if((current_theme == "auto") || (current_theme == "dark"))
    {
        darkTheme = true;

        if(current_theme == "auto")
        {
            QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", QSettings::NativeFormat);

            if(settings.value("AppsUseLightTheme") != 0)
            {
                darkTheme = false;
            }
        }

        /*-------------------------------------------------*\
        | Apply dark theme on Windows if configured         |
        \*-------------------------------------------------*/
        if(darkTheme)
        {
            darkTheme = 1;
            QPalette pal = palette();
            pal.setColor(QPalette::WindowText, Qt::white);
            QApplication::setPalette(pal);
            QFile darkTheme(":/windows_dark.qss");
            darkTheme.open(QFile::ReadOnly);
            setStyleSheet(darkTheme.readAll());
        }

        QLabel *HiddenTabLabel = new QLabel(NewLabelString, NewPage);
        //NewTabLabel->setText(NewLabelString);
        HiddenTabLabel->setHidden(true);
        HiddenTabLabel->setObjectName("TABLABEL");     //Required for the .findChild later
        QLabel *VisibleTabLabel = new QLabel(HiddenTabLabel->text());
        VisibleTabLabel->setIndent(20);
        VisibleTabLabel->setGeometry(0, 0, 200, 20);

        //DevicesTabBar->setTabButton(dev_idx, QTabBar::LeftSide, NewTabLabel);
        ui->DevicesTabBar->tabBar()->setTabButton(dev_idx, QTabBar::LeftSide,VisibleTabLabel); //This creates a copy of the Label otherwise it is re-parented
        //DevicesTabBar->setAccessibleTabName(dev_idx, QString::fromStdString(control[dev_idx]->name));
        ui->DevicesTabBar->widget(dev_idx)->setObjectName(QString::fromStdString(control[dev_idx]->name));
        //DevicesTabBar->setElideMode(Qt::ElideMiddle);
    }
    else if (current_theme == "light")
    {
        /*-----------------------------------------------------------------*\
        | It defaults to light theme so just an empty statement should work |
        \*-----------------------------------------------------------------*/
    }
#endif

    /*-----------------------------------------------------*\
    | Update the profile list                               |
    \*-----------------------------------------------------*/
    UpdateProfileList();

    /*-----------------------------------------------------*\
    | Update the device list and make sure the              |
    | ProgressBar gets a proper value                       |
    \*-----------------------------------------------------*/
    UpdateDevicesList();

    /*-----------------------------------------------------*\
    | Add Server Tab                                        |
    \*-----------------------------------------------------*/
    AddServerTab();

    /*-----------------------------------------------------*\
    | Add Client Tab                                        |
    \*-----------------------------------------------------*/
    AddClientTab();

    /*-----------------------------------------------------*\
    | Add the Software Info page                            |
    \*-----------------------------------------------------*/
    AddSoftwareInfoPage();

    /*-----------------------------------------------------*\
    | Add the SMBus Tools page if enabled                   |
    \*-----------------------------------------------------*/
    if(ShowI2CTools)
    {
        AddI2CToolsPage();
    }
}

OpenRGBDialog2::~OpenRGBDialog2()
{
    delete ui;
}

void OpenRGBDialog2::closeEvent(QCloseEvent *event)
{
    ResourceManager::get()->WaitForDeviceDetection();
    event->accept();
}

void OpenRGBDialog2::AddSoftwareInfoPage()
{
    /*-----------------------------------------------------*\
    | Create the Software Information page                  |
    \*-----------------------------------------------------*/
    SoftInfoPage = new OpenRGBSoftwareInfoPage();

    ui->InformationTabBar->addTab(SoftInfoPage, "");

    QString SoftwareLabelString = "<html><table><tr><td width='30'><img src='";
    SoftwareLabelString += ":/software";
    if(darkTheme) SoftwareLabelString += "_dark";
    SoftwareLabelString += ".png' height='16' width='16'></td><td>Software</td></tr></table></html>";

    QLabel *SoftwareTabLabel = new QLabel();
    SoftwareTabLabel->setText(SoftwareLabelString);
    SoftwareTabLabel->setIndent(20);
    SoftwareTabLabel->setGeometry(0, 0, 200, 20);

    ui->InformationTabBar->tabBar()->setTabButton(ui->InformationTabBar->tabBar()->count() - 1, QTabBar::LeftSide, SoftwareTabLabel);
}

void OpenRGBDialog2::AddI2CToolsPage()
{
    ShowI2CTools = true;

    /*-----------------------------------------------------*\
    | Create the I2C Tools page if it doesn't exist yet     |
    \*-----------------------------------------------------*/
    SMBusToolsPage = new OpenRGBSystemInfoPage(busses);

    /*-----------------------------------------------------*\
    | Create the I2C Tools tab in the Information bar       |
    \*-----------------------------------------------------*/
    ui->InformationTabBar->addTab(SMBusToolsPage, "");

    QString SMBusToolsLabelString = "<html><table><tr><td width='30'><img src='";
    SMBusToolsLabelString += ":/tools";
    if(darkTheme) SMBusToolsLabelString += "_dark";
    SMBusToolsLabelString += ".png' height='16' width='16'></td><td>SMBus Tools</td></tr></table></html>";

    QLabel *SMBusToolsTabLabel = new QLabel();
    SMBusToolsTabLabel->setText(SMBusToolsLabelString);
    SMBusToolsTabLabel->setIndent(20);
    SMBusToolsTabLabel->setGeometry(0, 0, 200, 20);

    ui->InformationTabBar->tabBar()->setTabButton(ui->InformationTabBar->tabBar()->count() - 1, QTabBar::LeftSide, SMBusToolsTabLabel);
}

void OpenRGBDialog2::AddClientTab()
{
    /*-----------------------------------------------------*\
    | Add client information tab if it doesn't exist yet    |
    \*-----------------------------------------------------*/
    if(ClientInfoPage == NULL)
    {
        ClientInfoPage = new OpenRGBClientInfoPage(controllers);
        ui->MainTabBar->addTab(ClientInfoPage, "SDK Client");

        /*-----------------------------------------------------*\
        | Connect the page's Set All button to the Set All slot |
        \*-----------------------------------------------------*/
        connect(ClientInfoPage,
                SIGNAL(ClientListUpdated()),
                this,
                SLOT(on_ClientListUpdated()));
    }
}

void OpenRGBDialog2::AddClient(NetworkClient* new_client)
{
    /*-----------------------------------------------------*\
    | Add a client to the client information page           |
    \*-----------------------------------------------------*/
    if(ClientInfoPage != NULL)
    {
        ClientInfoPage->AddClient(new_client);
    }
}

void OpenRGBDialog2::AddServerTab()
{
    /*-----------------------------------------------------*\
    | Add server information tab if there is a server       |
    \*-----------------------------------------------------*/
    OpenRGBServerInfoPage *ServerInfoPage = new OpenRGBServerInfoPage(ResourceManager::get()->GetServer());
    ui->MainTabBar->addTab(ServerInfoPage, "SDK Server");
}

void OpenRGBDialog2::ClearDevicesList()
{
    for(int tab_idx = 0; tab_idx < ui->InformationTabBar->count(); tab_idx++)
    {
        delete ui->DevicesTabBar->widget(tab_idx);
    }
    ui->DevicesTabBar->clear();

    for(int tab_idx = 0; tab_idx < ui->InformationTabBar->count(); tab_idx++)
    {
        delete ui->InformationTabBar->widget(tab_idx);
    }
    ui->InformationTabBar->clear();
}

void OpenRGBDialog2::UpdateDevicesList()
{
    /*-----------------------------------------------------*\
    | Loop through each controller in the list.             |
    \*-----------------------------------------------------*/
    for(unsigned int controller_idx = 0; controller_idx < controllers.size(); controller_idx++)
    {
        /*-----------------------------------------------------*\
        | Loop through each tab in the devices tab bar          |
        \*-----------------------------------------------------*/
        bool found = false;

        for(unsigned int tab_idx = 0; tab_idx < ui->DevicesTabBar->count(); tab_idx++)
        {
            OpenRGBDevicePage* page = (OpenRGBDevicePage*) ui->DevicesTabBar->widget(tab_idx);

            /*-----------------------------------------------------*\
            | If the current tab matches the current controller,    |
            | move the tab to the correct position                  |
            \*-----------------------------------------------------*/
            if(controllers[controller_idx] == page->GetController())
            {
                found = true;
                ui->DevicesTabBar->tabBar()->moveTab(tab_idx, controller_idx);
                break;
            }
        }

        if(!found)
        {
            /*-----------------------------------------------------*\
            | The controller does not have a tab already created    |
            | Create a new tab and move it to the correct position  |
            \*-----------------------------------------------------*/
            OpenRGBDevicePage *NewPage = new OpenRGBDevicePage(controllers[controller_idx]);
            ui->DevicesTabBar->addTab(NewPage, "");

            /*-----------------------------------------------------*\
            | Connect the page's Set All button to the Set All slot |
            \*-----------------------------------------------------*/
            connect(NewPage,
                    SIGNAL(SetAllDevices(unsigned char, unsigned char, unsigned char)),
                    this,
                    SLOT(on_SetAllDevices(unsigned char, unsigned char, unsigned char)));

            /*-----------------------------------------------------*\
            | Connect the page's Resize signal to the Save Size slot|
            \*-----------------------------------------------------*/
            connect(NewPage,
                    SIGNAL(SaveSizeProfile()),
                    this,
                    SLOT(on_SaveSizeProfile()));

            /*-----------------------------------------------------*\
            | Use Qt's HTML capabilities to display both icon and   |
            | text in the tab label.  Choose icon based on device   |
            | type and append device name string.                   |
            \*-----------------------------------------------------*/
            QString NewLabelString = "<html><table><tr><td width='30'><img src=':/";
            NewLabelString += GetIconString(controllers[controller_idx]->type, darkTheme);
            NewLabelString += "' height='16' width='16'></td><td>" + QString::fromStdString(controllers[controller_idx]->name) + "</td></tr></table></html>";

            QLabel *NewTabLabel = new QLabel();
            NewTabLabel->setText(NewLabelString);
            NewTabLabel->setIndent(20);
            NewTabLabel->setGeometry(0, 0, 200, 20);

            ui->DevicesTabBar->tabBar()->setTabButton(ui->DevicesTabBar->count() - 1, QTabBar::LeftSide, NewTabLabel);

            /*-----------------------------------------------------*\
            | Now move the new tab to the correct position          |
            \*-----------------------------------------------------*/
            ui->DevicesTabBar->tabBar()->moveTab(ui->DevicesTabBar->count() - 1, controller_idx);
        }

        /*-----------------------------------------------------*\
        | Loop through each tab in the information tab bar      |
        \*-----------------------------------------------------*/
        found = false;

        for(unsigned int tab_idx = 0; tab_idx < ui->InformationTabBar->count(); tab_idx++)
        {
            /*-----------------------------------------------------*\
            | If type is a device info page, check it               |
            \*-----------------------------------------------------*/
            std::string type_str = ui->InformationTabBar->widget(tab_idx)->metaObject()->className();
            if(type_str == "Ui::OpenRGBDeviceInfoPage")
            {
                OpenRGBDeviceInfoPage* page = (OpenRGBDeviceInfoPage*) ui->InformationTabBar->widget(tab_idx);

                /*-----------------------------------------------------*\
                | If the current tab matches the current controller,    |
                | move the tab to the correct position                  |
                \*-----------------------------------------------------*/
                if(controllers[controller_idx] == page->GetController())
                {
                    found = true;
                    ui->InformationTabBar->tabBar()->moveTab(tab_idx, controller_idx);
                    break;
                }
            }
        }

        if(!found)
        {
            /*-----------------------------------------------------*\
            | The controller does not have a tab already created    |
            | Create a new tab and move it to the correct position  |
            \*-----------------------------------------------------*/
            OpenRGBDeviceInfoPage *NewPage = new OpenRGBDeviceInfoPage(controllers[controller_idx]);
            ui->InformationTabBar->addTab(NewPage, "");

            /*-----------------------------------------------------*\
            | Use Qt's HTML capabilities to display both icon and   |
            | text in the tab label.  Choose icon based on device   |
            | type and append device name string.                   |
            \*-----------------------------------------------------*/
            QString NewLabelString = "<html><table><tr><td width='30'><img src=':/";
            NewLabelString += GetIconString(controllers[controller_idx]->type, darkTheme);
            NewLabelString += "' height='16' width='16'></td><td>" + QString::fromStdString(controllers[controller_idx]->name) + "</td></tr></table></html>";

            QLabel *NewTabLabel = new QLabel();
            NewTabLabel->setText(NewLabelString);
            NewTabLabel->setIndent(20);
            NewTabLabel->setGeometry(0, 0, 200, 20);

            ui->InformationTabBar->tabBar()->setTabButton(ui->InformationTabBar->count() - 1, QTabBar::LeftSide, NewTabLabel);

            /*-----------------------------------------------------*\
            | Now move the new tab to the correct position          |
            \*-----------------------------------------------------*/
            ui->InformationTabBar->tabBar()->moveTab(ui->InformationTabBar->count() - 1, controller_idx);
        }
    }

    /*-----------------------------------------------------*\
    | Remove all remaining device tabs                      |
    \*-----------------------------------------------------*/
    unsigned int tab_count = ui->DevicesTabBar->count();
    for(unsigned int tab_idx = controllers.size(); tab_idx < tab_count; tab_idx++)
    {
        ui->DevicesTabBar->removeTab(ui->DevicesTabBar->count() - 1);
    }

    bool found = true;
    while(found)
    {
        found = false;

        /*-----------------------------------------------------*\
        | Remove all remaining device information tabs, leaving |
        | other information tabs alone                          |
        \*-----------------------------------------------------*/
        for(unsigned int tab_idx = controllers.size(); tab_idx < ui->InformationTabBar->count(); tab_idx++)
        {
            std::string type_str = ui->InformationTabBar->widget(tab_idx)->metaObject()->className();
            if(type_str == "Ui::OpenRGBDeviceInfoPage")
            {
                found = true;
                ui->InformationTabBar->removeTab(tab_idx);
                break;
            }
        }
    }
}

void OpenRGBDialog2::UpdateProfileList()
{
    ProfileManager* profile_manager = ResourceManager::get()->GetProfileManager();

    if(profile_manager != NULL)
    {
        /*-----------------------------------------------------*\
        | Clear profile combo box and tray icon menu            |
        \*-----------------------------------------------------*/
        ui->ProfileBox->clear();
        profileMenu->clear();

        for(std::size_t profile_index = 0; profile_index < profile_manager->profile_list.size(); profile_index++)
        {
            /*-----------------------------------------------------*\
            | Fill in profile combo box                             |
            \*-----------------------------------------------------*/
            ui->ProfileBox->addItem(profile_manager->profile_list[profile_index].c_str());

            /*-----------------------------------------------------*\
            | Fill in profile tray icon menu                        |
            \*-----------------------------------------------------*/
            QAction* actionProfileSelected = new QAction(profile_manager->profile_list[profile_index].c_str(), this);
            actionProfileSelected->setObjectName(profile_manager->profile_list[profile_index].c_str());
            connect(actionProfileSelected, SIGNAL(triggered()), this, SLOT(on_ProfileSelected()));
            profileMenu->addAction(actionProfileSelected);
        }
    }
}

void OpenRGBDialog2::on_Exit()
{
    trayIcon->hide();
    close();
}

void OpenRGBDialog2::on_LightsOff()
{
    on_SetAllDevices(0x00, 0x00, 0x00);
}

void OpenRGBDialog2::on_QuickRed()
{
    on_SetAllDevices(0xFF, 0x00, 0x00);
}

void OpenRGBDialog2::on_QuickYellow()
{
    on_SetAllDevices(0xFF, 0xFF, 0x00);
}

void OpenRGBDialog2::on_QuickGreen()
{
    on_SetAllDevices(0x00, 0xFF, 0x00);
}

void OpenRGBDialog2::on_QuickCyan()
{
    on_SetAllDevices(0x00, 0xFF, 0xFF);
}

void OpenRGBDialog2::on_QuickBlue()
{
    on_SetAllDevices(0x00, 0x00, 0xFF);
}

void OpenRGBDialog2::on_QuickMagenta()
{
    on_SetAllDevices(0xFF, 0x00, 0xFF);
}

void OpenRGBDialog2::on_QuickWhite()
{
    on_SetAllDevices(0xFF, 0xFF, 0xFF);
}

void OpenRGBDialog2::on_ClientListUpdated()
{
    ClearDevicesList();
    UpdateDevicesList();
}

void OpenRGBDialog2::onDeviceListUpdated()
{
    UpdateDevicesList();
}

void OpenRGBDialog2::onDetectionProgressUpdated()
{
    ui->DetectionProgressBar->setValue(ResourceManager::get()->GetDetectionPercent());
    ui->DetectionProgressBar->setFormat(QString::fromStdString(ResourceManager::get()->GetDetectionString()));

    if(ResourceManager::get()->GetDetectionPercent() == 100)
    {
        SetDetectionViewState(false);
    }
    else
    {
        SetDetectionViewState(true);
    }
}

void OpenRGBDialog2::on_SetAllDevices(unsigned char red, unsigned char green, unsigned char blue)
{
    for(int device = 0; device < ui->DevicesTabBar->count(); device++)
    {
        qobject_cast<OpenRGBDevicePage *>(ui->DevicesTabBar->widget(device))->SetCustomMode(red, green, blue);
    }
}

void OpenRGBDialog2::on_SaveSizeProfile()
{
    ProfileManager* profile_manager = ResourceManager::get()->GetProfileManager();

    if(profile_manager != NULL)
    {
        /*---------------------------------------------------------*\
        | Save the profile                                          |
        \*---------------------------------------------------------*/
        profile_manager->SaveProfile("sizes.ors");
    }
}

void OpenRGBDialog2::on_ShowHide()
{
    if(isHidden())
    {
        show();
    }
    else
    {
        hide();
    }
}

void Ui::OpenRGBDialog2::on_ProfileSelected()
{
    ProfileManager* profile_manager = ResourceManager::get()->GetProfileManager();

    if(profile_manager != NULL)
    {
        /*---------------------------------------------------------*\
        | Get the profile filename from the selected object         |
        \*---------------------------------------------------------*/
        std::string profile_name = QObject::sender()->objectName().toStdString();

        /*---------------------------------------------------------*\
        | Load the profile                                          |
        \*---------------------------------------------------------*/
        if(profile_manager->LoadProfile(profile_name))
        {
            for(int device = 0; device < ui->DevicesTabBar->count(); device++)
            {
                qobject_cast<OpenRGBDevicePage *>(ui->DevicesTabBar->widget(device))->UpdateDevice();
            }
        }
    }
}

void Ui::OpenRGBDialog2::on_ButtonSaveProfile_clicked()
{
    OpenRGBProfileSaveDialog dialog;
    ProfileManager* profile_manager = ResourceManager::get()->GetProfileManager();

    if(profile_manager != NULL)
    {
        /*---------------------------------------------------------*\
        | Open Profile Name Dialog                                  |
        \*---------------------------------------------------------*/
        std::string profile_name = dialog.show();

        /*---------------------------------------------------------*\
        | Extension .orp - OpenRgb Profile                          |
        \*---------------------------------------------------------*/
        std::string filename = profile_name + ".orp";

        /*---------------------------------------------------------*\
        | Save the profile                                          |
        \*---------------------------------------------------------*/
        if(profile_manager->SaveProfile(filename))
        {
            UpdateProfileList();
        }
    }
}

void Ui::OpenRGBDialog2::on_ButtonLoadProfile_clicked()
{
    ProfileManager* profile_manager = ResourceManager::get()->GetProfileManager();

    if(profile_manager != NULL)
    {
        /*---------------------------------------------------------*\
        | Get the profile filename from the profiles list           |
        \*---------------------------------------------------------*/
        std::string profile_name = ui->ProfileBox->currentText().toStdString();

        /*---------------------------------------------------------*\
        | Load the profile                                          |
        \*---------------------------------------------------------*/
        if(profile_manager->LoadProfile(profile_name))
        {
            for(int device = 0; device < ui->DevicesTabBar->count(); device++)
            {
                qobject_cast<OpenRGBDevicePage *>(ui->DevicesTabBar->widget(device))->UpdateDevice();
            }
        }
    }
}

void Ui::OpenRGBDialog2::on_ButtonDeleteProfile_clicked()
{
    ProfileManager* profile_manager = ResourceManager::get()->GetProfileManager();

    if(profile_manager != NULL)
    {
        /*---------------------------------------------------------*\
        | Get the profile filename from the profiles list           |
        \*---------------------------------------------------------*/
        std::string profile_name = ui->ProfileBox->currentText().toStdString();

        /*---------------------------------------------------------*\
        | Confirm we want to delete the profile                     |
        \*---------------------------------------------------------*/
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Delete Profile", "Do you really want to delete this profile?", QMessageBox::Yes|QMessageBox::No);

        /*---------------------------------------------------------*\
        | Load the profile                                          |
        \*---------------------------------------------------------*/
        if(reply == QMessageBox::Yes)
        {
            profile_manager->DeleteProfile(profile_name);

            UpdateProfileList();
        }
    }
}

void Ui::OpenRGBDialog2::on_ButtonToggleDeviceView_clicked()
{
    if(device_view_showing)
    {
        for(int device = 0; device < ui->DevicesTabBar->count(); device++)
        {
            qobject_cast<OpenRGBDevicePage *>(ui->DevicesTabBar->widget(device))->HideDeviceView();
        }
        device_view_showing = false;
    }
    else
    {
        for(int device = 0; device < ui->DevicesTabBar->count(); device++)
        {
            qobject_cast<OpenRGBDevicePage *>(ui->DevicesTabBar->widget(device))->ShowDeviceView();
        }
        device_view_showing = true;
    }
}

void Ui::OpenRGBDialog2::on_ButtonStopDetection_clicked()
{
    /*---------------------------------------------------------*\
    | Notify the detection thread that it has to die            |
    \*---------------------------------------------------------*/
    ResourceManager::get()->StopDeviceDetection();

    /*---------------------------------------------------------*\
    | Pretend we're done already by hiding the progress bar     |
    \*---------------------------------------------------------*/
    SetDetectionViewState(false);
}

void Ui::OpenRGBDialog2::SetDetectionViewState(bool detection_showing)
{
    if(detection_showing)
    {
        /*---------------------------------------------------------*\
        | Show the detection progress and hide the normal buttons   |
        \*---------------------------------------------------------*/
        ui->ButtonToggleDeviceView->setVisible(false);
        ui->ButtonRescan->setVisible(false);
        ui->ButtonLoadProfile->setVisible(false);
        ui->ButtonSaveProfile->setVisible(false);
        ui->ButtonDeleteProfile->setVisible(false);
        ui->ProfileBox->setVisible(false);

        ui->DetectionProgressBar->setVisible(true);
        ui->DetectionProgressLabel->setVisible(true);
        ui->ButtonStopDetection->setVisible(true);
    }
    else
    {
        /*---------------------------------------------------------*\
        | Hide the detection progress and show the normal buttons   |
        \*---------------------------------------------------------*/
        ui->DetectionProgressBar->setVisible(false);
        ui->DetectionProgressLabel->setVisible(false);
        ui->ButtonStopDetection->setVisible(false);

        ui->ButtonToggleDeviceView->setVisible(true);
        ui->ButtonRescan->setVisible(true);
        ui->ButtonLoadProfile->setVisible(true);
        ui->ButtonSaveProfile->setVisible(true);
        ui->ButtonDeleteProfile->setVisible(true);
        ui->ProfileBox->setVisible(true);
    }
}
void Ui::OpenRGBDialog2::on_ButtonRescan_clicked()
{
    SetDetectionViewState(true);

    ResourceManager::get()->DetectDevices();
}

void Ui::OpenRGBDialog2::contextMenuEvent(QContextMenuEvent *event)
{
     QPoint _offset;
     _offset.setX(-10);
     _offset.setY(10);
     this->contextMenu->popup(event->pos() + this->pos() + _offset);
}

void Ui::OpenRGBDialog2::on_GroupController()
{
     QDialog* dlgGroup = new QDialog;
     QVBoxLayout* lyoVertical = new QVBoxLayout; //Layout Veritcally

     qleGroupName = new QLineEdit("Controller Group", dlgGroup);
     qliController = new QListWidget(dlgGroup);
     qliController->setSelectionMode(QAbstractItemView::MultiSelection);
     for(std::size_t dev_idx = 0; dev_idx < controllers.size(); dev_idx++)
     {
         QListWidgetItem* qliItem = new QListWidgetItem(QString("%1").arg(controllers.at(dev_idx)->name.c_str()), qliController);
         qliItem->setData(Qt::StatusTipRole, QString("%1").arg(dev_idx)); //Stores the device index from controllers for later
         qliItem->setData(Qt::ToolTipRole, QString("%1").arg(dev_idx)); //Stores the TAB index from DevicesTabBar for later
         qliController->addItem(qliItem);
     }
     QPushButton* qbtOK;
     qbtOK = new QPushButton("Group Selected", dlgGroup);
     connect(qbtOK, SIGNAL(clicked(bool)), this, SLOT(on_GroupSelected()));
     connect(qbtOK, SIGNAL(clicked(bool)), dlgGroup, SLOT(close()));
     QPushButton* qbtCancel;
     qbtCancel = new QPushButton("Cancel", dlgGroup);
     connect(qbtCancel, SIGNAL(clicked(bool)), dlgGroup, SLOT(close()));

     lyoVertical->addWidget(qliController);
     lyoVertical->addWidget(qleGroupName);
     lyoVertical->addWidget(qbtOK);
     lyoVertical->addWidget(qbtCancel);
     dlgGroup->setLayout(lyoVertical); //Done last or it's not inhiereted
     dlgGroup->show();
}

void Ui::OpenRGBDialog2::on_UngroupControllers()
{
    //If the tab that was clicked on was a Group then ungroup
    int index = ui->DevicesTabBar->currentIndex();
    QWidget* page = ui->DevicesTabBar->widget(index);
    if ( page->accessibleName() == "GROUP")
    {
        QTabWidget* qtwTemp = page->findChild<QTabWidget *>("GROUPLIST");
        for( int count = qtwTemp->count() ; count > 0; count--)
        {
            int dev_idx = qtwTemp->widget(0)->accessibleName().toInt(); //accesibleName was the original index preserved prior to grouping
            QLabel* TabLabel = qtwTemp->widget(0)->findChild<QLabel *>("TABLABEL");    //Find the TABLABEL from when this page was set up
            TabLabel->setIndent(20);
            TabLabel->setGeometry(0, 0, 200, 20);
            dev_idx = ui->DevicesTabBar->insertTab(dev_idx - count + 1, qtwTemp->widget(0), "");
            ui->DevicesTabBar->tabBar()->setTabButton(dev_idx, QTabBar::LeftSide, TabLabel);
            ui->DevicesTabBar->widget(dev_idx)->setEnabled(true);
        }
        page->deleteLater();    //Delete later will remove the object after the current scope
    }
}

void Ui::OpenRGBDialog2::on_GroupSelected()
{
    int index;
    std::vector<RGBController*> group;

    for (int i = 0; i < qliController->selectedItems().count(); i++)
    {
        index = qliController->selectedItems().at(i)->data(Qt::StatusTipRole).toInt();
        group.push_back(controllers.at(index));
        //Just set the wiget as disabled for now otherwise indexing is a problem
        ui->DevicesTabBar->widget(index)->setDisabled(true);
    }

    RGBGroupController* rgb_controller = new RGBGroupController( qleGroupName->text().toStdString(), group);
    controllers.push_back(rgb_controller);
    //Still need to add the dialog which requires pulling out the set up code
    OpenRGBDevicePage *NewPage = new OpenRGBDevicePage(rgb_controller);
    NewPage->setAccessibleName("GROUP");

    QTabWidget* qtwTemp = new QTabWidget(NewPage);
    qtwTemp->setHidden(true);
    qtwTemp->setObjectName("GROUPLIST");   //Required for the .findChild later

    index = ui->DevicesTabBar->currentIndex(); //Save this for the insert
    //You need to delete from the "back" as the count will change
    for (int i = ui->DevicesTabBar->count()-1; i > -1 ; i--)
    {
        if (!ui->DevicesTabBar->widget(i)->isEnabled())
        {
            //Add the tab to the created hidden Qtabwidget
            int j = qtwTemp->addTab(ui->DevicesTabBar->widget(i),ui->DevicesTabBar->widget(i)->accessibleName());
            qtwTemp->widget(j)->setAccessibleName(QString("%1").arg(i));
        }
    }

    /*-----------------------------------------------------*\
    | Connect the page's Set All button to the Set All slot |
    \*-----------------------------------------------------*/
    connect(NewPage,
            SIGNAL(SetAllDevices(unsigned char, unsigned char, unsigned char)),
            this,
            SLOT(on_SetAllDevices(unsigned char, unsigned char, unsigned char)));

    /*-----------------------------------------------------*\
    | Connect the page's Resize signal to the Save Size slot|
    \*-----------------------------------------------------*/
    connect(NewPage,
            SIGNAL(SaveSizeProfile()),
            this,
            SLOT(on_SaveSizeProfile()));

    /*-----------------------------------------------------*\
    | Use Qt's HTML capabilities to display both icon and   |
    | text in the tab label.  Choose icon based on device   |
    | type and append device name string.                   |
    \*-----------------------------------------------------*/
    QString NewLabelString = "<html><table><tr><td width='30'><img src=':/";
    NewLabelString += GetIconString(rgb_controller->type, darkTheme);
    NewLabelString += "' height='16' width='16'></td><td>" + QString::fromStdString(rgb_controller->name) + "</td></tr></table></html>";

    QLabel *NewTabLabel = new QLabel();
    NewTabLabel->setText(NewLabelString);
    NewTabLabel->setIndent(20);
    NewTabLabel->setGeometry(0, 0, 200, 20);

    ui->DevicesTabBar->tabBar()->setTabButton(index, QTabBar::LeftSide, NewTabLabel);
}
