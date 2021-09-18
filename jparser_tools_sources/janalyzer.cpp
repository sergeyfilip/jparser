/**
 * @file janalyzer.cpp
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
#include "item.h"
#include "rule_item.h"
#include "config.h"

using namespace std;



/** @brief Search category by template, defined by string s
 *
 *  Subsequent check all items in list and return full name for category,
 *  or Unknown Software
 *
 *  @param l list of items
 *  @param s string template
 *  @return string
 */
string findCat(list<Item> l, string s)
{

    list<Item>::iterator it;

    for(it = l.begin(); it != l.end(); ++it) {

        std::size_t found = s.find((*it).getJa3_hash());
        if (found!=std::string::npos) {
            return (*it).getCategory();
        }

    }

    return "Unknown Software";

}

/** @brief compare two rule items and return true if equal
 *
 *
 *  @param first item to compare
 *  @param second item to compare
 *  @return bool
 */
bool same_rule_item (RuleItem first, RuleItem second)
{
    string s1 = first.getItemHash();
    string s2 = second.getItemHash();
    if (s1.compare(s2) == 0) return true;

    return false;
}

/** @brief compare two rule items and return true if first "less" then second
 *
 *
 *  @param f item to compare
 *  @param s item to compare
 *  @return bool
 */
bool compare_nocase (RuleItem f, RuleItem s)
{
    unsigned int i=0;
    string first = f.getItemHash();
    string second = s.getItemHash();
    while ( (i<first.length()) && (i<second.length()) ) {
        if (tolower(first[i])<tolower(second[i])) return true;
        else if (tolower(first[i])>tolower(second[i])) return false;
        ++i;
    }
    return ( first.length() < second.length() );
}

/** @brief set elasticsearch max result index to defined integer
 *
 *  Send special curl request to elasticsearch,
 *  mostly to increase it
 *
 *  @param n integer value of index
 *  @return CURLcode
 *  @see https://curl.haxx.se/libcurl/c/libcurl-errors.html
 *  @see https://www.elastic.co/guide/en/elasticsearch/reference/current/search-request-from-size.html
 */
CURLcode set_index(int n)
{
//  Preparing CURL to set elastic
    CURL *curl;
    std::string s2 = std::to_string(n);
    std::string s1 = "{  \"index.max_result_window\" : \"" + s2 + "\" } ";
    // const char *postFields = "{  \"index.max_result_window\" : \"" + s + "\" } ";
    const char *postFields = s1.c_str();
    CURLcode res = CURLE_FAILED_INIT;


    curl = curl_easy_init();


    if (curl) {

        string r1 = "http://localhost:9200/_settings  ";

        string s1 = r1;

        curl_easy_setopt(curl, CURLOPT_URL, s1.c_str());
        curl_easy_setopt(curl,CURLOPT_POSTFIELDS,postFields);
        /* HTTP PUT please */
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");


        // curl_easy_setopt(curl, CURLOPT_HEADER, 1L);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "elastic index setting failed: %s\n",
                    curl_easy_strerror(res));
            setlogmask (LOG_UPTO (LOG_NOTICE));

            openlog ("ja_parser", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
            syslog (LOG_NOTICE, "index setting failed with elasticsearch.");
            closelog ();
        }
        curl_easy_cleanup(curl);
        return res;

    }
    return res;
}

/** @brief get number of days from OS start
 *
 *  @return int.
 */
