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
#include <map>

#include <iterator>

#include <cstring>

#include "policy.h"
#include "policies.h"
#include "group.h"
#include "groups.h"
#include "device.h"
#include "devices.h"
#include "reportitem.h"
#include "helper.h"
#include "config.h"
#include "item.h"

using namespace std;


int main(int argc, char * argv[])
{
    Policies pl;
    Groups gr;
    std::list<Fingerprint> fingerprints;
    Json::Reader reader;
    Json::Value obj;
    std::ofstream wfile;
    set_sid(1100000);
    setlogmask (LOG_UPTO (LOG_NOTICE));
//openning system log
    openlog ("Jparser_rules_generator", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
    syslog (LOG_NOTICE, "Jparser_rules_generator started by User %d", getuid ());
//Check for path argument
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << "PATH_TO_RULES_DESTINATION_FOLDER" << std::endl;
        return 1;
    }
    if(Configuration::getInstance()->read_configuration() != EXIT_SUCCESS) {
     std::cerr << "Error reading config file" << std::endl;
        return 1;

    }

    const char * fingerprints_file = Configuration::getInstance()->get_fingerprints_file();

    const char * policy_file = Configuration::getInstance()->get_policy_file();
    const char * groups_file = Configuration::getInstance()->get_groups_file();
    const char * config_file = Configuration::getInstance()->get_config_file();
    
    string path_to_rules= argv[1];
    if (!path_to_rules.empty() && path_to_rules.back() != '/') //detecting trailing slash
        path_to_rules += '/';
////////////Parse fingerprints file and load fingerprints in list //////////////
    syslog(LOG_NOTICE,"Getting fingerprints data");

    ifstream ifs2(fingerprints_file);
    std::string name;
        while(std::getline (ifs2,name)) {

            reader.parse(name, obj);
            const Json::Value& st = obj["desc"];
            const Json::Value& st1 = obj["Ja3_hash"];
            const Json::Value& st2 = obj["category"];
            string s = "";
	    string h = st1.asString();
	    string c = st2.asString();
	    string d = st.asString();
	    Fingerprint f(h,s,c,d);
	    fingerprints.push_back(f);

        }


 /////////////////////////////////////////////////////////////////
 ////////////////// Developing policies //////////////////////////
 syslog(LOG_NOTICE,"Developing policies");
    ifstream ifs3(policy_file);
   
    reader.parse(ifs3, obj); // reader can also read strings
    const Json::Value& pol = obj["policies"]; // array of characters
    for(auto r: pol) {
    const Json::Value& st = r; // array of characters
    Policy* p = new Policy(st["name"].asString());

    for(auto z : st["categories"]) {
      p->add_category(z.asString());
    }
    pl.add_policy(*p);
    }
    Policylist* l = pl.get_list();
    for(auto m=l->begin(); m!= l->end();++m) {
      
      auto b = m->get_categories_list();
      for(auto u= b->begin(); u != b->end(); ++u) {
	
      }
    }
 ///////////////////////////////////////////////////////////
 //////////////////Developing groups //////////////////////
 syslog(LOG_NOTICE,"Developing groups");
      ifstream ifs1(groups_file);
        //now it is time to parse body1.txt
        //parsing json
    reader.parse(ifs1, obj); // reader can also read strings
    const Json::Value& groups = obj["groups"]; // array of characters

    for(auto st:groups) {
    string name_st = st["name"].asString();
    Policy* p1 = pl.find_policy_by_name(st["policy"].asString());
    Devices* dd = new Devices();
       for(auto c: st["devices"]) {
	      Device* d = new Device(c["ip"].asString(),c["name"].asString());
	      dd->add_device(*d);
       }
    Group* g = new Group(name_st,p1,dd);
    gr.add_group(*g);
    }

   /////////////////////////
    syslog(LOG_NOTICE,"Generating rules");
    
    long int u = get_sid();
    std::string z = path_to_rules+"white_list.rules";
    wfile.open(z.c_str(),std::ofstream::out );
    string cl = "#classification:security";
    string des = "#description:ja3 fingerprints";
    wfile << cl << endl << des << endl;
    for(auto zf:fingerprints) {

      string st = gr.get_Group_part_of_rule(zf.getCategory());
      
      if(st != "") {
      string net = gr.get_Net_settings();
      string j3 = zf.getJa3_hash();
      string cat = zf.getCategory();
      
      char str[300];
         sprintf(str,"pass tls [%s] any -> any any (msg:\"allow match whitelisting known hash category %s\"; ja3_hash; content:\"%s\"; sid:%ld;rev:11;)", st.c_str(), cat.c_str(), j3.c_str(), u);
       u++;
    
       wfile << str << endl;
      }

    }
    /////////////////////////////
    /* for(auto zr:fingerprints) {

      string st = gr.get_Group_part_of_rule(zr.getCategory());
      
      if(st == "") {
      string net = gr.get_Net_settings();
      string j3 = zr.getJa3_hash();
      string cat = zr.getCategory();
      
      char str[300];
         sprintf(str,"reject tls $AWL_HOME_NET any -> $EXTERNAL_NET any (msg:\"reject match whitelisting known hash category %s\"; ja3_hash; content:\"%s\"; sid:%ld;rev:11;)", cat.c_str(), j3.c_str(), u);
       u++;
    
       wfile << str << endl;
      }

    } */
    //////////////////////////////////
    char str2[300];
    sprintf(str2,"reject tls $AWL_HOME_NET any -> $EXTERNAL_NET any (msg:\"not match any  allowed whitelisting hash\"; sid:%ld;rev:11;)", u);
    wfile << str2 << endl;
    wfile.close();
    wfile.open(config_file,std::ofstream::out );
    wfile << "%YAML 1.1" << endl;
    wfile << "---" << endl;
    wfile << endl;

    wfile << gr.get_Net_settings() << endl;
  
    wfile.close();
set_sid(++u);
syslog (LOG_NOTICE, "Rules generator finished. Success!");
return 0;
}
