#include "FanController.h"
#include <cstring>

unsigned char * FanController::GetDeviceDescription()
{
    unsigned int data_ptr = 0;
    unsigned int data_size = 0;

    /*---------------------------------------------------------*\
    | Calculate data size                                       |
    \*---------------------------------------------------------*/
    unsigned short name_len         = strlen(name.c_str())          + 1;
    unsigned short description_len  = strlen(description.c_str())   + 1;
    unsigned short version_len      = strlen(version.c_str())       + 1;
    unsigned short serial_len       = strlen(serial.c_str())        + 1;
    unsigned short location_len     = strlen(location.c_str())      + 1;
    unsigned short num_fans         = fans.size();

    unsigned short *fan_name_len    = new unsigned short[num_fans];

    data_size += sizeof(data_size);
    data_size += name_len           + sizeof(name_len);
    data_size += description_len    + sizeof(description_len);
    data_size += version_len        + sizeof(version_len);
    data_size += serial_len         + sizeof(serial_len);
    data_size += location_len       + sizeof(location_len);

    data_size += sizeof(num_fans);

    for(int fan_index = 0; fan_index < num_fans; fan_index++)
    {
        fan_name_len[fan_index]     = strlen(fans[fan_index].name.c_str()) + 1;

        data_size += fan_name_len[fan_index] + sizeof(fan_name_len[fan_index]);
        data_size += sizeof(fans[fan_index].speed_cmd);
        data_size += sizeof(fans[fan_index].speed_min);
        data_size += sizeof(fans[fan_index].speed_max);
        data_size += sizeof(fans[fan_index].rpm_rdg);
    }

    /*---------------------------------------------------------*\
    | Create data buffer                                        |
    \*---------------------------------------------------------*/
    unsigned char *data_buf = new unsigned char[data_size];

    /*---------------------------------------------------------*\
    | Copy in data size                                         |
    \*---------------------------------------------------------*/
    memcpy(&data_buf[data_ptr], &data_size, sizeof(data_size));
    data_ptr += sizeof(data_size);

    /*---------------------------------------------------------*\
    | Copy in name (size+data)                                  |
    \*---------------------------------------------------------*/
    memcpy(&data_buf[data_ptr], &name_len, sizeof(unsigned short));
    data_ptr += sizeof(unsigned short);

    strcpy((char *)&data_buf[data_ptr], name.c_str());
    data_ptr += name_len;

    /*---------------------------------------------------------*\
    | Copy in description (size+data)                           |
    \*---------------------------------------------------------*/
    memcpy(&data_buf[data_ptr], &description_len, sizeof(unsigned short));
    data_ptr += sizeof(unsigned short);

    strcpy((char *)&data_buf[data_ptr], description.c_str());
    data_ptr += description_len;

    /*---------------------------------------------------------*\
    | Copy in version (size+data)                               |
    \*---------------------------------------------------------*/
    memcpy(&data_buf[data_ptr], &version_len, sizeof(unsigned short));
    data_ptr += sizeof(unsigned short);

    strcpy((char *)&data_buf[data_ptr], version.c_str());
    data_ptr += version_len;

    /*---------------------------------------------------------*\
    | Copy in serial (size+data)                                |
    \*---------------------------------------------------------*/
    memcpy(&data_buf[data_ptr], &serial_len, sizeof(unsigned short));
    data_ptr += sizeof(unsigned short);

    strcpy((char *)&data_buf[data_ptr], serial.c_str());
    data_ptr += serial_len;

    /*---------------------------------------------------------*\
    | Copy in location (size+data)                              |
    \*---------------------------------------------------------*/
    memcpy(&data_buf[data_ptr], &location_len, sizeof(unsigned short));
    data_ptr += sizeof(unsigned short);

    strcpy((char *)&data_buf[data_ptr], location.c_str());
    data_ptr += location_len;

    /*---------------------------------------------------------*\
    | Copy in number of fans (data)                             |
    \*---------------------------------------------------------*/
    memcpy(&data_buf[data_ptr], &num_fans, sizeof(unsigned short));
    data_ptr += sizeof(unsigned short);

    /*---------------------------------------------------------*\
    | Copy in fans                                              |
    \*---------------------------------------------------------*/
    for(int fan_index = 0; fan_index < num_fans; fan_index++)
    {
        /*---------------------------------------------------------*\
        | Copy in fan name (size+data)                              |
        \*---------------------------------------------------------*/
        memcpy(&data_buf[data_ptr], &fan_name_len[fan_index], sizeof(unsigned short));
        data_ptr += sizeof(unsigned short);

        strcpy((char *)&data_buf[data_ptr], fans[fan_index].name.c_str());
        data_ptr += fan_name_len[fan_index];

        /*---------------------------------------------------------*\
        | Copy in fan speed_cmd (data)                              |
        \*---------------------------------------------------------*/
        memcpy(&data_buf[data_ptr], &fans[fan_index].speed_cmd, sizeof(fans[fan_index].speed_cmd));
        data_ptr += sizeof(fans[fan_index].speed_cmd);

        /*---------------------------------------------------------*\
        | Copy in fan speed_min (data)                              |
        \*---------------------------------------------------------*/
        memcpy(&data_buf[data_ptr], &fans[fan_index].speed_min, sizeof(fans[fan_index].speed_min));
        data_ptr += sizeof(fans[fan_index].speed_min);

        /*---------------------------------------------------------*\
        | Copy in fan speed_max (data)                              |
        \*---------------------------------------------------------*/
        memcpy(&data_buf[data_ptr], &fans[fan_index].speed_max, sizeof(fans[fan_index].speed_max));
        data_ptr += sizeof(fans[fan_index].speed_max);

        /*---------------------------------------------------------*\
        | Copy in fan rpm_rdg (data)                                |
        \*---------------------------------------------------------*/
        memcpy(&data_buf[data_ptr], &fans[fan_index].rpm_rdg, sizeof(fans[fan_index].rpm_rdg));
        data_ptr += sizeof(fans[fan_index].rpm_rdg);
    }

    delete[] fan_name_len;

    return(data_buf);
}

