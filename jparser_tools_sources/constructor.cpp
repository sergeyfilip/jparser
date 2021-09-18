#include <map>
#include <curl/curl.h>
#include <string>
#include <list>
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
#include "policy.h"
#include "policies.h"
#include "group.h"
#include "groups.h"
#include "device.h"
#include "devices.h"
#include "reportitem.h"
#include <iterator>

#include <cstring>

#include "helper.h"
#include "config.h"

using namespace std;

int main(int argc, char * argv[])
{
    
    std::list<string> fingerprints;
    Json::Reader reader;
    Json::Value obj;
    std::ofstream wfile;
    
    setlogmask (LOG_UPTO (LOG_NOTICE));
//openning system log
    openlog ("constructor", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
    syslog (LOG_NOTICE, "constructor started by User %d", getuid ());
//Check for path argument
    
    if(Configuration::getInstance()->read_configuration() != EXIT_SUCCESS) {
     std::cerr << "Error reading config file" << std::endl;
        return 1;   
        
    }
    
    const char * fingerprints_file = Configuration::getInstance()->get_fingerprints_file();

    
    cout << fingerprints_file << endl;
    
    cout << "OOOOO" << endl;
////////////Parse fingerprints file and load fingerprints in list //////////////   
    syslog(LOG_NOTICE,"Getting fingerprints data");
    ifstream ifs2(fingerprints_file);
    std::string name;
        while(std::getline (ifs2,name)) {

            reader.parse(name, obj);
      //      const Json::Value& st = obj["desc"];
      //      const Json::Value& st1 = obj["Ja3_hash"];
            const Json::Value& st2 = obj["category"];
       //     string s = "";
	 //   string h = st1.asString();
	    string c = st2.asString();
	 //   string d = st.asString();
	 //   Fingerprint f(h,s,c,d);
	    fingerprints.push_back(c);
	    
        }
        fingerprints.sort();
        fingerprints.unique();
        int b = 1;
    for(auto i : fingerprints) { 
        cout << b << "," << i.c_str() << "," << i.c_str() << endl;
        b++;
    }
    
}
