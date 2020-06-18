#include "OpenRGBDialog2.h"
#include "OpenRGBDeviceInfoPage.h"
#include "OpenRGBServerInfoPage.h"
#include "OpenRGBSoftwareInfoPage.h"
#include "OpenRGBSystemInfoPage.h"
#include "OpenRGBProfileSaveDialog.h"
#include <QLabel>
#include <QTabBar>
#include <QMessageBox>
#include <QListWidget>

using namespace Ui;

static QString GetIconString(device_type type)
{
    switch(type)
    {
    case DEVICE_TYPE_MOTHERBOARD:
        return("motherboard.png");
        break;
    case DEVICE_TYPE_DRAM:
        return("dram.png");
        break;
    case DEVICE_TYPE_GPU:
        return("gpu.png");
        break;
    case DEVICE_TYPE_COOLER:
        return("fan.png");
        break;
    case DEVICE_TYPE_LEDSTRIP:
        return("ledstrip.png");
        break;
    case DEVICE_TYPE_KEYBOARD:
        return("keyboard.png");
        break;
    case DEVICE_TYPE_MOUSE:
        return("mouse.png");
        break;
    case DEVICE_TYPE_MOUSEMAT:
        return("mousemat.png");
        break;
    case DEVICE_TYPE_HEADSET:
        return("headset.png");
        break;
    case DEVICE_TYPE_HEADSET_STAND:
        return("headsetstand.png");
        break;
    case DEVICE_TYPE_UNKNOWN:
        return("unknown.png");
        break;
    }
}

OpenRGBDialog2::OpenRGBDialog2(std::vector<i2c_smbus_interface *>& bus, std::vector<RGBController *>& control, ProfileManager* manager, NetworkServer* server, bool show_i2c_tools, QWidget *parent) : QMainWindow(parent), busses(bus), controllers(control), profile_manager(manager), network_server(server), ui(new OpenRGBDialog2Ui)
{
    ui->setupUi(this);

    QIcon logo(":OpenRGB.png");
    setWindowIcon(logo);

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

    RefreshProfileList();

    /*-----------------------------------------------------*\
    | Set up list of devices                                |
    \*-----------------------------------------------------*/
    QTabBar *DevicesTabBar = ui->DevicesTabBar->tabBar();

    for(std::size_t dev_idx = 0; dev_idx < control.size(); dev_idx++)
    {
        OpenRGBDevicePage *NewPage = new OpenRGBDevicePage(control[dev_idx]);
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
        NewLabelString += GetIconString(control[dev_idx]->type);
        NewLabelString += "' height='16' width='16'></td><td>" + QString::fromStdString(control[dev_idx]->name) + "</td></tr></table></html>";

        QLabel *NewTabLabel = new QLabel();
        NewTabLabel->setText(NewLabelString);
        NewTabLabel->setIndent(20);
        NewTabLabel->setGeometry(0, 0, 200, 20);

        DevicesTabBar->setTabButton(dev_idx, QTabBar::LeftSide, NewTabLabel);
        DevicesTabBar->setObjectName(QString::fromStdString(control[dev_idx]->name));
        DevicesTabBar->setElideMode(Qt::ElideMiddle);
    }


    /*-----------------------------------------------------*\
    | Set up list of information                            |
    \*-----------------------------------------------------*/
    QTabBar *InformationTabBar = ui->InformationTabBar->tabBar();

    for(std::size_t dev_idx = 0; dev_idx < control.size(); dev_idx++)
    {
        OpenRGBDeviceInfoPage *NewPage = new OpenRGBDeviceInfoPage(control[dev_idx]);
        ui->InformationTabBar->addTab(NewPage, "");

        /*-----------------------------------------------------*\
        | Use Qt's HTML capabilities to display both icon and   |
        | text in the tab label.  Choose icon based on device   |
        | type and append device name string.                   |
        \*-----------------------------------------------------*/
        QString NewLabelString = "<html><table><tr><td width='30'><img src=':/";
        NewLabelString += GetIconString(control[dev_idx]->type);
        NewLabelString += "' height='16' width='16'></td><td>" + QString::fromStdString(control[dev_idx]->name) + "</td></tr></table></html>";

        QLabel *NewTabLabel = new QLabel();
        NewTabLabel->setText(NewLabelString);
        NewTabLabel->setIndent(20);
        NewTabLabel->setGeometry(0, 0, 200, 20);

        InformationTabBar->setTabButton(dev_idx, QTabBar::LeftSide, NewTabLabel);
    }

    /*-----------------------------------------------------*\
    | Show the I2C Tools page only if enabled               |
    \*-----------------------------------------------------*/
    if(show_i2c_tools)
    {
        OpenRGBSystemInfoPage *SMBusToolsPage = new OpenRGBSystemInfoPage(bus);
        ui->InformationTabBar->addTab(SMBusToolsPage, "");

        QString SMBusToolsLabelString = "<html><table><tr><td width='30'><img src='";
        SMBusToolsLabelString += ":/tools.png";
        SMBusToolsLabelString += "' height='16' width='16'></td><td>SMBus Tools</td></tr></table></html>";

        QLabel *SMBusToolsTabLabel = new QLabel();
        SMBusToolsTabLabel->setText(SMBusToolsLabelString);
        SMBusToolsTabLabel->setIndent(20);
        SMBusToolsTabLabel->setGeometry(0, 0, 200, 20);

        InformationTabBar->setTabButton(control.size(), QTabBar::LeftSide, SMBusToolsTabLabel);
    }

    /*-----------------------------------------------------*\
    | Always show the software information page             |
    \*-----------------------------------------------------*/
    OpenRGBSoftwareInfoPage *SoftInfoPage = new OpenRGBSoftwareInfoPage();
    ui->InformationTabBar->addTab(SoftInfoPage, "");

    QString SoftwareLabelString = "<html><table><tr><td width='30'><img src='";
    SoftwareLabelString += ":/software.png";
    SoftwareLabelString += "' height='16' width='16'></td><td>Software</td></tr></table></html>";

    QLabel *SoftwareTabLabel = new QLabel();
    SoftwareTabLabel->setText(SoftwareLabelString);
    SoftwareTabLabel->setIndent(20);
    SoftwareTabLabel->setGeometry(0, 0, 200, 20);

    if(show_i2c_tools)
    {
        InformationTabBar->setTabButton(control.size() + 1, QTabBar::LeftSide, SoftwareTabLabel);
    }
    else
    {
        InformationTabBar->setTabButton(control.size(), QTabBar::LeftSide, SoftwareTabLabel);
    }

    /*-----------------------------------------------------*\
    | Add server information tab if there is a server       |
    \*-----------------------------------------------------*/
    if(network_server != NULL)
    {
        OpenRGBServerInfoPage *ServerInfoPage = new OpenRGBServerInfoPage(network_server);

        ui->MainTabBar->addTab(ServerInfoPage, "SDK Server");
    }
}