void FanController::ReadDeviceDescription(unsigned char* data_buf)
{
	unsigned int data_ptr = 0;

    data_ptr += sizeof(unsigned int);

    /*---------------------------------------------------------*\
    | Copy in name                                              |
    \*---------------------------------------------------------*/
    unsigned short name_len;
    memcpy(&name_len, &data_buf[data_ptr], sizeof(unsigned short));
    data_ptr += sizeof(unsigned short);

    name = (char *)&data_buf[data_ptr];
    data_ptr += name_len;

    /*---------------------------------------------------------*\
    | Copy in description                                       |
    \*---------------------------------------------------------*/
    unsigned short description_len;
    memcpy(&description_len, &data_buf[data_ptr], sizeof(unsigned short));
    data_ptr += sizeof(unsigned short);

    description = (char *)&data_buf[data_ptr];
    data_ptr += description_len;

    /*---------------------------------------------------------*\
    | Copy in version                                           |
    \*---------------------------------------------------------*/
    unsigned short version_len;
    memcpy(&version_len, &data_buf[data_ptr], sizeof(unsigned short));
    data_ptr += sizeof(unsigned short);

    version = (char *)&data_buf[data_ptr];
    data_ptr += version_len;

    /*---------------------------------------------------------*\
    | Copy in serial                                            |
    \*---------------------------------------------------------*/
    unsigned short serial_len;
    memcpy(&serial_len, &data_buf[data_ptr], sizeof(unsigned short));
    data_ptr += sizeof(unsigned short);

    serial = (char *)&data_buf[data_ptr];
    data_ptr += serial_len;

    /*---------------------------------------------------------*\
    | Copy in location                                          |
    \*---------------------------------------------------------*/
    unsigned short location_len;
    memcpy(&location_len, &data_buf[data_ptr], sizeof(unsigned short));
    data_ptr += sizeof(unsigned short);

    location = (char *)&data_buf[data_ptr];
    data_ptr += location_len;

    /*---------------------------------------------------------*\
    | Copy in number of fans (data)                             |
    \*---------------------------------------------------------*/
    unsigned short num_fans;
    memcpy(&num_fans, &data_buf[data_ptr], sizeof(unsigned short));
    data_ptr += sizeof(unsigned short);

    /*---------------------------------------------------------*\
    | Copy in fans                                              |
    \*---------------------------------------------------------*/
    for(int fan_index = 0; fan_index < num_fans; fan_index++)
    {
        fan new_fan;

        /*---------------------------------------------------------*\
        | Copy in fan name (size+data)                              |
        \*---------------------------------------------------------*/
        unsigned short fan_name_len;
        memcpy(&fan_name_len, &data_buf[data_ptr], sizeof(unsigned short));
        data_ptr += sizeof(unsigned short);

        new_fan.name = (char *)&data_buf[data_ptr];
        data_ptr += fan_name_len;

        /*---------------------------------------------------------*\
        | Copy in fan speed_cmd (data)                              |
        \*---------------------------------------------------------*/
        memcpy(&new_fan.speed_cmd, &data_buf[data_ptr], sizeof(new_fan.speed_cmd));
        data_ptr += sizeof(new_fan.speed_cmd);

        /*---------------------------------------------------------*\
        | Copy in fan speed_min (data)                              |
        \*---------------------------------------------------------*/
        memcpy(&new_fan.speed_min, &data_buf[data_ptr], sizeof(new_fan.speed_min));
        data_ptr += sizeof(new_fan.speed_min);

        /*---------------------------------------------------------*\
        | Copy in fan speed_max (data)                              |
        \*---------------------------------------------------------*/
        memcpy(&new_fan.speed_max, &data_buf[data_ptr], sizeof(new_fan.speed_max));
        data_ptr += sizeof(new_fan.speed_max);

        /*---------------------------------------------------------*\
        | Copy in fan rpm_rdg (data)                                |
        \*---------------------------------------------------------*/
        memcpy(&new_fan.rpm_rdg, &data_buf[data_ptr], sizeof(new_fan.rpm_rdg));
        data_ptr += sizeof(new_fan.rpm_rdg);

        fans.push_back(new_fan);
    }
}

