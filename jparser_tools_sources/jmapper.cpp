/**
 * @file jmapper.cpp
 * @author Sergey Filipovich
 * @date 9 Jan 2019
 * @brief File containing code for program which generates list of devices from files in /var/crystaleye/network_map
 *
 * Jmapper takes data from /var/crystaleye/network_map,
 * generates files for devices and all connections
 *
 */


#include <cstdlib>
#include <iostream>
#include <fstream>
#include <jsoncpp/json/json.h> // or something
#include <string>
#include <list>
#include <syslog.h>
#include <unistd.h>
#include <sys/types.h>
#include <iterator>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <grp.h>
#include "device.h"
#include "devices.h"
#include "config.h"


using namespace std;

int main(int argc, char * argv[])
{


    Json::Reader reader;
    Json::Value obj;
    std::ofstream wfile;
    Devices devs, macs;

    setlogmask (LOG_UPTO (LOG_NOTICE));

    openlog ("jmapper", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1); //opening log
    syslog (LOG_NOTICE, "Jmapper started by User %d", getuid ());
    ///Check for path argument
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " DESTINATION_FOR_DATA_FILES" << std::endl;
        return 1;
    }
    if(Configuration::getInstance()->read_configuration() != EXIT_SUCCESS) {
     std::cerr << "Error reading config file" << std::endl;
        return 1;   
        
    }
    
    
    const char * nicknames_file = Configuration::getInstance()->get_nicknames_file();
    const char * macs_file = Configuration::getInstance()->get_macs_file();
    

    string path_to_files = argv[1];

    if (!path_to_files.empty() && path_to_files.back() != '/') //detecting trailing slash
        path_to_files += '/';

    syslog (LOG_NOTICE, "Starting work");

    try {
        ifstream ifs1(nicknames_file);
        ///parsing json
        reader.parse(ifs1, obj); // reader can also read strings
        for(Json::Value::iterator it=obj.begin(); it!=obj.end(); ++it) {
            cout << it.key().asString() << ':' << it->asString() << '\n';
            Device d = Device(it.key().asString(),it->asString());
            devs.add_device(d);

        }
        for(auto ii = devs.get_list()->begin(); ii!=devs.get_list()->end(); ii++) cout << ii->getName() << ": " << ii->getIp() << endl;

        ifstream ifs2(macs_file);
        ///parsing json
        reader.parse(ifs2, obj); // reader can also read strings
        for(Json::Value::iterator it=obj.begin(); it!=obj.end(); ++it) {
            cout << it.key().asString() << ':' << it->asString() << '\n';
            Device d = Device(it.key().asString()," ",it->asString());
            macs.add_device(d);

        }


        for(auto ii = macs.get_list()->begin(); ii!=macs.get_list()->end(); ii++) {

            Device* b = devs.find_device_by_ip(ii->getIp());
            if(b)  ii->setName(b->getName());
        }

        for(auto ii = macs.get_list()->begin(); ii!=macs.get_list()->end(); ii++) cout << ii->getNetwork_interface() << ": " << ii->getIp() << " : " << ii->getName() << endl;

        string z = path_to_files+"devices.txt";
        wfile.open(z.c_str(),std::ofstream::out );



        for(auto ii = macs.get_list()->begin(); ii!=macs.get_list()->end(); ii++) {

            char str[300];
            sprintf(str,"%s;%s;%s", ii->getNetwork_interface().c_str(),ii->getIp().c_str(), ii->getName().c_str());


            wfile << str << endl;

        }

        wfile.close();

        z = path_to_files+"devices_ip.txt";
        wfile.open(z.c_str(),std::ofstream::out );



        for(auto ii = macs.get_list()->begin(); ii!=macs.get_list()->end(); ii++) {


            char str[300];
            sprintf(str,"%s", ii->getIp().c_str());

            wfile << str << endl;

        }

        wfile.close();
        // for (auto v = white_list->begin(); v != white_list->end(); v++) cout << "11" << endl;
    } catch (const std::exception& e) { // caught by reference to base
        std::cout << " a standard exception was caught, with message '"
                  << e.what() << "'\n";
        syslog (LOG_NOTICE, "JA_analyzer failed parsing.");
        closelog ();
        return 2;
    }
    syslog (LOG_NOTICE, "JA_mapper finished. Success!");

    closelog ();

    // get_days();
    return 0;
}

