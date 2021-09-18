#include <iostream>
#include <fstream>
#include <string>
#include <syslog.h>
#include <vector>
#include <stdio.h>
#include <list>
#include <string.h>
#include <sys/types.h>
#include "item.h"
#include <stddef.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <grp.h>
#include <jsoncpp/json/json.h> // or something
#include "config.h"
#include "helper.h"
#include "settings.h"


using namespace std;



int main(int argc, char * argv[])
{
    
    list <Item> items;
    Json::Reader reader;
    Json::Value obj;
    
    setlogmask (LOG_UPTO (LOG_NOTICE));
    
    openlog ("AWL jmaker start", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
    syslog (LOG_NOTICE, "jmaker started by User %d", getuid ());
    
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << "  action (p or r), fingerprint, ip" << std::endl;
        return 0;
    }
    string option = argv[1];
    if((option != "p" ) && (option != "r")) {
        
        std::cerr << "jmaker wrong arguments" << std::endl;
        return 0;
    }
    if(Configuration::getInstance()->read_configuration() != EXIT_SUCCESS) {
     std::cerr << "Error reading config file" << std::endl;
        return 0;   
        
    }
    
    const char * fingerprints_file = Configuration::getInstance()->get_fingerprints_file();
    string fingerprint = argv[2];
    string ip = argv[3];
    check();
   // 
    
        
        ifstream ifs2(fingerprints_file);
        string name;
        while(std::getline (ifs2,name)) {

            reader.parse(name, obj);
            const Json::Value& st = obj["desc"];
            const Json::Value& st1 = obj["Ja3_hash"];
            const Json::Value& st2 = obj["category"];
            if(st1.asString().length() < 40) {

                Item * iz = new Item(st.asString(),st1.asString(),"", st2.asString());

                items.push_back(*iz);
            }
        }
    string category = findCategory(items, fingerprint);
    char strtemp[300];
    sprintf(strtemp,"tls %s any -> any any (msg:\"match whitelisting known hash category %s\"; ja3_hash; content:\"%s\";", ip.c_str(), category.c_str(), fingerprint.c_str());
    int s = check_template(RULES_FILE,strtemp);
    if(s > 0) {
        
      syslog(LOG_NOTICE,"jmaker finished, these fingerprint and device already in rules.");  
    return 2;
        
    }
    long int u = get_sid();
    u++;
    if(option == "p") {
         char str[300];
         sprintf(str,"pass tls %s any -> any any (msg:\"match whitelisting known hash category %s\"; ja3_hash; content:\"%s\"; sid:%ld;rev:11;)", ip.c_str(), category.c_str(), fingerprint.c_str(), u);
    set_sid(u); 
    cout << str << endl;
     modify_file(RULES_FILE, 3, str);   
        
    }
    /////////////////////// delete /////////////////////////
    if(option == "r") {
     char str[300];
         sprintf(str,"reject tls %s any -> any any (msg:\"match whitelisting known hash category %s\"; ja3_hash; content:\"%s\"; sid:%ld;rev:11;)", ip.c_str(), category.c_str(), fingerprint.c_str(), u);
    set_sid(u); 
    cout << str << endl;
     modify_file(RULES_FILE, 3, str);      
        
        
     }
   syslog(LOG_NOTICE,"jmaker finished with success, fingerprint added.");
        return 1;
    

}
