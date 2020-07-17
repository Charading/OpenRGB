/******************************************************************************************\
*                                                                                          *
*   main.cpp                                                                               *
*                                                                                          *
*       Main function for OpenAuraSDK GUI project                                          *
*                                                                                          *
\******************************************************************************************/

#include "NetworkClient.h"
#include "NetworkServer.h"
#include "OpenRGB.h"
#include "ProfileManager.h"
#include "RGBController.h"
#include "i2c_smbus.h"
#include <vector>
#include <stdio.h>
#include <stdlib.h>

#include "OpenRGBDialog2.h"

using namespace std::chrono_literals;

std::vector<NetworkClient*> clients;

extern std::vector<i2c_smbus_interface*> busses;
extern std::vector<RGBController*> rgb_controllers;

/*-------------------------------------------------------------*\
| Command line functionality and return flags                   |
\*-------------------------------------------------------------*/
extern unsigned int cli_main(int argc, char *argv[], std::vector<RGBController *> &rgb_controllers, ProfileManager* profile_manager_in, NetworkServer* network_server_in, std::vector<NetworkClient*> &clients);

enum
{
    RET_FLAG_PRINT_HELP         = 1,
    RET_FLAG_START_GUI          = 2,
    RET_FLAG_I2C_TOOLS          = 4,
    RET_FLAG_START_MINIMIZED    = 8,
};

/******************************************************************************************\
*                                                                                          *
*   AttemptLocalConnection                                                                 *
*                                                                                          *
*       Attempts an SDK connection to the local server.  Returns true if success           *
*                                                                                          *
\******************************************************************************************/

bool AttemptLocalConnection()
{
    bool success = false;

    NetworkClient * client = new NetworkClient(rgb_controllers);
    client->StartClient();

    for(int timeout = 0; timeout < 10; timeout++)
    {
        if(client->GetConnected())
        {
            break;
        }
        std::this_thread::sleep_for(5ms);
    }

    if(!client->GetConnected())
    {
        client->StopClient();

        delete client;

        client = NULL;
    }
    else
    {
        clients.push_back(client);

        success = true;
    }

    return success;
}

/******************************************************************************************\
*                                                                                          *
*   main                                                                                   *
*                                                                                          *
*       Main function.  Detects busses and Aura controllers, then opens the main window    *
*                                                                                          *
\******************************************************************************************/

int main(int argc, char* argv[])
{
    ProfileManager profile_manager(rgb_controllers);
    NetworkServer server(rgb_controllers);
    
    if(!AttemptLocalConnection())
    {
        DetectRGBControllers();
    }

    profile_manager.LoadSizeFromProfile("sizes.ors");

    /*---------------------------------------------------------*\
    | Process command line arguments                            |
    \*---------------------------------------------------------*/
    unsigned int ret_flags = RET_FLAG_START_GUI;
    if(argc > 1)
    {
        ret_flags = cli_main(argc, argv, rgb_controllers, &profile_manager, &server, clients);
    }

    /*---------------------------------------------------------*\
    | If the command line parser indicates that the GUI should  |
    | run, or if there were no command line arguments, start the|
    | GUI.                                                      |
    \*---------------------------------------------------------*/
    if(ret_flags & RET_FLAG_START_GUI)
    {
        QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
        QApplication a(argc, argv);

        Ui::OpenRGBDialog2 dlg(busses, rgb_controllers, &profile_manager);

        if(ret_flags & RET_FLAG_I2C_TOOLS)
        {
            dlg.AddI2CToolsPage();
        }

        if(clients.size() == 0)
        {
            dlg.AddServerTab(&server);
        }

        dlg.AddClientTab();

        for(int client_idx = 0; client_idx < clients.size(); client_idx++)
        {
            dlg.AddClient(clients[client_idx]);
        }
        
        if(ret_flags & RET_FLAG_START_MINIMIZED)
        {
            dlg.hide();
        }
        else
        {
            dlg.show();
        }
        
        return a.exec();
    }
    else
    {
        return 0;
    }
}
