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



//Global function which detect item category
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
//used for selecting uniq items
bool same_rule_item (RuleItem first, RuleItem second)
{
    string s1 = first.getItemHash();
    string s2 = second.getItemHash();
    if (s1.compare(s2) == 0) return true;

    return false;
}

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

CURLcode set_index(int n)
{
//  Preparing CURL to set elastic
    CURL *curl;
    std::string s2 = std::to_string(n);
    std::string s1 = "{  \"index.max_result_window\" : \"" + s2 + "\" } ";
    // const char *postFields = "{  \"index.max_result_window\" : \"" + s + "\" } ";
    const char *postFields = s1.c_str();
    CURLcode res = CURLE_OK;


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
    std::ofstream wfile;

    std::string mode = "a";

    setlogmask (LOG_UPTO (LOG_NOTICE));
    Configuration::getInstance()->read_configuration();
    
    const char * fingerprints_file = Configuration::getInstance()->get_fingerprints_file();
    openlog ("ja_parser", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
    syslog (LOG_NOTICE, "Program started by User %d", getuid ());
//Check for path argument
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " DESTINATION MODE (a - alerts, d - drop)" << std::endl;
        return 1;
    }
    string path_to_files = argv[1];
    mode = argv[2];
    if (!path_to_files.empty() && path_to_files.back() != '/') //detecting trailing slash
        path_to_files += '/';
    syslog (LOG_NOTICE, "Program started elastic index");
    set_index(750000);
    syslog (LOG_NOTICE, "Index is set");
//Preparing CURL to ask elastic
    CURL *curl;

    CURLcode res;

    syslog (LOG_NOTICE, "Try to init curl");
    curl = curl_easy_init();
    syslog (LOG_NOTICE, "Curl is ready");


    if (curl) {
        FILE* bodyfile;
        const char *postFields = "{  \"size\" : 10000,  \"query\": {    \"match\": {  \"event_type\": \"tls\"  } }} ";

        string r1 = "http://localhost:9200/_search?pretty  ";

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
            syslog (LOG_NOTICE, "JA_parser failed with elasticsearch.");
            closelog ();
            return 1;
        }
        curl_easy_cleanup(curl);
        syslog (LOG_NOTICE, "Curl cleaned");
        fclose(bodyfile);
    }

//results saved to bodyfile

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
    std::list<RuleItem>* white_list = new  list<RuleItem>;
    try {
        ifstream ifs1("/tmp/body1.txt");
//parsing json
        reader.parse(ifs1, obj); // reader can also read strings
        const Json::Value& chards = obj["hits"]; // array of characters
        const Json::Value& st = chards["hits"]; // array of characters
        //    std::list<RuleItem>* white_list = new  list<RuleItem>;



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
            delete ir;

        }
        cout << "end list" << endl;

        white_list->sort(compare_nocase);
        white_list->unique(same_rule_item );
        ///////////////////////////////
        std::string z = path_to_files+"white_list.rules";
        wfile.open(z.c_str(),std::ofstream::out );
        int u = 1100000;

        if(mode == "d") {
            for (auto ii = white_list->begin(); ii != white_list->end(); ii++) {
                string t = (*ii).getJa3_hash();
                string t1 = (*ii).getDstIP();
                string t2 = (*ii).getSrcIP();
                string tz = (*ii).getCategory();
                //cout <<  << "  " <<  << "   " <<  << (*ii).getCategory() << endl;
                char str[300];
                if(((*ii).getCategory() == "Malware ") || ((*ii).getCategory() == "Unknown Software"))
                    sprintf(str,"drop tls %s any -> %s any (msg:\"drop match whitelisting hash malware or unknown hash %s\"; ja3_hash; content:\"%s\"; sid:%d;rev:11;)", t2.c_str(),t1.c_str(),tz.c_str(),t.c_str(), u);
                else
                    sprintf(str,"pass tls %s any -> %s any (msg:\"pass match whitelisting hash %s\"; ja3_hash; content:\"%s\"; sid:%d;rev:11;)", t2.c_str(),t1.c_str(),tz.c_str(),t.c_str(), u);
                wfile << str << endl;
                u++;
            }
            //wfile << "drop tls any any -> any any (msg:\"don't match JA3 hash\"; sid:%d;rev:11;)";
            char str2[300];
            sprintf(str2,"drop tls any any -> any any (msg:\"not match any  whitelisting hash\"; sid:%d;rev:11;)", u);
            wfile << str2 << endl;
            wfile.close();
        } else {
            for (auto ii = white_list->begin(); ii != white_list->end(); ii++) {
                string t = (*ii).getJa3_hash();
                string t1 = (*ii).getDstIP();
                string t2 = (*ii).getSrcIP();
                string tz = (*ii).getCategory();
                //cout <<  << "  " <<  << "   " <<  << (*ii).getCategory() << endl;
                char str[300];
                if(((*ii).getCategory() == "Malware ") || ((*ii).getCategory() == "Unknown Software"))
                    sprintf(str,"alert tls %s any -> %s any (msg:\"alert match whitelisting hash malware or unknown hash %s\"; ja3_hash; content:\"%s\"; sid:%d;rev:11;)", t2.c_str(),t1.c_str(),tz.c_str(),t.c_str(), u);
                else
                    sprintf(str,"pass tls %s any -> %s any (msg:\"pass match whitelisting hash %s\"; ja3_hash; content:\"%s\"; sid:%d;rev:11;)", t2.c_str(),t1.c_str(),tz.c_str(),t.c_str(), u);
                wfile << str << endl;
                u++;
            }
            //wfile << "drop tls any any -> any any (msg:\"don't match JA3 hash\"; sid:%d;rev:11;)";
            char str2[300];
            sprintf(str2,"alert tls any any -> any any (msg:\"not match any  whitelisting hash\"; sid:%d;rev:11;)", u);
            wfile << str2 << endl;
            wfile.close();
            white_list->clear();


        }

    } catch (const std::exception& e) { // caught by reference to base
        std::cout << " a standard exception was caught, with message '"
                  << e.what() << "'\n";
        syslog (LOG_NOTICE, "JA_parser failed parsing.");
        white_list->clear();
        closelog ();
        return 2;
    }
    syslog (LOG_NOTICE, "JA_parser finished. Success!");
    if( remove( "/tmp/body.txt" ) == 0 )
        syslog (LOG_INFO, "JA_parser: temporary file body deleted");
    if( remove( "/tmp/body1.txt" ) == 0 )
        syslog (LOG_INFO, "JA_parser: temporary file body1 deleted");
    closelog ();
    set_index(10000);
    white_list->clear();
    delete white_list;
    // get_days();
    return 0;
}

