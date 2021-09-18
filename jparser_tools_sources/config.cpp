#include "config.h"

int Configuration::read_configuration() {
    
    
   Config cfg;

  
  try
  {
    cfg.readFile("/etc/jparser/conf.cfg");
  }
  catch(const FileIOException &fioex)
  {
    std::cerr << "I/O error while reading config file." << std::endl;
    return(EXIT_FAILURE);
  }
  catch(const ParseException &pex)
  {
    std::cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
              << " - " << pex.getError() << std::endl;
    return(EXIT_FAILURE);
  }

  
  try
  {
    cfg.lookupValue("suricata_version",suricata_version);
    
  }
  catch(const SettingNotFoundException &nfex)
  {
    cerr << "No 'version' setting in configuration file." << endl;
  }

  const Setting& root = cfg.getRoot();

  
  //cout << "Start search: "  << endl << endl;
  try
  {
    const Setting &main = root["application"]["main"];
    
    
          main.lookupValue("title", title);
          main.lookupValue("version", version);
          main.lookupValue("date", date);
  
  }
  catch(const SettingNotFoundException &nfex)
  {
    // Ignore.
  }
////////////////////////////////
try
  {
    const Setting &settings = root["application"]["settings"];
    settings.lookupValue("fingerprints_file", fingerprints_file);
    settings.lookupValue("count_file", count_file);
    settings.lookupValue("policy_file", policy_file);
    settings.lookupValue("groups_file", groups_file);
    settings.lookupValue("config_file", config_file);
    settings.lookupValue("nicknames_file", nicknames_file);
    settings.lookupValue("macs_file", macs_file);
    settings.lookupValue("devices_file", devices_file);
        
  }
  catch(const SettingNotFoundException &nfex)
  {
    // Ignore.
  } 
 return(EXIT_SUCCESS);   
}
const char * Configuration::get_title() {
    
     return title.c_str();  
}
    
const char * Configuration::get_version() { 
    
    return version.c_str();
}

const char * Configuration::get_suricata_version() { 
    
    return suricata_version.c_str();
}
const char * Configuration::get_date() { 
 
    return date.c_str();
    
}
const char * Configuration::get_fingerprints_file() {
    
    return fingerprints_file.c_str(); 
}
const char * Configuration::get_count_file() { 
    
    return count_file.c_str();
}
const char * Configuration::get_policy_file() {
    
    return policy_file.c_str();
}
const char * Configuration::get_groups_file() {
    
    return groups_file.c_str();
}
const char * Configuration::get_config_file() {
    
    return config_file.c_str();
}
const char * Configuration::get_nicknames_file() {
    
   return nicknames_file.c_str(); 
}
const char * Configuration::get_macs_file() {
    
    return macs_file.c_str();
}
const char * Configuration::get_devices_file() {
    
    return devices_file.c_str();
}
