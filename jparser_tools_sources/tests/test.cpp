#include <cstdlib>
#include <iostream>
#include <fstream>
#include <jsoncpp/json/json.h> // or something
#include <map>
#include <curl/curl.h>
#include <string>
#include <list>
#include <syslog.h>
#include <unistd.h>
#include <sys/types.h>
#include <iterator>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <grp.h>

using namespace std;

struct Umap {
  
  //std::string operator()(std::string s) { return s;}
  int operator()(int i) {  return 2*i;}
  string operator()(std::string str) 
    {
        
    for(auto& x: str)
        x = toupper(x);
    return str;
    }
  
};

int main(int argc, char * argv[])
{

    Umap up;
    
    string s = up("sssss");
    cout << s.c_str() << endl;
    int d = up(2);
    cout << "int = " << d << endl;
    
    std::map<std::string, std::vector<int>> map;
    std::vector<int> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    map["one"] = v;
    map["two"].push_back(7);
    map["one"].push_back(11);
    for(const auto &kvp: map) 
    {
    std::cout << kvp.first << std::endl;
    for(auto v: kvp.second)
      std::cout << v << std::endl;
    }
    cout << "------------------------------" << endl;
auto rr = map.find("two");
 if (rr != map.end()) {
   std::cout << rr->first << std::endl;
   for(auto vv: rr->second)
      std::cout << vv << std::endl;
    
 }
    return 0;
}

