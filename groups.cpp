/**
 * @file groups.cpp
 * @author S.Filipovich
 * @date 23 Nov 2018
 * @brief File containing implementation for class Devices
 *
 * Class Devices is used to store data about devices connected to CE
 * Class viriables - Device list
 *
 */

#include "groups.h"

Groups::Groups()
{
    gl = new list<Group>;

}
Groups::Groups(const Groups & d)
{
    gl = new Grouplist();
    for(auto f = d.gl->begin(); f != d.gl->end(); f++) {
        gl->push_back((*f));
    }

}

Groups& Groups::operator=(const Groups &d)
{
    if(this != &d) {

        delete gl;
        gl = new Grouplist();
        for(auto f = d.gl->begin(); f != d.gl->end(); f++) {
           gl->push_back((*f));
        }

    }
    return *this;
}

Groups::~Groups()
{
    // TODO Auto-generated destructor stub
}

void Groups::add_group(Group& d)
{

    gl->push_front(d);

}

Group* Groups::find_group_by_name(string s) {


    list<Group>::iterator it;

    for(it = gl->begin(); it != gl->end(); ++it) {

        int found = s.compare((*it).getName());
        if (found == 0) {
            return &(*it);
        }


    }


    return nullptr;



}

Grouplist* Groups::get_list()
{
    return gl;
}
string Groups::get_Net_settings() {
  string s;
  for(auto u = gl->begin(); u != gl->end(); ++u) {
    s.append(u->get_Network_macros()); 
    s.append("\n");
   }
   s.append("AWL_HOME_NET: \"");
   for(auto u = gl->begin(); u != gl->end(); ++u) {
    s.append("$");
       s.append(u->getUpperName()); 
    auto p = u;
    if(++p != gl->end()) s.append(", ");
   }
   s.append("\""); s.append("\n");
   return s;
}

string Groups::get_Group_part_of_rule(string templ) {
  string s;
  list<string> temp;
  for(auto u = gl->begin(); u != gl->end(); ++u) {
    if(!(u->get_devices_list()->get_list()->empty())) {
    string t = u->get_Group_macros(templ);
    if(t != "") {
    temp.push_back(t);
    } }
   }
  for(auto g = temp.begin(); g != temp.end(); ++g) {
   s.append(*g);
   auto p = g;
   if(++p != temp.end()) s.append(",");
  }
    
   return s;
  
  
}
