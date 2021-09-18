/**
 * @file jreport.cpp
 * @author Sergey Filipovich
 * @date 12 Sep 2018
 * @brief File containing code for program which generates jsom report
 *
 * Report - json file white_list_report.json which links devices with fingerprints
 * This file is used by other utilities to generate rules and also by GUI to show network map
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
#include <stddef.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <grp.h>
#include "item.h"
#include "rule_item.h"
#include "device.h"
#include "settings.h"
#include "devices.h"
#include "reportitem.h"
#include <cstring>
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
    return "Unknown";
}
/** @brief Search description by template, defined by string s
 *
 *
 *  @param l list of items
 *  @param s string template
 *  @return string
 */
string findDesc(list<Item> l, string s)
{

    list<Item>::iterator it;

    for(it = l.begin(); it != l.end(); ++it) {

        std::size_t found = s.find((*it).getJa3_hash());
        if (found!=std::string::npos) {
            return (*it).getDesc();
        }

    }

    return "Unknown";

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
    string s1 = first.get_stamp();
    string s2 = second.get_stamp();
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
    string first = f.get_stamp();
    string second = s.get_stamp();
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
    std::ofstream wfile, reportfile, testfile;

    Devices devs;

    setlogmask (LOG_UPTO (LOG_NOTICE));
    //openning system log
    openlog ("jreport", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
    syslog (LOG_NOTICE, "Jreport started by User %d", getuid ());
    //Check for path argument
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " DESTINATION  " << std::endl;
        return 1;
    }
    if(Configuration::getInstance()->read_configuration() != EXIT_SUCCESS) {
     std::cerr << "Error reading config file" << std::endl;
        return 1;   
        
    }
    
    const char * fingerprints_file = Configuration::getInstance()->get_fingerprints_file();
    const char * devices_file = Configuration::getInstance()->get_devices_file();
    string path_to_files = argv[1];

    if (!path_to_files.empty() && path_to_files.back() != '/') //detecting trailing slash
        path_to_files += '/';
    syslog (LOG_NOTICE, "Program started elastic index");
    set_index(80000);
    syslog (LOG_NOTICE, "Index is set");
    //Preparing CURL to ask elastic
    CURL *curl;

    CURLcode res;


    syslog(LOG_NOTICE,"Getting devices");

    //////////////////////////// DEVICES
    ifstream ifd(devices_file);
    string pline;

    while (! ifd.eof() ) {   //read and parse all devices: network_interface, ip, name
        getline (ifd,pline);
        if ((pline.length() > 1)) {
            const char* sstring = pline.c_str();
            char ss[200];
            strcpy(ss,sstring);
            const char delimiters[] = ",;";
            char *token;
            token = strtok (ss, delimiters);
            string s1(token);
            token = strtok (NULL, delimiters);
            string s2(token);
            token = strtok (NULL, delimiters);
            string s3(token);
            Device dev = Device(s2,s3,s1);
            devs.add_device(dev);

        }

    }

    ifd.close();


    /// //////////////////////////////////////

    syslog (LOG_NOTICE, "Try to init curl");
    curl = curl_easy_init();
    syslog (LOG_NOTICE, "Curl is ready");

    if (curl) {
        const char *postFields = "{  \"size\" : 70000,  \"query\": {    \"match\": {  \"event_type\": \"tls\"  } }} ";
        string r1 = "http://localhost:9200/_search?pretty&filter_path=took,hits.total,hits.hits._source.dest_ip,hits.hits._source.src_ip,hits.hits._source.tls.fingerprint,hits.hits._source.tls.ja3  ";
        FILE* bodyfile;
        string s1 = r1;
        syslog (LOG_NOTICE, "Openning body");
        bodyfile = fopen("/tmp/body.txt", "w");

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
            syslog (LOG_NOTICE, "JA_report failed with elasticsearch.");
            closelog ();
            return 1;
        }
        curl_easy_cleanup(curl);
        syslog (LOG_NOTICE, "Curl cleaned");
        //results saved to bodyfile
        fclose(bodyfile);
    }



    syslog (LOG_NOTICE, "Results saved to body");
