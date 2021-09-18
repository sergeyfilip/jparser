/**
 * @file device.h
 * @author S.Filipovich
 * @date 23 Nov 2018
 * @brief File containing class Device for network mapping
 *
 * Class Device is used to store data about devices connected to CE
 * Class viriables - ip, name, network interface
 *
 */

#ifndef DEVICE_H_
#define DEVICE_H_

#include <list>
#include <iostream>
#include <string>
#include <stdio.h>

using namespace std;

/**
 * @brief Class Device is used to store data about devices connected to CE
 *
 * @details Class is used in c++ utilities to get and store device info
 *
 */
class Device
{

private:
    string ip;
    string name;
    string network_interface;

public:
    /**
     * Constructors
     *
     */

    Device():ip(""),name(""),network_interface("") {}
    Device(string s1,string s2,string s3):ip(s1),name(s2),network_interface(s3) {}
    Device(string s1,string s2):ip(s1),name(s2) {}
    /**
     * Getters and setters
     *
     */
    string getName();
    void setName(string);

    string getIp();
    void setIp(string&);

    string getNetwork_interface();
    void setNetwork_interface(string&);


};




#endif /* DEVICE_H_ */
