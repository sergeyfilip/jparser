/**
 * @file helper.h
 * @author S.Filipovich
 * @date 25 August 2019
 * @brief File containing class Group for AWL
 *
 * Class Group is used to store data about AWL group
 * Class viriables - 
 *
 */

#ifndef HELPER_H_
#define HELPER_H_
#include <syslog.h>
#include <unistd.h>
#include <sys/types.h>

#include <stddef.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <grp.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <jsoncpp/json/json.h> // or something
#include <map>

#include <iterator>

#include <cstring>
#include "settings.h"
#include <list>
#include <iostream>
#include <string>
#include <stdio.h>
#include "policy.h"
#include "devices.h"
#include "item.h"

using namespace std;

struct Upper
{
  

public:
  
  string operator()(std::string str) 
    {
        
    for(auto& x: str)
        x = toupper(x);
    return str;
    }
    
};

long get_sid();
void set_sid(long);
int check_template(const char * path_to_file, const char* templ);
void modify_file(const char * path_to_file, int row_number, const string templ);
void delete_from_file(const char * path_to_file, int row_number);
void check();
string findCategory(list<Item> l, string s);
#endif