unsigned char * FanController::GetFansRpmDescription()
{
    unsigned int data_ptr = 0;
    unsigned int data_size = 0;

    unsigned short num_fans = fans.size();

    /*---------------------------------------------------------*\
    | Calculate data size                                       |
    \*---------------------------------------------------------*/
    data_size += sizeof(data_size);
    data_size += sizeof(num_fans);
    data_size += num_fans * sizeof(fan().rpm_rdg);

    /*---------------------------------------------------------*\
    | Create data buffer                                        |
    \*---------------------------------------------------------*/
    unsigned char *data_buf = new unsigned char[data_size];

    /*---------------------------------------------------------*\
    | Copy in data size                                         |
    \*---------------------------------------------------------*/
    memcpy(&data_buf[data_ptr], &data_size, sizeof(data_size));
    data_ptr += sizeof(data_size);

    /*---------------------------------------------------------*\
    | Copy in number of fans (data)                             |
    \*---------------------------------------------------------*/
    memcpy(&data_buf[data_ptr], &num_fans, sizeof(unsigned short));
    data_ptr += sizeof(unsigned short);

    /*---------------------------------------------------------*\
    | Copy in fans                                              |
    \*---------------------------------------------------------*/
    for(int fan_index = 0; fan_index < num_fans; fan_index++)
    {
        /*---------------------------------------------------------*\
        | Copy in fan rpm_rdg (data)                                |
        \*---------------------------------------------------------*/
        memcpy(&data_buf[data_ptr], &fans[fan_index].rpm_rdg, sizeof(fans[fan_index].rpm_rdg));
        data_ptr += sizeof(fans[fan_index].rpm_rdg);
    }

    return(data_buf);
}

