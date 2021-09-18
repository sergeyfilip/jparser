/**
 * @file devices.h
 * @author S.Filipovich
 * @date 23 Nov 2018
 * @brief File containing class Devices - list of network mapping devices
 *
 * Class Devices is used to store data about devices connected to CE
 * Class viriables - Device list
 *
 */

#ifndef DEVICES_H_
#define DEVICES_H_

#include <list>
#include <iostream>
#include <string>
#include <stdio.h>
#include "device.h"

using namespace std;
//Type for devices list
typedef std::list<Device> Devicelist;

class Devices
{

private:
    Devicelist* dl;
public:
    /**
     * Constructors
     *
     */
    Devices();
    Devices(const Devices &);
    Devices& operator=(const Devices &);
    virtual ~Devices();
    /** @brief Add new device to list dl
     *
     *  @param reference to new device added
     *  @return Void.
     */
    void add_device(Device& d);
    /** @brief Find device in list dl by ip
     *
     *  @param string ip
     *  @return pointer to device or nullptr
     */
    Device* find_device_by_ip(string);
    /** @brief return pointer to devices kist
     *
     *  @return pointer to device list
     */
    Devicelist* get_list();
};

#endif /* DEVICES_H_ */
