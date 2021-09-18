#ifndef REPORT_ITEM_H_
#define REPORT_ITEM_H_

#include <list>
#include <iostream>
#include <string>
#include <stdio.h>
#include "device.h"

using namespace std;

class Fingerprint
{

private:
    string ja3_hash;
    string ja3_string;
    string category;
    string desc;
public:
    Fingerprint():ja3_hash(""),ja3_string("") {}
    Fingerprint(string s3,string s4, string s5):ja3_hash(s3),ja3_string(s4),category(s5) {}
    Fingerprint(const string &s3,const string &s4, const string &s5, const string &s6):ja3_hash(s3),ja3_string(s4),category(s5),desc(s6) {}
    

    string getJa3_hash();
    void setJa3_hash(string &);

    string getJa3_string();
    void setJa3_string(string&);

    string getCategory();
    void setCategory(string&);

    string getDescription();
    void setDescription(string&);

};

class ReportItem: public Device
{

private:
    list<Fingerprint> fingerprints_list;
public:
    ReportItem(): Device() {}
    ReportItem(string& s1, string& s2, string& s3);
    void addFingerprint(Fingerprint);
    list<Fingerprint> getlist();
    void make_uniq();
    bool findFingerprint(string hash);
};

class Report
{

private:
    list<ReportItem> rep;
    ReportItem* find_item_by_ip(string);
public:
    Report() {};
    void add_new_line(string ip, string name, string interface, string hash, string ja3st, string category, string description);
    list<ReportItem> get_whole_list();



};

#endif
