/**
 * @file devices.cpp
 * @author S.Filipovich
 * @date 23 Nov 2018
 * @brief File containing implementation for class Devices
 *
 * Class Devices is used to store data about devices connected to CE
 * Class viriables - Device list
 *
 */

#include "devices.h"

Devices::Devices()
{
    dl = new list<Device>;

}
Devices::Devices(const Devices & d)
{
    dl = new Devicelist();
    for(auto f = d.dl->begin(); f != d.dl->end(); f++) {
        dl->push_back((*f));
    }

}

Devices& Devices::operator=(const Devices &d)
{
    if(this != &d) {

        delete dl;
        dl = new Devicelist();
        for(auto f = d.dl->begin(); f != d.dl->end(); f++) {
            dl->push_back((*f));
        }

    }
    return *this;
}

Devices::~Devices()
{
    // TODO Auto-generated destructor stub
}

void Devices::add_device(Device& d)
{

    dl->push_front(d);

}

Device* Devices::find_device_by_ip(string s)
{


    list<Device>::iterator it;

    for(it = dl->begin(); it != dl->end(); ++it) {

        int found = s.compare((*it).getIp());
        if (found == 0) {
            return &(*it);
        }


    }


    return nullptr;



}

Devicelist* Devices::get_list()
{
    return dl;
}
