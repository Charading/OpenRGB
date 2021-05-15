#include <QSignalMapper>
#include <QTimer>
#include "OpenRGBClientInfoPage.h"
#include "ResourceManager.h"

using namespace Ui;

static void UpdateInfoCallback(void * this_ptr)
{
    OpenRGBClientInfoPage * this_obj = (OpenRGBClientInfoPage *)this_ptr;

    QMetaObject::invokeMethod(this_obj, "UpdateInfo", Qt::QueuedConnection);
}

class NetworkClientPointer : public QObject
{
public:
    NetworkClient * net_client;
};

OpenRGBClientInfoPage::OpenRGBClientInfoPage(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::OpenRGBClientInfoPageUi)
{
    /*-----------------------------------------------------*\
    | Set initial values for GUI fields                     |
    \*-----------------------------------------------------*/
    ui->setupUi(this);
    ui->ClientIPValue->setText("127.0.0.1");
    ui->ClientPortValue->setText(QString::number(OPENRGB_SDK_PORT));

    /*-----------------------------------------------------*\
    | Register callbacks for existing clients               |
    \*-----------------------------------------------------*/
    for(unsigned int client_idx = 0; client_idx < ResourceManager::get()->GetClients().size(); client_idx++)
    {
        ResourceManager::get()->GetClients()[client_idx]->RegisterClientInfoChangeCallback(UpdateInfoCallback, this);
    }


    /*-----------------------------------------------------*\
    | Start thread to update fans                           |
    \*-----------------------------------------------------*/
    fan_thread_running = true;
    FanUpdateThread = new std::thread(&OpenRGBClientInfoPage::FanUpdateThreadFunction, this);

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, QOverload<>::of(&OpenRGBClientInfoPage::UpdateFanReadings));
    timer->start(1000);

    /*-----------------------------------------------------*\
    | Update the information view                           |
    \*-----------------------------------------------------*/
    UpdateInfo();
}

OpenRGBClientInfoPage::~OpenRGBClientInfoPage()
{
    fan_thread_running = false;
    FanUpdateThread->join();
    delete ui;
}

void OpenRGBClientInfoPage::FanUpdateThreadFunction()
{
    while(fan_thread_running)
    {
        /*-----------------------------------------------------*\
        | Loop through all clients in list and update them      |
        \*-----------------------------------------------------*/
        for(std::size_t client_idx = 0; client_idx < ResourceManager::get()->GetClients().size(); client_idx++)
        {
            /*-----------------------------------------------------*\
            | Add Fan Controllers for each device in the client     |
            \*-----------------------------------------------------*/
            if(ResourceManager::get()->GetClients()[client_idx]->server_fan_controllers.size() > 0)
            {
                for(std::size_t fan_controller = 0; fan_controller < ResourceManager::get()->GetClients()[client_idx]->server_fan_controllers.size(); fan_controller++)
                {
                    /*-----------------------------------------------------*\
                    | Create child tree widget items and display the device |
                    | names in them                                         |
                    \*-----------------------------------------------------*/
                    ResourceManager::get()->GetClients()[client_idx]->server_fan_controllers[fan_controller]->UpdateReading();
                }
            }
        }
        Sleep(1000);
    }
}

