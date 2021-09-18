#include <iostream>
#include <string>
#include <stdio.h>
#include <list>
#include <fstream>
#include <jsoncpp/json/json.h> // or something
#include "item.h"

using namespace std;
std::string sortedStr(Json::Value & value, std::vector<std::string> sortKeys) 
{
    Json::Value sortedValue;  // The JSON object to store the new (sorted) hash
    char newKey[60]; // I use C expressions, modify to C++ if you like 
    // Build new sortedValue
    int i = 0;
    for (auto & key : sortKeys) {
        sprintf(newKey, "SORTEDKEY:%03d-%s", i++, key.c_str());
        sortedValue[newKey] = value[key];
    }
    // Write to string, should be sorted on primary keys
    Json::StyledWriter w;
    std::string result = w.write(sortedValue);
    // Remove aux. tags from primary keys 
    std::size_t pos = 0;       
    while ((pos = result.find("SORTEDKEY:", pos)) != std::string::npos) {
        result.erase(pos, 14);
    }
    return result;
}
int main(int argc, char * argv[])
{
   Json::Reader reader;
    Json::Value obj;
  std::string name;
   list <string> namelist;
   list <Item> items;
 //  list<string>::iterator i;
   std::string s; 
   std::string s1;
   std::string s2;
   std::string s4;
   std::string path_to_files = "";
   std::ofstream ifile, wfile;
 //  if(argc == 1) {
  //               printf("Command line has no additional arguments\n");
  //               return 1;
  //       }
  // path_to_files = argv[1];
   while(std::getline (std::cin,name)) {
	   
	reader.parse(name, obj);   
        const Json::Value& st = obj["desc"];
	const Json::Value& st1 = obj["ja3_hash"];
	const Json::Value& st2 = obj["ja3_string"];
	if(st1.asString().length() < 40) {
	  
	Item * iz = new Item(st.asString(),st1.asString(),st2.asString());
	items.push_back(*iz); }
   }
  string line;
  ifstream ifs("/etc/ja3parser/categories_orig.txt");
  int k=0;
    while (!ifs.eof() ) {
        std::getline (ifs,line);
	//cout << line;
        namelist.push_back(line);
    }
    
    ifs.close();
    namelist.sort();
	
//	std::string z = path_to_files+"jar3mapping.csv";
//	ifile.open(z.c_str(),std::ofstream::out | std::ofstream::app);
  list<Item>::iterator i;
  for(i = items.begin(); i != items.end(); i++) {
    (*i).findCategory(namelist);
	//	cout << (*i).getCategory() << "\n";
  }
  


Json::FastWriter writer;
// Make a new JSON document for the configuration. Preserve original comments.

int d = 1;
 for(i = items.begin(); i != items.end(); i++) {
  Json::Value root;
  root["#"] = d;
  root["Ja3_hash"] = (*i).getJa3_hash();
  root["desc"] = (*i).getDesc().substr(0,48);
  root["category"] = (*i).getCategory();
  std::string outputst = writer.write( root ); 
  //std::string outputst1 = sortedStr(outputst, {"ja3_hash", "desc", "category"});
  
  cout <<  outputst;
  d++;
  }
/*  ifile.close();
  z = path_to_files+"white_list.csv";
  wfile.open(z.c_str(),std::ofstream::out | std::ofstream::app);
  for(i = white_list.begin(); i != white_list.end(); i++)
		wfile << (*i) << "\n";
  wfile.close(); */
}
