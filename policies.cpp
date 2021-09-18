/**
 * @file policies.cpp
 * @author S.Filipovich
 * @date 28 Aug 2019
 * @brief File containing implementation for class Policiess
 *
 * Class Devices is used to store data about devices connected to CE
 * Class viriables - 
 *
 */

#include "policies.h"

Policies::Policies()
{
    dl = new list<Policy>;

}
Policies::Policies(const Policies & d)
{
    dl = new Policylist();
    for(auto f = d.dl->begin(); f != d.dl->end(); f++) {
        dl->push_back((*f));
    }

}

Policies& Policies::operator=(const Policies &d)
{
    if(this != &d) {

        delete dl;
        dl = new Policylist();
        for(auto f = d.dl->begin(); f != d.dl->end(); f++) {
            dl->push_back((*f));
        }

    }
    return *this;
}

Policies::~Policies()
{
    // TODO Auto-generated destructor stub
}

void Policies::add_policy(Policy& d)
{

    dl->push_front(d);

}

Policy* Policies::find_policy_by_name(string s)
{


    list<Policy>::iterator it;

    for(it = dl->begin(); it != dl->end(); ++it) {

        int found = s.compare((*it).getName());
        if (found == 0) {
            return &(*it);
        }


    }


    return nullptr;



}

Policylist* Policies::get_list()
{
    return dl;
}
