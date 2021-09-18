#include "./include/item.h"
#include <string>

using namespace std;

string Item::getDesc()
{
    return desc;
}

void Item::setDesc(string s)
{
    desc = s;
}

string Item::getJa3_hash()
{
    return ja3_hash;
}

void Item::setJa3_hash(string s)
{
    ja3_hash = s;
}

string Item::getJa3_string()
{
    return ja3_string;
}

void Item::setJa3_string(string s)
{
    ja3_string = s;
}

string Item::getCategory()
{
    return category;
}

void Item::setCategory(string s)
{
    category = s;
}



void Item::findCategory(list<string> l)
{

    list<string>::iterator it;
    for(it = l.begin(); it != l.end(); ++it) {
        std::size_t found = getDesc().find(*it);
        if (found!=std::string::npos) {
            this->setCategory(*it);
            return;
        }


    }
    string os = "Other Software";
    this->setCategory(os);
    return;



}
