#include "helper.h"

using namespace std;



/////////////////////////////////////////////
long get_sid()
{

    
    ifstream fin(COUNT_FILE);

    if (!fin.is_open()) {
        ofstream fout(COUNT_FILE);
        fout << "1100000";
        fout.close();
        return 1100000;
    } else {
        char buff[50];
        fin >> buff;
        return atoi(buff);
        
    }
return 1100000;
}

void set_sid(long s)
{


    ofstream fout(COUNT_FILE);
    fout << s << endl;
    fout.close();
    return;
}

int check_template(const char * path_to_file, const char* templ) {
    
    const char * config_file = path_to_file;
    std::ifstream ifs2(config_file);
    
    int count = 0;
    std::string name;
        while(std::getline (ifs2,name)) {
            count++;
            
         char *p;
         char cstr[name.size() + 1];
         strcpy(cstr, name.c_str());
         p = strstr(cstr, templ); 
         if(p != nullptr) {
             
	         return count;
         }
        }
  ifs2.close();
  return 0;  
}

void modify_file(const char * path_to_file, int row_number, const string templ) {
  const char * config_file = path_to_file;
  std::ifstream ifs2(config_file);
  std::ofstream wout;
  int count = 0;
  list <string> l;
  std::string name;
  
      while(std::getline (ifs2,name)) {
         count++;
         if(count == row_number) l.push_back(templ);
         l.push_back(name);
	     
      } 
      wout.open(config_file);
      for(auto z: l) {
         wout << z << endl; 
      }
      wout.close();
}

void delete_from_file(const char * path_to_file, int row_number) {
  const char * config_file = path_to_file;
  std::ifstream ifs2(config_file);
  std::ofstream wout;
  int count = 0;
  std::string name;
  list <string> l;
  
      while(std::getline (ifs2,name)) {
         count++;
         if(count == row_number) continue;
         l.push_back(name);
	     
      } 
      wout.open(config_file);
      for(auto z: l) {
         wout << z << endl; 
      }
      wout.close();
}

void check()
{

    
    ifstream fin(RULES_FILE);

    if (!fin.is_open()) {
        ofstream fout(RULES_FILE);
        fout << "#classification:security" << endl;
        fout << "#description:ja3 fingerprints" << endl;
        fout << " " << endl;
        fout.close();
        return;
    } 
    return;
}

string findCategory(list<Item> l, string s)
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

