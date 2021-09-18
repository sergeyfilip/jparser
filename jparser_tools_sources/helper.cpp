#include "helper.h"

using namespace std;

const char * count_file = "/etc/jparser/count.txt";


/////////////////////////////////////////////
long get_sid()
{

    char buff[50];
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


    ofstream fout(count_file);
    fout << s << endl;
    fout.close();
    return;
}
