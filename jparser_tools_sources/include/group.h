/**
 * @file group.h
 * @author S.Filipovich
 * @date 25 August 2019
 * @brief File containing class Group for AWL
 *
 * Class Group is used to store data about AWL group
 * Class viriables - 
 *
 */

#ifndef GROUP_H_
#define GROUP_H_

#include <list>
#include <iostream>
#include <string>
#include <stdio.h>
#include "policy.h"
#include "devices.h"
#include "helper.h"

using namespace std;


class Group
{

private:
    string name;
    Policy* pol;
    Devices* dv;

public:
    /**
     * Constructors
     *
     */

    Group():name(""),pol(nullptr),dv(nullptr){}
    Group(string s1, Policy* p, Devices* d):name(s1),pol(p),dv(d){}
    
    
    /**
     * Getters and setters
     *
     */
    string getName();
    string getUpperName();
    void setName(string);
    
    Policy* getPolicy();
    void setPolicy(Policy*);

    void add_device(Device&);
    Devices* get_devices_list();
    
    string generate_group_vars();
    
    string get_Group_macros(string category);
    
    string get_Network_macros();


};



#endif /* GROUP_H_ */