int get_days()
{
    struct sysinfo sys_info;

    int days;


    if(sysinfo(&sys_info) != 0)
        perror("sysinfo");

    days = sys_info.uptime ;// 86400;
    printf("days %d\n",days);
    return days;


}
int main(int argc, char * argv[])
{


    list <Item> items;
    Json::Reader reader;
    Json::Value obj;
    std::ofstream wfile;

    //set log mask and start logging
    setlogmask (LOG_UPTO (LOG_NOTICE));

    openlog ("janalyzer", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
    syslog (LOG_NOTICE, "Janalyzer started by User %d", getuid ());
    //Check for path argument
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " DESTINATION_FOR_DATA_FILES" << std::endl;
        return 1;
    }
    string path_to_files = argv[1];

    if (!path_to_files.empty() && path_to_files.back() != '/') //detecting trailing slash
        path_to_files += '/';
    syslog (LOG_NOTICE, "Program started elastic index");
    set_index(80000);
    syslog (LOG_NOTICE, "Index is set");
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
        FILE* bodyfile;
        const char *postFields = "{  \"size\" : 70000,  \"query\": {    \"match\": {  \"event_type\": \"tls\"  } }} ";
        string r1 = "http://localhost:9200/_search?pretty&filter_path=took,hits.total,hits.hits._source.dest_ip,hits.hits._source.src_ip,hits.hits._source.tls.fingerprint,hits.hits._source.tls.ja3  ";

        string s1 = r1;
        syslog (LOG_NOTICE, "Openning body");
        bodyfile = fopen("/tmp/body.txt", "w"); //temporary file

        syslog (LOG_NOTICE, "Body opened");
        curl_easy_setopt(curl, CURLOPT_URL, s1.c_str());
        curl_easy_setopt(curl,CURLOPT_POSTFIELDS,postFields);
        curl_easy_setopt(curl,CURLOPT_WRITEDATA,bodyfile);
        syslog (LOG_NOTICE, "Option is ready");

        curl_easy_setopt(curl, CURLOPT_HEADER, 1);
        syslog (LOG_NOTICE, "Options were set");
        res = curl_easy_perform(curl);
        syslog (LOG_NOTICE, "Curl easy performed");
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));
            syslog (LOG_NOTICE, "JAnalyzer failed with elasticsearch.");
            closelog ();
            return 1;
        }
        curl_easy_cleanup(curl);
        syslog (LOG_NOTICE, "Curl cleaned");
        fclose(bodyfile);

    }

    //results now saved to bodyfile

    syslog (LOG_NOTICE, "Results saved to body");
    //filtering not json strings to another file - body1.txt
    //this operation is needed becouse of test strings in json responser. we must filter them
    string line;
    ifstream ifs("/tmp/body.txt");
    std::ofstream out("/tmp/body1.txt");
    int k=0;
    while (! ifs.eof() ) {
        getline (ifs,line);
        if ((line.length()==1) && (line == "{")) k = 1;
        if (k)  out << line;
    }
    out.close();
    ifs.close();
    syslog (LOG_NOTICE, "Before last block");
    /////// last block //////
    try {
        ifstream ifs1("/tmp/body1.txt");
        //now it is time to parse body1.txt
        //parsing json
        reader.parse(ifs1, obj); // reader can also read strings
        const Json::Value& chards = obj["hits"]; // array of characters
        const Json::Value& st = chards["hits"]; // array of characters
        std::list<RuleItem>* white_list = new  list<RuleItem>;
        std::list<string>* src = new list<string>;


        //////////////////Get files
        Json::Reader reader;
        Json::Value obj;
        std::string name;


        cout << "start" << endl;
        ifstream ifs2(fingerprints_file);
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

        for (unsigned int i =0 ; i < st.size() ; i++) {
            const Json::Value& tt = st[i]["_source"];
            const Json::Value& ttt  = tt["tls"]["ja3"];
            RuleItem * ir = new RuleItem(tt["dest_ip"].asString(),tt["src_ip"].asString(),ttt["hash"].asString(),ttt["hash"].asString(),findCat(items,ttt["hash"].asString()));
            white_list->push_back(*ir);
            src->push_back(tt["src_ip"].asString());
            delete ir;

        }
        cout << "end list" << endl;

        white_list->sort(compare_nocase);
        white_list->unique(same_rule_item );
        src->sort();
        src->unique();
        ///////////////////////////////
        std::string z = path_to_files+"white_list_connections.txt";
        wfile.open(z.c_str(),std::ofstream::out );



        for (auto ii = white_list->begin(); ii != white_list->end(); ii++) {
            string t = (*ii).getJa3_hash();
            string t1 = (*ii).getDstIP();
            string t2 = (*ii).getSrcIP();
            string tz = (*ii).getCategory();
            //cout <<  << "  " <<  << "   " <<  << (*ii).getCategory() << endl;
            char str[300];
            sprintf(str,"src_ip -> %s ; dest_ip -> %s ; hash -> %s ; def -> %s", t2.c_str(),t1.c_str(),t.c_str(), tz.c_str());

            wfile << str << endl;

        }

        wfile.close();
        int count = 0;
        z = path_to_files+"devices.txt";
        wfile.open(z.c_str(),std::ofstream::out );



        for (auto ii = src->begin(); ii != src->end(); ii++) {
            string t = (*ii);

            //cout <<  << "  " <<  << "   " <<  << (*ii).getCategory() << endl;
            char str[300];
            sprintf(str,"%s;%s;Device %d", "Unknown",t.c_str(), count);
            count++;

            wfile << str << endl;

        }

        wfile.close();

        z = path_to_files+"devices_ip.txt";
        wfile.open(z.c_str(),std::ofstream::out );



        for (auto ii = src->begin(); ii != src->end(); ii++) {
            string t = (*ii);

            char str[300];
            sprintf(str,"%s", t.c_str());

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
    syslog (LOG_NOTICE, "JA_analyzer finished. Success!");
    if( remove( "/tmp/body.txt" ) == 0 )
        syslog (LOG_INFO, "JA_analyzer: temporary file body deleted");
    if( remove( "/tmp/body1.txt" ) == 0 )
        syslog (LOG_INFO, "JA_analyzer: temporary file body1 deleted");
    closelog ();
    set_index(10000);
    // get_days();
    return 0;
}

