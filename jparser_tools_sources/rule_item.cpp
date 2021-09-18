#include "rule_item.h"
#include <string>

using namespace std;

string RuleItem::getDstIP()
{
    return dst_ip;
}

void RuleItem::setDstIP(string s)
{
    dst_ip = s;
}

string RuleItem::getSrcIP()
{
    return src_ip;
}

void RuleItem::setSrcIP(string s)
{
    src_ip = s;
}

string RuleItem::getJa3_hash()
{
    return ja3_hash;
}

void RuleItem::setJa3_hash(string s)
{
    ja3_hash = s;
}

string RuleItem::getJa3_string()
{
    return ja3_string;
}

void RuleItem::setJa3_string(string s)
{
    ja3_string = s;
}

string RuleItem::getCategory()
{
    return category;
}

void RuleItem::setCategory(string s)
{
    category = s;
}
string RuleItem::getItemHash()
{
    return ja3_hash + dst_ip + src_ip;
}

string RuleItem::get_stamp()
{

    return (ja3_hash+src_ip);
}
