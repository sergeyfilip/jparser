/**
 * @file ruleitem.h
 * @author S.Filipovich
 * @date 18 Nov 2018
 * @brief File containing class Device for network mapping
 *
 * Class RuleItem  is used to manage data about fingerprints collected from elasticsearch
 * Class members - description, hash, ja3 string and category, destination ip, source ip
 *
 */
#ifndef RULE_ITEM_H_
#define RULE_ITEM_H_
#include <list>
#include <iostream>
#include <string>
#include <stdio.h>
using namespace std;
class RuleItem
{

private:
    string dst_ip;
    string src_ip;
    string ja3_hash;
    string ja3_string;
    string category;

public:
    RuleItem():dst_ip(""),src_ip(""),ja3_hash(""),ja3_string("") {}
    RuleItem(string s1,string s2,string s3, string s4):dst_ip(s1),src_ip(s2),ja3_hash(s3),ja3_string(s4) {}
    RuleItem(string s1,string s2,string s3,string s4, string s5):dst_ip(s1),src_ip(s2),ja3_hash(s3),ja3_string(s4),category(s5) {}

    string getDstIP();
    void setDstIP(string);

    string getSrcIP();
    void setSrcIP(string);

    string getJa3_hash();
    void setJa3_hash(string);

    string getJa3_string();
    void setJa3_string(string);

    string getCategory();
    void setCategory(string);

    string get_stamp();

    string getItemHash();
};



#endif
