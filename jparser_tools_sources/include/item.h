/**
 * @file item.h
 * @author S.Filipovich
 * @date 19 Nov 2018
 * @brief File containing class Device for network mapping
 *
 * Class Item  is used to manage data about fingerprints collected from database
 * Class members - description, hash, ja3 string and category
 *
 */
#ifndef ITEM_H_
#define ITEM_H_
#include <list>
#include <iostream>
#include <string>
#include <stdio.h>
using namespace std;
class Item
{

private:
    string desc;
    string ja3_hash;
    string ja3_string;
    string category;
public:
    /**
     * Constructors
     *
     */
    Item():desc(""),ja3_hash(""),ja3_string("") {}
    Item(string s1,string s2,string s3):desc(s1),ja3_hash(s2),ja3_string(s3) {}
    Item(string s1,string s2,string s3,string s4):desc(s1),ja3_hash(s2),ja3_string(s3),category(s4) {}

    string getDesc();
    void setDesc(string);

    string getJa3_hash();
    void setJa3_hash(string);

    string getJa3_string();
    void setJa3_string(string);

    string getCategory();
    void setCategory(string);



    void findCategory(list<string>);

};



#endif
