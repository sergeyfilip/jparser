/**
 * @file config.h
 * @author S.Filipovich
 * @date 07 Nov 2019
 * @brief File containing singleton class  - to read and store AWL settings
 *
 * Class Config is used to store data about AWL settings
 *
 *
 */

#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include <list>
#include <iostream>
#include <string>
#include <stdio.h>
#include <iomanip>
#include <cstdlib>
#include <libconfig.h++>


using namespace std;
using namespace libconfig;

class Configuration {
public:

    static Configuration* getInstance() {
        static Configuration instance;
        return &instance;
    }


    int read_configuration();
    const char * get_title();
    const char * get_suricata_version();
    const char * get_version();
    const char * get_date();
    const char * get_fingerprints_file();
    const char * get_count_file();
    const char * get_policy_file();
    const char * get_groups_file();
    const char * get_config_file();
    const char * get_nicknames_file();
    const char * get_macs_file();
    const char * get_devices_file();
    const char * get_update_url();
private:

    Configuration() {
      title = "";
      version = "";
      suricata_version = "";
      date = "";
      fingerprints_file = "";
      count_file = "";
      policy_file = "";
      groups_file = "";
      config_file = "";
      nicknames_file = "";
      macs_file = "";
      devices_file = "";
      update_url = "";
    }
    std::string title;
    std::string version;
    std::string suricata_version;
    std::string date;
    std::string fingerprints_file;
    std::string count_file;
    std::string policy_file;
    std::string groups_file;
    std::string config_file;
    std::string nicknames_file;
    std::string macs_file;
    std::string devices_file;
    std::string update_url;
};


#endif /* CONFIG_H_ */