void OpenRGBClientInfoPage::UpdateFanReadings()
{
    /*-----------------------------------------------------*\
    | Loop through all clients in list and update them      |
    \*-----------------------------------------------------*/
    for(std::size_t client_idx = 0; client_idx < ResourceManager::get()->GetClients().size(); client_idx++)
    {
        // make sure the client widget has been added
        if(client_idx >= ui->ClientTree->topLevelItemCount()) break;

        if(ResourceManager::get()->GetClients()[client_idx]->server_fan_controllers.size() == 0) continue;

        // make sure the top item widgets have been added
        QTreeWidgetItem* top_item = ui->ClientTree->topLevelItem(client_idx);
        if(top_item->childCount() == 0) continue;

        QTreeWidgetItem* fan_controllers_item = top_item->child(top_item->childCount() - 1);

        /*-----------------------------------------------------*\
        | Loop through all fan controllers and update them      |
        \*-----------------------------------------------------*/
        for(std::size_t fan_controller = 0; fan_controller < ResourceManager::get()->GetClients()[client_idx]->server_fan_controllers.size(); fan_controller++)
        {
            // make sure the fan controller widget has been added
            if(fan_controller >= fan_controllers_item->childCount()) break;

            QTreeWidgetItem* fan_controller_sub_item = fan_controllers_item->child(fan_controller);

            /*-----------------------------------------------------*\
            | Loop through all fans and update them                 |
            \*-----------------------------------------------------*/
            for(std::size_t fan_idx = 0; fan_idx < ResourceManager::get()->GetClients()[client_idx]->server_fan_controllers[fan_controller]->fans.size(); fan_idx++)
            {
                // make sure the fan widget has been added
                if(fan_idx >= fan_controller_sub_item->childCount()) break;

                QTreeWidgetItem* fan_str_child = fan_controller_sub_item->child(fan_idx);

                std::string fan_str = ResourceManager::get()->GetClients()[client_idx]->server_fan_controllers[fan_controller]->fans[fan_idx].name + ": ";
                fan_str.append(std::to_string(ResourceManager::get()->GetClients()[client_idx]->server_fan_controllers[fan_controller]->fans[fan_idx].rpm_rdg) + " RPM / ");
                fan_str.append(std::to_string(ResourceManager::get()->GetClients()[client_idx]->server_fan_controllers[fan_controller]->fans[fan_idx].speed_cmd) + "% (");
                fan_str.append(std::to_string(ResourceManager::get()->GetClients()[client_idx]->server_fan_controllers[fan_controller]->fans[fan_idx].speed_min) + "% -> ");
                fan_str.append(std::to_string(ResourceManager::get()->GetClients()[client_idx]->server_fan_controllers[fan_controller]->fans[fan_idx].speed_max) + "%)");

                fan_str_child->setText(0, QString::fromStdString(fan_str));
            }
        }
    }
}

void OpenRGBClientInfoPage::AddClient(NetworkClient* new_client)
{
    /*-----------------------------------------------------*\
    | Add a new client to the list, register the callback,  |
    | and update the information view if the pointer is     |
    | valid                                                 |
    \*-----------------------------------------------------*/
    if(new_client != NULL)
    {
        ResourceManager::get()->GetClients().push_back(new_client);
        new_client->RegisterClientInfoChangeCallback(UpdateInfoCallback, this);

        UpdateInfo();
    }
}

