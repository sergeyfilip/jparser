/**
 * @file device.cpp
 * @author S.Filipovich
 * @date 23 Nov 2018
 * @brief File containing implementation for class Device for network mapping
 *
 * Class Device is used to store data about devices connected to CE
 * Class viriables - ip, name, network interface
 *
 */
#include "device.h"
#include <string>

using namespace std;

string Device::getIp()
{
    return ip;
}

void Device::setIp(string& s)
{
    ip = s;
}

string Device::getName()
{
    return name;
}

void Device::setName(string s)
{
    name = s;
}
string Device::getNetwork_interface()
{
    return network_interface;
}

void Device::setNetwork_interface(string& s)
{
    network_interface = s;
}






