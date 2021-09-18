/**
 * @file group.cpp
 * @author S.Filipovich
 * @date 27 Aug 2018
 * @brief File containing implementation for class Group
 *
 * Class Group is used to store data about AWL Group
 * Class viriables - 
 *
 */
#include "policy.h"
#include "group.h"
#include "helper.h"
#include <string>

using namespace std;


string Group::getName()
{
    return name;
}

void Group::setName(string s)
{
    name = s;
}

Policy* Group::getPolicy()
{
    return pol;
}

void Group::setPolicy(Policy* p)
{
    pol = p;
}

Devices* Group::get_devices_list(){
  
    return dv;
}
void Group::add_device(Device& d){
  
  dv->add_device(d);
  
}

string Group::generate_group_vars() {
  Upper n;
  string gn = n(name);
  return gn;
}

string Group::get_Group_macros(string st) {
  string s ="";
  Policy* pt = getPolicy();
  Upper up;
  if(pt!= nullptr) {
    if(pt->check_category(st)) s= string("$").append(up(getName()));
  }
  return s;
  
}

string Group::get_Network_macros() {
  Devices* d = get_devices_list();
  string s;
  Upper up;
  if(d) {
    s = (up(getName()));
    s.append(": \"[");
    for(auto f = d->get_list()->begin(); f != d->get_list()->end(); ++f) {
      s.append(f->getIp()); 
      auto g = f;
      if(++g != d->get_list()->end()) s.append(",");
    }
    s.append("]\"");
    
  }
  return s;
}
string Group::getUpperName()
{
    Upper up;
    return up(name);
}
