#include <iostream>
#include <fstream>
#include <string>
#include <syslog.h>
#include <vector>
#include <stdio.h>
#include <list>
#include <string.h>
#include <sys/types.h>
#include "helper.h"
#include <stddef.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <grp.h>

using namespace std;



const char * template1 = "/suricata.d/network.yaml";
const char * template2 = "    include: /var/lib/jparser/awl.yaml";

int main(int argc, char * argv[])
{
    
    setlogmask (LOG_UPTO (LOG_NOTICE));
    
    openlog ("AWL jinteger start", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
    syslog (LOG_NOTICE, "jinteger started by User %d", getuid ());
    
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << "  action i or u" << std::endl;
        return 0;
    }
    string option = argv[1];
    if((option != "i" ) && (option != "u")) {
        
        std::cerr << "jinteger wrong arguments" << std::endl;
        return 0;
    }
    if(option == "i") {
        
        if(check_template(CONFIG_YAML_FILE,"awl.yaml") > 0) {
           syslog(LOG_NOTICE,"awl record already exists");
           
        }
        else {
            
            int d = check_template(CONFIG_YAML_FILE, template1);
            if(d > 0) modify_file(CONFIG_YAML_FILE, d+1,template2 );
            else syslog(LOG_NOTICE,"no map file");
            
        }
        ////////////////////white list ////////////////////
        int r = check_template(RULES_LIST_FILE,"white_list.rules");
        cout << r << endl;
        if( r > 0) {
           delete_from_file(RULES_LIST_FILE,r);
           int z = check_template(RULES_LIST_FILE, "worm.rules");
            if(z > 0) modify_file(RULES_LIST_FILE, z+1, " - white_list.rules");
           return 1;
        }
        else {
            
            int t = check_template(RULES_LIST_FILE, "worm.rules");
            if(t > 0) modify_file(RULES_LIST_FILE, t+1, " - white_list.rules");
            
            
        }
        
        
    }
    /////////////////////// delete /////////////////////////
    if(option == "u") {
        
        int z = check_template(CONFIG_YAML_FILE,"awl.yaml");
        
        if(z > 0)  {
           delete_from_file(CONFIG_YAML_FILE,z);
           
        } 
        int p = check_template(RULES_LIST_FILE,"white_list.rules");
        if(p > 0) {
           delete_from_file(RULES_LIST_FILE,p);
           
        }  
        
    }
syslog(LOG_NOTICE,"jinteger finished");
return 1;
}