void OpenRGBClientInfoPage::UpdateInfo()
{
    /*-----------------------------------------------------*\
    | Clear the tree view before recreating its contents    |
    \*-----------------------------------------------------*/
    ui->ClientTree->clear();

    /*-----------------------------------------------------*\
    | Set up the tree view header                           |
    \*-----------------------------------------------------*/
    ui->ClientTree->setColumnCount(3);
    ui->ClientTree->header()->setStretchLastSection(false);
    ui->ClientTree->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->ClientTree->setColumnWidth(1, 100);
    ui->ClientTree->setColumnWidth(2, 100);
    ui->ClientTree->setHeaderLabels(QStringList() << "Connected Clients" << "Protocol Version" << "");

    /*-----------------------------------------------------*\
    | Set up a signal mapper to handle disconnect buttons   |
    \*-----------------------------------------------------*/
    QSignalMapper* signalMapper = new QSignalMapper(this);
    connect(signalMapper, SIGNAL(mapped(QObject *)), this, SLOT(onClientDisconnectButton_clicked(QObject *)));

    /*-----------------------------------------------------*\
    | Loop through all clients in list and display them     |
    \*-----------------------------------------------------*/
    for(std::size_t client_idx = 0; client_idx < ResourceManager::get()->GetClients().size(); client_idx++)
    {
        /*-----------------------------------------------------*\
        | Create the top level tree widget items and display the|
        | client IP addresses and protocol versions in them     |
        \*-----------------------------------------------------*/
        QTreeWidgetItem* new_top_item = new QTreeWidgetItem(ui->ClientTree);
        new_top_item->setText(0, QString::fromStdString(ResourceManager::get()->GetClients()[client_idx]->GetIP()));
        new_top_item->setText(1, QString::number(ResourceManager::get()->GetClients()[client_idx]->GetProtocolVersion()));

        /*-----------------------------------------------------*\
        | Create the disconnect buttons and connect them to the |
        | signal mapper                                         |
        \*-----------------------------------------------------*/
        QPushButton* new_button = new QPushButton( "Disconnect" );
        ui->ClientTree->setItemWidget(new_top_item, 2, new_button);

        connect(new_button, SIGNAL(clicked()), signalMapper, SLOT(map()));

        NetworkClientPointer * new_arg = new NetworkClientPointer();
        new_arg->net_client = ResourceManager::get()->GetClients()[client_idx];

        signalMapper->setMapping(new_button, new_arg);

        /*-----------------------------------------------------*\
        | Add RGB Controllers for each device in the client     |
        \*-----------------------------------------------------*/
        if(ResourceManager::get()->GetClients()[client_idx]->server_rgb_controllers.size() > 0)
        {

            QTreeWidgetItem* new_item = new QTreeWidgetItem(new_top_item);
            new_item->setText(0, "RGB Controllers");

            for(std::size_t rgb_controller = 0; rgb_controller < ResourceManager::get()->GetClients()[client_idx]->server_rgb_controllers.size(); rgb_controller++)
            {
                /*-----------------------------------------------------*\
                | Create child tree widget items and display the device |
                | names in them                                         |
                \*-----------------------------------------------------*/
                QTreeWidgetItem* new_sub_item = new QTreeWidgetItem(new_item);
                new_sub_item->setText(0, QString::fromStdString(ResourceManager::get()->GetClients()[client_idx]->server_rgb_controllers[rgb_controller]->name));

                /*-----------------------------------------------------*\
                | Add child items for each zone in the device           |
                \*-----------------------------------------------------*/
                for(std::size_t zone_idx = 0; zone_idx < ResourceManager::get()->GetClients()[client_idx]->server_rgb_controllers[rgb_controller]->zones.size(); zone_idx++)
                {
                    /*-----------------------------------------------------*\
                    | Create child tree widget items and display the zone   |
                    | names, number of LEDs, and types in them              |
                    \*-----------------------------------------------------*/
                    QTreeWidgetItem* new_child = new QTreeWidgetItem();

                    std::string zone_str = ResourceManager::get()->GetClients()[client_idx]->server_rgb_controllers[rgb_controller]->zones[zone_idx].name + ", ";
                    zone_str.append(std::to_string(ResourceManager::get()->GetClients()[client_idx]->server_rgb_controllers[rgb_controller]->zones[zone_idx].leds_count));
                    zone_str.append(" LEDs, ");

                    switch(ResourceManager::get()->GetClients()[client_idx]->server_rgb_controllers[rgb_controller]->zones[zone_idx].type)
                    {
                        case ZONE_TYPE_SINGLE:
                            zone_str.append("Single");
                        break;

                        case ZONE_TYPE_LINEAR:
                            zone_str.append("Linear");
                            break;

                        case ZONE_TYPE_MATRIX:
                            zone_str.append("Matrix");
                            break;
                    }

                    new_child->setText(0, QString::fromStdString(zone_str));

                    new_sub_item->addChild(new_child);
                }
            }
        }

        /*-----------------------------------------------------*\
        | Add Fan Controllers for each device in the client     |
        \*-----------------------------------------------------*/
        if(ResourceManager::get()->GetClients()[client_idx]->server_fan_controllers.size() > 0)
        {

            QTreeWidgetItem* new_item = new QTreeWidgetItem(new_top_item);
            new_item->setText(0, "Fan Controllers");

            for(std::size_t fan_controller = 0; fan_controller < ResourceManager::get()->GetClients()[client_idx]->server_fan_controllers.size(); fan_controller++)
            {
                /*-----------------------------------------------------*\
                | Create child tree widget items and display the device |
                | names in them                                         |
                \*-----------------------------------------------------*/
                QTreeWidgetItem* new_sub_item = new QTreeWidgetItem(new_item);
                new_sub_item->setText(0, QString::fromStdString(ResourceManager::get()->GetClients()[client_idx]->server_fan_controllers[fan_controller]->name));

                /*-----------------------------------------------------*\
                | Add child items for each fan in the device            |
                \*-----------------------------------------------------*/
                for(std::size_t fan_idx = 0; fan_idx < ResourceManager::get()->GetClients()[client_idx]->server_fan_controllers[fan_controller]->fans.size(); fan_idx++)
                {
                    /*-----------------------------------------------------*\
                    | Create child tree widget items and display the fan    |
                    | names, RPM reading, and speeds                        |
                    \*-----------------------------------------------------*/
                    QTreeWidgetItem* new_child = new QTreeWidgetItem();

                    std::string fan_str = ResourceManager::get()->GetClients()[client_idx]->server_fan_controllers[fan_controller]->fans[fan_idx].name + ": ";
                    fan_str.append(std::to_string(ResourceManager::get()->GetClients()[client_idx]->server_fan_controllers[fan_controller]->fans[fan_idx].rpm_rdg) + " RPM / ");
                    fan_str.append(std::to_string(ResourceManager::get()->GetClients()[client_idx]->server_fan_controllers[fan_controller]->fans[fan_idx].speed_cmd) + "% (");
                    fan_str.append(std::to_string(ResourceManager::get()->GetClients()[client_idx]->server_fan_controllers[fan_controller]->fans[fan_idx].speed_min) + "% -> ");
                    fan_str.append(std::to_string(ResourceManager::get()->GetClients()[client_idx]->server_fan_controllers[fan_controller]->fans[fan_idx].speed_max) + "%)");

                    new_child->setText(0, QString::fromStdString(fan_str));

                    new_sub_item->addChild(new_child);
                }
            }
        }
    }

    /*-----------------------------------------------------*\
    | Emit client information updated signal                |
    \*-----------------------------------------------------*/
    emit ClientListUpdated();
}