void FanController::SetFansRpmDescription(unsigned char* data_buf)
{
    unsigned int data_ptr = sizeof(unsigned int);

    /*---------------------------------------------------------*\
    | Copy in number of fans (data)                             |
    \*---------------------------------------------------------*/
    unsigned short num_fans;
    memcpy(&num_fans, &data_buf[data_ptr], sizeof(unsigned short));
    data_ptr += sizeof(unsigned short);

    /*---------------------------------------------------------*\
    | Check if we aren't reading beyond the list of fans.       |
    \*---------------------------------------------------------*/
    if(((size_t) num_fans) > fans.size())
    {
        return;
    }

    /*---------------------------------------------------------*\
    | Copy in fans                                              |
    \*---------------------------------------------------------*/
    for(int fan_index = 0; fan_index < num_fans; fan_index++)
    {
		unsigned int new_rpm_rdg;

        /*---------------------------------------------------------*\
        | Copy in fan rpm_rdg (data)                                |
        \*---------------------------------------------------------*/
        memcpy(&new_rpm_rdg, &data_buf[data_ptr], sizeof(fans[fan_index].rpm_rdg));
        data_ptr += sizeof(fans[fan_index].rpm_rdg);

        fans[fan_index].rpm_rdg = new_rpm_rdg;
    }
}

unsigned char * FanController::GetFansCmdDescription()
{
    unsigned int data_ptr = 0;
    unsigned int data_size = 0;

    unsigned short num_fans = fans.size();

    /*---------------------------------------------------------*\
    | Calculate data size                                       |
    \*---------------------------------------------------------*/
    data_size += sizeof(data_size);
    data_size += sizeof(num_fans);
    data_size += num_fans * sizeof(fan().speed_cmd);

    /*---------------------------------------------------------*\
    | Create data buffer                                        |
    \*---------------------------------------------------------*/
    unsigned char *data_buf = new unsigned char[data_size];

    /*---------------------------------------------------------*\
    | Copy in data size                                         |
    \*---------------------------------------------------------*/
    memcpy(&data_buf[data_ptr], &data_size, sizeof(data_size));
    data_ptr += sizeof(data_size);

    /*---------------------------------------------------------*\
    | Copy in number of fans (data)                             |
    \*---------------------------------------------------------*/
    memcpy(&data_buf[data_ptr], &num_fans, sizeof(unsigned short));
    data_ptr += sizeof(unsigned short);

    /*---------------------------------------------------------*\
    | Copy in fans                                              |
    \*---------------------------------------------------------*/
    for(int fan_index = 0; fan_index < num_fans; fan_index++)
    {
        /*---------------------------------------------------------*\
        | Copy in fan speed_cmd (data)                              |
        \*---------------------------------------------------------*/
        memcpy(&data_buf[data_ptr], &fans[fan_index].speed_cmd, sizeof(fans[fan_index].speed_cmd));
        data_ptr += sizeof(fans[fan_index].speed_cmd);
    }

    return(data_buf);
}

void FanController::SetFansCmdDescription(unsigned char* data_buf)
{
    unsigned int data_ptr = sizeof(unsigned int);

    /*---------------------------------------------------------*\
    | Copy in number of fans (data)                             |
    \*---------------------------------------------------------*/
    unsigned short num_fans;
    memcpy(&num_fans, &data_buf[data_ptr], sizeof(unsigned short));
    data_ptr += sizeof(unsigned short);

    /*---------------------------------------------------------*\
    | Check if we aren't reading beyond the list of fans.       |
    \*---------------------------------------------------------*/
    if(((size_t) num_fans) > fans.size())
    {
        return;
    }

    /*---------------------------------------------------------*\
    | Copy in fans                                              |
    \*---------------------------------------------------------*/
    for(int fan_index = 0; fan_index < num_fans; fan_index++)
    {
		unsigned int new_speed_cmd;

        /*---------------------------------------------------------*\
        | Copy in fan speed_cmd (data)                              |
        \*---------------------------------------------------------*/
        memcpy(&new_speed_cmd, &data_buf[data_ptr], sizeof(fans[fan_index].speed_cmd));
        data_ptr += sizeof(fans[fan_index].speed_cmd);

        fans[fan_index].speed_cmd = new_speed_cmd;
    }
}
