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
#include <stddef.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <grp.h>
#include "config.h"
#include <cstring>

using namespace std;




struct rule_line {
    string ip;
    string fingerprint;
    string software;


    rule_line(string& s1, string& s2, string& s3):ip(s1),fingerprint(s2),software(s3) {}

};
/////////////////////////////////////////////
long get_sid()
{

    char buff[50];
    if(Configuration::getInstance()->read_configuration() != EXIT_SUCCESS) {
     std::cerr << "Error reading config file" << std::endl;
        return EXIT_FAILURE;   
        
    }
    
  //  const char * fingerprints_file = Configuration::getInstance()->get_fingerprints_file().c_str();
    const char * count_file = Configuration::getInstance()->get_count_file();
    ifstream fin(count_file);

    if (!fin.is_open()) {
        ofstream fout(count_file);
        fout << "1100000";
        fout.close();
        return 1100000;
    } else {
        fin >> buff;
        return atoi(buff);
        cout << buff << endl;
    }
return 1100000;
}

void set_sid(long s)
{

    if(Configuration::getInstance()->read_configuration() != EXIT_SUCCESS) {
     std::cerr << "Error reading config file" << std::endl;
        return;   
        
    }
    
  //  const char * fingerprints_file = Configuration::getInstance()->get_fingerprints_file().c_str();
    const char * count_file = Configuration::getInstance()->get_count_file();
    ofstream fout(count_file);
    fout << s << endl;
    fout.close();
    return;
}

int main(int argc, char * argv[])
{

    setlogmask (LOG_UPTO (LOG_NOTICE));
//openning system log
    openlog ("jrules", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
    syslog (LOG_NOTICE, "Jrules started by User %d", getuid ());
//Check for path argument
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << " PATH_TO_DATA PATH_TO_DESTINATION FILE_WITH_DEVICES_LIST " << std::endl;
        return 1;
    }
    
    
    string path_to_data_files = argv[1];
    string path_to_rules= argv[2];
    string dev_file = argv[3];
    list<string> devices;
    std::ofstream wfile;
    //////////////////////////////
    ifstream fd(dev_file);
    string line;

    while (! fd.eof() ) {   //read and parse all devices
        getline (fd,line);
        if ((line.length() > 1)) {

            devices.push_back(line);


        }

    }
    fd.close();
    //////////////////////////////
    long g = 1100000;
    set_sid(g);
    if (!path_to_data_files.empty() && path_to_data_files.back() != '/') //detecting trailing slash
        path_to_data_files += '/';

    if (!path_to_rules.empty() && path_to_rules.back() != '/') //detecting trailing slash
        path_to_rules += '/';

    string data_file = path_to_data_files+"white_list_data.txt";
    syslog(LOG_NOTICE,"Getting data");

    //////////////////////////// DEVICES
    ifstream ifd(data_file);
    string pline;
    list<rule_line> rules;
    while (! ifd.eof() ) {   //read and parse all devices: network_interface, ip, name
        getline (ifd,pline);
        if ((pline.length() > 1)) {
            const char* sstring = pline.c_str();
            char ss[300];
            strcpy(ss,sstring);
            const char delimiters[] = ";";
            char *token;
            token = strtok (ss, delimiters);
            string s1(token);
            token = strtok (NULL, delimiters);
            string s2(token);
            token = strtok (NULL, delimiters);
            string s3(token);
            rule_line rl = rule_line(s1,s2,s3);
            rules.push_back(rl);


        }

    }

    ifd.close();
    //  int u = 1100000;
    long u = get_sid();
    std::string z = path_to_rules+"white_list.rules";
    wfile.open(z.c_str(),std::ofstream::out );
    for(auto it = devices.begin(); it != devices.end(); it++) {

            for(auto ii = rules.begin(); ii != rules.end(); ii++) {
                if((*it) == (*ii).ip) {
                    char str[300];
                    if(((*ii).software == "Malware ") || ((*ii).software == "Unknown Software"))
                        sprintf(str,"reject tls %s any -> any any (msg:\"reject match whitelisting hash malware or unknown hash %s\"; ja3_hash; content:\"%s\"; sid:%ld;rev:11;)", ((*ii).ip).c_str(), ((*ii).software).c_str(),((*ii).fingerprint).c_str(), u);
                    else
                        sprintf(str,"pass tls %s any -> any any (msg:\"pass match whitelisting hash %s\"; ja3_hash; content:\"%s\"; sid:%ld;rev:11;)", ((*ii).ip).c_str(), ((*ii).software).c_str(),((*ii).fingerprint).c_str(), u);
                    wfile << str << endl;
                    u++;
                }

            }
            char str2[300];
            sprintf(str2,"reject tls %s any -> any any (msg:\"not match any  whitelisting hash\"; sid:%ld;rev:11;)", (*it).c_str(), u);
            wfile << str2 << endl;





    u++;

    }
    char str2[300];

        sprintf(str2,"reject tls any any -> any any (msg:\"not match any  whitelisting hash\"; sid:%ld;rev:11;)", u);
        wfile << str2 << endl;

    wfile.close();

    syslog (LOG_NOTICE, "Jrules finished. Success!");
    set_sid(u);
    closelog ();

    return 0;
}

