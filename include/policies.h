/**
 * @file policies.h
 * @author S.Filipovich
 * @date 25 August 2019
 * @brief File containing class Policies for AWL
 *
 * Class Policies is used to store data about AWL policies
 * Class viriables - 
 *
 */

#ifndef POLICIES_H_

#define POLICIES_H_

#include <list>
#include <iostream>
#include <string>
#include <stdio.h>
#include "policy.h"

using namespace std;
//Type for devices list
typedef std::list<Policy> Policylist;

class Policies
{

private:
    Policylist* dl;
public:
    /**
     * Constructors
     *
     */
    Policies();
    Policies(const Policies &);
    Policies& operator=(const Policies &);
    virtual ~Policies();
    /** @brief Add new device to list dl
     *
     *  @param reference to new device added
     *  @return Void.
     */
    void add_policy(Policy& d);
    /** @brief Find device in list dl by ip
     *
     *  @param string ip
     *  @return pointer to device or nullptr
     */
    Policy* find_policy_by_name(string);
    /** @brief return pointer to devices kist
     *
     *  @return pointer to device list
     */
    Policylist* get_list();
};

#endif /* POLICIESES_H_ */
