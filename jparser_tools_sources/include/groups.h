/**
 * @file groups.h
 * @author S.Filipovich
 * @date 10 Sep 2019
 * @brief File containing class Devices - list of network mapping devices
 *
 * Class Devices is used to store data about devices connected to CE
 * Class viriables - Device list
 *
 */

#ifndef GROUPS_H_
#define GROUPS_H_

#include <list>
#include <iostream>
#include <string>
#include <stdio.h>
#include "device.h"
#include "group.h"


using namespace std;
//Type for devices list
typedef std::list<Group> Grouplist;

class Groups
{

private:
    Grouplist* gl;
public:
    /**
     * Constructors
     *
     */
    Groups();
    Groups(const Groups &);
    Groups& operator=(const Groups &);
    virtual ~Groups();
    /** @brief Add new group to list dl
     *
     *  @param reference to new group added
     *  @return Void.
     */
    void add_group(Group& d);
    /** @brief Find group in list dl by ip
     *
     *  @param string name
     *  @return pointer to group or nullptr
     */
    Group* find_group_by_name(string);
    /** @brief return pointer to groups list
     *
     *  @return pointer to group list
     */
    Grouplist* get_list();
    
    string get_Net_settings();
    string get_Group_part_of_rule(string);
};

#endif /* GROUPS_H_ */
