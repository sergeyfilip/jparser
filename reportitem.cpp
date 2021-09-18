#include "reportitem.h"

using namespace std;

bool same_fingerprint (Fingerprint first, Fingerprint second)
{
    string s1 = first.getJa3_hash();
    string s2 = second.getJa3_hash();
    if (s1 == s2) return true;

    return false;
}


string Fingerprint::getJa3_hash()
{
    return ja3_hash;
}

void Fingerprint::setJa3_hash(string& s)
{
    ja3_hash = s;
}

string Fingerprint::getJa3_string()
{
    return ja3_string;
}

void Fingerprint::setJa3_string(string& s)
{
    ja3_string = s;
}

string Fingerprint::getCategory()
{
    return category;
}

void Fingerprint::setCategory(string& s)
{
    category = s;
}

string Fingerprint::getDescription()
{
    if(desc.length() > 2 )  return desc;
    else return "Unknown Description";
}

void Fingerprint::setDescription(string& s)
{
    desc = s;
}

ReportItem::ReportItem(string& s1, string& s2, string& s3)
{

    this->setIp(s1);
    this->setName(s2);
    this->setNetwork_interface(s3);
    if(fingerprints_list.size() > 0) fingerprints_list.clear();
}

void ReportItem::addFingerprint(Fingerprint f)
{

    fingerprints_list.push_back(f);


}

list<Fingerprint> ReportItem::getlist()
{

    return fingerprints_list;

}

list<ReportItem> Report::get_whole_list()
{

    return rep;

}

ReportItem* Report::find_item_by_ip(string ip)
{

    list<ReportItem>::iterator it;
    if(!rep.empty()) {
        for(it = rep.begin(); it != rep.end(); ++it) {

            int  found = ip.compare((*it).getIp());
            if (found == 0) {
                return &(*it);
            }


        }
    }

    return nullptr;

}

bool ReportItem::findFingerprint(string hash)
{
    list<Fingerprint>::iterator it;
    if(!fingerprints_list.empty()) {
        for(it = fingerprints_list.begin(); it != fingerprints_list.end(); ++it) {

            if(hash == ((*it).getJa3_hash())) return true;



        }
    }

    return false;



}

void ReportItem::make_uniq()
{

    fingerprints_list.unique(same_fingerprint);
}

void Report::add_new_line(string ip, string name, string interface, string hash, string ja3st, string category, string description)
{

    ReportItem* r;

    r = find_item_by_ip(ip);
    if(r != nullptr) {
        Fingerprint f = Fingerprint(hash,ja3st,category,description);
        if(!r->findFingerprint(hash))

            r->addFingerprint(f);


    } else {
        r = new ReportItem(ip,name,interface);
        Fingerprint f = Fingerprint(hash,ja3st,category,description);
        r->addFingerprint(f);
        rep.push_back(*r);
    }


}