OpenRGBDialog2::~OpenRGBDialog2()
{
    delete ui;
}

void OpenRGBDialog2::show()
{
    QMainWindow::show();
}

void OpenRGBDialog2::RefreshProfileList()
{
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

void OpenRGBDialog2::on_SetAllDevices(unsigned char red, unsigned char green, unsigned char blue)
{
    for(int device = 0; device < ui->DevicesTabBar->count(); device++)
    {
        qobject_cast<OpenRGBDevicePage *>(ui->DevicesTabBar->widget(device))->SetCustomMode();
        qobject_cast<OpenRGBDevicePage *>(ui->DevicesTabBar->widget(device))->SetDevice(red, green, blue);
    }
}

void OpenRGBDialog2::on_SaveSizeProfile()
{
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
            RefreshProfileList();
        }
    }
}

void Ui::OpenRGBDialog2::on_ButtonLoadProfile_clicked()
{
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

            RefreshProfileList();
        }
    }
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
        //Figure out how to ungroup
        //Compare the Page -> Device to controllers and reparent the associated page from hiddenpages
        QTabWidget* qtwTemp = page->findChild<QTabWidget *>("GROUPLIST");
        for( ; qtwTemp->count() > 0; )
        {
            //QTabWidget* qtw = hiddenTabs->findChild<QTabWidget *>(qliTemp->item(i)->data(Qt::ItemDataRole));
            //qtw->setParent(ui->DevicesTabBar);
            int j = qtwTemp->widget(0)->accessibleName().toInt();
            j = ui->DevicesTabBar->insertTab(j, qtwTemp->widget(0), qtwTemp->widget(0)->objectName());
            ui->DevicesTabBar->widget(j)->setEnabled(true);
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
            //Save the details of the page for possible ungrouping
            //QListWidgetItem* qliItem = new QListWidgetItem(QString("%1").arg(ui->DevicesTabBar->widget(i)->objectName()), qliTemp);
            //Reparent the page to remove it from the list
            int j = qtwTemp->addTab(ui->DevicesTabBar->widget(i),ui->DevicesTabBar->widget(i)->objectName());
            //ui->DevicesTabBar->widget(i)->setParent(hiddenTabs);

            qtwTemp->widget(j)->setAccessibleName(QString("%1").arg(i)); //Stores the device index from controllers for later
            //qliTemp->addItem(qliItem);
        }
    }
    index = ui->DevicesTabBar->insertTab(index, NewPage, "");

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
    NewLabelString += GetIconString(rgb_controller->type);
    NewLabelString += "' height='16' width='16'></td><td>" + QString::fromStdString(rgb_controller->name) + "</td></tr></table></html>";

    QLabel *NewTabLabel = new QLabel();
    NewTabLabel->setText(NewLabelString);
    NewTabLabel->setIndent(20);
    NewTabLabel->setGeometry(0, 0, 200, 20);

    ui->DevicesTabBar->tabBar()->setTabButton(index, QTabBar::LeftSide, NewTabLabel);
}