//filtering not json strings
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
//parsing json
        reader.parse(ifs1, obj); // reader can also read strings
        const Json::Value& chards = obj["hits"]; // array of characters
        const Json::Value& st = chards["hits"]; // array of characters
        std::list<RuleItem>* white_list = new  list<RuleItem>();



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
                Item * iz = new Item(st.asString(),st1.asString(),"",st2.asString());
                items.push_back(*iz);
            }
        }

        for (unsigned int i =0 ; i < st.size() ; i++) {
            const Json::Value& tt = st[i]["_source"];
            const Json::Value& ttt  = tt["tls"]["ja3"];
            RuleItem * ir = new RuleItem(tt["dest_ip"].asString(),tt["src_ip"].asString(),ttt["hash"].asString(),ttt["string"].asString(),findCat(items,ttt["hash"].asString()));
            white_list->push_back(*ir);
            delete ir;

        }
        cout << "end list" << endl;

        white_list->sort(compare_nocase);
        white_list->unique(same_rule_item );
        ///////////////////////////////
        std::string z = path_to_files+"white_list_report.json";
        std::string zz = path_to_files+"white_list_data.txt";
        std::string b1 = path_to_files+"white_list_test.txt";
        wfile.open(z.c_str(),std::ofstream::out );
        reportfile.open(zz.c_str(), std::ofstream::out);
        testfile.open(b1.c_str(), std::ofstream::out);
        //Start making report
        Report rz;
        for (auto ii = white_list->begin(); ii != white_list->end(); ii++) {
            string t =  (*ii).getJa3_hash();
            string tm = (*ii).getJa3_string();
            //string t1 = (*ii).getDstIP();
            string t2 = (*ii).getSrcIP();
            string tz = (*ii).getCategory();
string ddesc = findDesc(items,t);
            Device * dddf = devs.find_device_by_ip(t2);
            string z, ni;
            if(dddf != nullptr)  {
                z = dddf->getName();    //if device exists
                ni = dddf->getNetwork_interface();
            } else {
                z = "unknown";
                ni = "unknown";
            }



            rz.add_new_line(t2,z,ni,t,tm,tz,ddesc);
            testfile << t2.c_str() << ";" << t.c_str() << ";" << tz.c_str() << ";" << ni.c_str() << ";" << tm.c_str() << endl;
            reportfile << t2.c_str() << ";" << t.c_str() << ";" << tz.c_str() << endl;

        }


        Json::Value array;
        Json::Value array1;
        Json::Value f,f1;
        Json::Value fingerprints;
        list<ReportItem> x = rz.get_whole_list();
        cout << "size:" << x.size() << endl;
        for (auto n = x.begin(); n != x.end(); n++) (*n).make_uniq();


        for (auto iii = x.begin(); iii != x.end(); iii++) {
            if( (devs.find_device_by_ip((*iii).getIp())) == nullptr) continue;
            string t = (*iii).getName();
            //    cout << "device name: " << t << "; ip:  " << (*iii).getIp() << endl;
            f["ip"] = (*iii).getIp();
            f["name"] = t;
            f["network interface"] = (*iii).getNetwork_interface();
            array.append(f);
            (*iii).make_uniq();
            list<Fingerprint> ff;
            ff.clear();
            ff = (*iii).getlist();
            if(!ff.empty()) {
                //  cout << "fingerprints:" << endl;

                for (auto mi = ff.begin(); mi != ff.end(); mi++) {
                    //      cout << (*mi).getJa3_hash() << endl;
                    f1["hash"] = (*mi).getJa3_hash();
                    f1["category"] = (*mi).getCategory();
                    f1["string"] = (*mi).getJa3_string();
                    f1["description"] = (*mi).getDescription();
                    fingerprints.append(f1);
                    f1.clear();
                }
                array1["fingerprints"] = fingerprints;
                fingerprints.clear();
                array.append(array1);
            }

        }
        ////////////////////////////////
        Json::Value fromScratch;


        fromScratch["devices"] = array;




        // write in a nice readible way
        Json::StyledWriter styledWriter;
        //  std::cout << styledWriter.write(fromScratch);
        wfile << styledWriter.write(fromScratch);
        wfile.close();
        reportfile.close();
        ////////////////////////////////
        // for (auto v = white_list->begin(); v != white_list->end(); v++) cout << "11" << endl;
    } catch (const std::exception& e) { // caught by reference to base
        std::cout << " a standard exception was caught, with message '"
                  << e.what() << "'\n";
        syslog (LOG_NOTICE, "JA_report failed parsing.");
        closelog ();
        return 2;
    }
    syslog (LOG_NOTICE, "Jreport finished. Success!");
    if( remove( "/tmp/body.txt" ) == 0 )
        syslog (LOG_INFO, "Jreport: temporary file body deleted");
    if( remove( "/tmp/body1.txt" ) == 0 )
        syslog (LOG_INFO, "Jreport: temporary file body1 deleted");
    closelog ();
    set_index(10000);
    // get_days();
    return 0;
}

