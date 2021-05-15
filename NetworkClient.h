/*-----------------------------------------*\
|  NetworkClient.h                          |
|                                           |
|  Client header for OpenRGB SDK            |
|                                           |
|  Adam Honse (CalcProgrammer1) 5/9/2020    |
\*-----------------------------------------*/

#include "RGBController.h"
#include "FanController.h"
#include "NetworkProtocol.h"
#include "net_port.h"

#include <mutex>
#include <thread>

#pragma once

typedef void (*NetClientCallback)(void *);

class NetworkClient
{
public:
    NetworkClient(std::vector<RGBController *>& rgb_control, std::vector<FanController *>& fan_control);
    ~NetworkClient();

    void            ClientInfoChanged();

    bool            GetConnected();
    const char *    GetIP();
    unsigned short  GetPort();
    unsigned int    GetProtocolVersion();
    bool            GetOnline();

    void            RegisterClientInfoChangeCallback(NetClientCallback new_callback, void * new_callback_arg);

    void            SetIP(const char *new_ip);
    void            SetName(const char *new_name);
    void            SetPort(unsigned short new_port);

    void            StartClient();
    void            StopClient();

    void            ConnectionThreadFunction();
    void            ListenThreadFunction();

    void            WaitOnControllerData();

    void        ProcessReply_RGBControllerCount(unsigned int data_size, char * data);
    void        ProcessReply_RGBControllerData(unsigned int data_size, char * data, unsigned int dev_idx);
    void        ProcessReply_FanControllerCount(unsigned int data_size, char * data);
    void        ProcessReply_FanControllerData(unsigned int data_size, char * data, unsigned int dev_idx);
    void        ProcessReply_ProtocolVersion(unsigned int data_size, char * data);
    void        ProcessReply_FanControllerReading(unsigned int data_size, char * data, unsigned int dev_idx);

    void        ProcessRequest_DeviceListChanged();

    void        SendData_ClientString();

    void        SendRequest_RGBControllerCount();
    void        SendRequest_RGBControllerData(unsigned int dev_idx);
    void        SendRequest_FanControllerCount();
    void        SendRequest_FanControllerData(unsigned int dev_idx);
    void        SendRequest_ProtocolVersion();

    void        SendRequest_RGBController_ResizeZone(unsigned int dev_idx, int zone, int new_size);

    void        SendRequest_RGBController_UpdateLEDs(unsigned int dev_idx, unsigned char * data, unsigned int size);
    void        SendRequest_RGBController_UpdateZoneLEDs(unsigned int dev_idx, unsigned char * data, unsigned int size);
    void        SendRequest_RGBController_UpdateSingleLED(unsigned int dev_idx, unsigned char * data, unsigned int size);

    void        SendRequest_RGBController_SetCustomMode(unsigned int dev_idx);

    void        SendRequest_RGBController_UpdateMode(unsigned int dev_idx, unsigned char * data, unsigned int size);

    void        SendRequest_FanController_UpdateControl(unsigned int dev_idx, unsigned char * data, unsigned int size);
    void        SendRequest_FanController_Reading(unsigned int dev_idx);


    std::vector<std::string> * ProcessReply_ProfileList(unsigned int data_size, char * data);

    void        SendRequest_GetProfileList();
    void        SendRequest_LoadProfile(std::string profile_name);
    void        SendRequest_SaveProfile(std::string profile_name);
    void        SendRequest_DeleteProfile(std::string profile_name);

    std::vector<RGBController *>  server_rgb_controllers;
    std::vector<FanController *>  server_fan_controllers;

    std::mutex                          RGBControllerListMutex;
    std::mutex                          FanControllerListMutex;

protected:
    std::vector<RGBController *>& rgb_controllers;
    std::vector<FanController *>& fan_controllers;


private:
    SOCKET          client_sock;
    std::string     client_name;
    net_port        port;
    char            port_ip[20];
    unsigned short  port_num;
    bool            client_active;
    bool            rgb_controller_data_received;
    bool            fan_controller_data_received;
    bool            server_connected;
    bool            server_initialized;
    unsigned int    server_rgb_controller_count;
    bool            server_rgb_controller_count_received;
    unsigned int    server_fan_controller_count;
    bool            server_fan_controller_count_received;
    unsigned int    server_protocol_version;
    bool            server_protocol_version_received;
	bool            server_fan_controller_reading_received;
    bool            change_in_progress;

    std::thread *   ConnectionThread;
    std::thread *   ListenThread;

    std::mutex                          ClientInfoChangeMutex;
    std::vector<NetClientCallback>      ClientInfoChangeCallbacks;
    std::vector<void *>                 ClientInfoChangeCallbackArgs;

    int recv_select(SOCKET s, char *buf, int len, int flags);
};
