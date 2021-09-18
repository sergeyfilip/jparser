/**
 * @file policy.cpp
 * @author S.Filipovich
 * @date 25 Aug 2018
 * @brief File containing implementation for class Policy
 *
 * Class Policy is used to store data about AWL policies
 * Class viriables - 
 *
 */
#include "policy.h"
#include <string>
#include <algorithm>

using namespace std;

Policy::Policy() {
  
  name = "default";
  
}

string Policy::getName()
{
    return name;
}

void Policy::setName(string s)
{
    name = s;
}

void Policy::add_category(string s) {
  categories.push_front(s);
}
list<string>* Policy::get_categories_list() {
  
  return &categories;
  
}

bool Policy::check_category(string s) {
  std::list<std::string>::iterator it;
 
  // Fetch the iterator of element with value 'the'
  it = std::find(get_categories_list()->begin(), get_categories_list()->end(), s);
 
// Check if iterator points to end or not
  if(it != categories.end())
	return true;
  else  return false;
}

