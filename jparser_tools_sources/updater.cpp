/**
 * @file updater.cpp
 * @author Sergey Filipovich
 * @date 9 Sep 2018
 * @brief File containing code for janalyzer utility
 *
 * Janalyser takes data from elasticsearch about all tls packages,
 * generates files for devices and all connections
 *
 */

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
#include "config.h"
#include "item.h"
using namespace std;
//function - search for Item with needed fingerprint
Item* find_fingerprint(list<Item> l, string s)
{

    list<Item>::iterator it;

    for(it = l.begin(); it != l.end(); ++it) {

        if (s.compare((*it).getJa3_hash()) == 0) {

         return &(*it);

        }

    }

    return nullptr;

}

bool compare_fingerprints(list<Item> l1, list<Item> l2) {
    if(l1.size() != l2.size()) return false;
    list<Item>::iterator it;

    for(it = l2.begin(); it != l2.end(); ++it) {
      string z = it->getJa3_hash();
        if(!find_fingerprint(l1, z)) return false;

    }
    return true;
}

int main(int argc, char * argv[])
{

    Json::Reader reader;
    Json::Value obj;
    std::ofstream wfile;
    list <Item> items, old_items;
    FILE* bodyfile;
    //set log mask and start logging
    setlogmask (LOG_UPTO (LOG_NOTICE));

    openlog ("jupdater", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
    syslog (LOG_NOTICE, "Jupdater started by User %d", getuid ());
    //Check for path argument
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " url" << std::endl;
        return 1;
    }
    string url_path_to_files = argv[1];

    if (!url_path_to_files.empty() && url_path_to_files.back() != '/') //detecting trailing slash
        url_path_to_files += '/';
    syslog (LOG_NOTICE, "Program started elastic index");

    if(Configuration::getInstance()->read_configuration() != EXIT_SUCCESS) {
     std::cerr << "Error reading config file" << std::endl;
        return 1;

    }

    const char * fingerprints_file = Configuration::getInstance()->get_fingerprints_file();
    //Preparing CURL to ask elastic
    CURL *curl;

    CURLcode res;

    syslog (LOG_NOTICE, "Try to init curl");
    curl = curl_easy_init();
    syslog (LOG_NOTICE, "Curl is ready");


    if (curl) {


        string r1 = "https://updates.staging.dev.redpiranha.net/userver/ja3.tar.gz";

        string s1 = r1;
        syslog (LOG_NOTICE, "Openning json fingerprints");
        bodyfile = fopen("/home/fil/ja3.tar.gz", "wb"); //temporary file

        syslog (LOG_NOTICE, "Body opened");
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0); // this line
        curl_easy_setopt (curl, CURLOPT_SSL_VERIFYPEER, 0); // and this line fixed the issue
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
        curl_easy_setopt(curl, CURLOPT_URL, s1.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
        curl_easy_setopt(curl,CURLOPT_WRITEDATA,bodyfile);
    //    curl_easy_setopt(curl, CURLOPT_USERNAME, "sergey");
    //    curl_easy_setopt(curl, CURLOPT_PASSWORD, "fsn1955_pvi");
    //    curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl,CURLOPT_FAILONERROR,0L);

        syslog (LOG_NOTICE, "Option is ready");

      //  curl_easy_setopt(curl, CURLOPT_HEADER, 1);
        syslog (LOG_NOTICE, "Options were set");
        res = curl_easy_perform(curl);
        syslog (LOG_NOTICE, "Curl easy performed");
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));
            syslog (LOG_NOTICE, "Jupdater failed with request.");
            closelog ();
            return 1;
        }
        curl_easy_cleanup(curl);
        syslog (LOG_NOTICE, "Curl cleaned");
        fclose(bodyfile);

    }
    system("tar -zxvf /home/fil/ja3.tar.gz");
    ifstream ifs2(fingerprints_file);
    std::string name;
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
   ifs2.close();

   ifstream ifs3(fingerprints_file);

       while(std::getline (ifs3,name)) {

           reader.parse(name, obj);
           const Json::Value& st = obj["desc"];
           const Json::Value& st1 = obj["Ja3_hash"];
           const Json::Value& st2 = obj["category"];
           if(st1.asString().length() < 40) {

               Item * iz = new Item(st.asString(),st1.asString(),"", st2.asString());

               old_items.push_back(*iz);
           }
       }
  ifs3.close();

 cout << "all OK!" << endl  << endl;
 if (!compare_fingerprints(items, old_items)) cout << "fingerprints are different" << endl;
 //for (auto ii =items.begin(); ii != items.end(); ii++) {
//     cout << ii->getJa3_hash() << endl;
 //}
 syslog (LOG_INFO, "Jupdeter: finished");
    closelog ();
  }