void Ui::OpenRGBClientInfoPage::on_ClientConnectButton_clicked()
{
    /*-----------------------------------------------------*\
    | Read the new client IP and Port values from the UI    |
    \*-----------------------------------------------------*/
    unsigned short  port = std::stoi(ui->ClientPortValue->text().toStdString());
    std::string     ip   = ui->ClientIPValue->text().toStdString();

    /*-----------------------------------------------------*\
    | Create a new client and set name, IP, and port values |
    \*-----------------------------------------------------*/
    NetworkClient * rgb_client = new NetworkClient(ResourceManager::get()->GetRGBControllers(), ResourceManager::get()->GetFanControllers());

    std::string titleString = "OpenRGB ";
    titleString.append(VERSION_STRING);

    rgb_client->SetIP(ip.c_str());
    rgb_client->SetName(titleString.c_str());
    rgb_client->SetPort(port);

    rgb_client->StartClient();

    /*-----------------------------------------------------*\
    | Add new client to list and register update callback   |
    \*-----------------------------------------------------*/
    ResourceManager::get()->GetClients().push_back(rgb_client);

    rgb_client->RegisterClientInfoChangeCallback(UpdateInfoCallback, this);
}

void Ui::OpenRGBClientInfoPage::onClientDisconnectButton_clicked(QObject * arg)
{
    /*-----------------------------------------------------*\
    | Get the pointer to the disconnecting client from args |
    \*-----------------------------------------------------*/
    NetworkClient * disconnect_client = ((NetworkClientPointer *)arg)->net_client;

    /*-----------------------------------------------------*\
    | Stop the disconnecting client                         |
    \*-----------------------------------------------------*/
    disconnect_client->StopClient();

    /*-----------------------------------------------------*\
    | Remove disconnecting client from list                 |
    \*-----------------------------------------------------*/
    for(unsigned int client_idx = 0; client_idx < ResourceManager::get()->GetClients().size(); client_idx++)
    {
        if(disconnect_client == ResourceManager::get()->GetClients()[client_idx])
        {
            ResourceManager::get()->GetClients().erase(ResourceManager::get()->GetClients().begin() + client_idx);
            break;
        }
    }

    /*-----------------------------------------------------*\
    | Delete the disconnecting client                       |
    \*-----------------------------------------------------*/
    delete disconnect_client;
}
