//01BD41C3BF016AD7E8B6F837DF18926EC3E83350

#include <iostream>
#include <fstream>
#include <getopt.h>
#include <string>
#include <vector>
#include <queue>
#include "log_runner.cpp"

using namespace std;

class Getopt{
public:
    void get_options(int argc, char** argv){
        int option_index = 0, option = 0;
        
        opterr = false;
        
        struct option longOpts[] = {{ "help", no_argument, nullptr, 'h' },
            { nullptr, 0, nullptr, '\0' }};
        
        while ((option = getopt_long(argc, argv, "h", longOpts, &option_index)) != -1) {
            switch (option) {
                case 'h':
                    cout << "provide a master log input file to read input from.\n"
                    << "Then provide commands in order to make changes to input values.\n";
                    exit(0);
            }
        }
    }
};

int64_t get_time_num(string time){
    int64_t multiple = 1;
    int64_t tvalue = 0;
    for(int i = (int) time.length() - 1; i >= 0; i--){
        if(time[(size_t)i] != ':'){
            tvalue += multiple * (time[(size_t)i] - '0');
            multiple *= 10;
        }
    }
    return tvalue;
}

int main(int argc, char * argv[]) {
    ios_base::sync_with_stdio(false);
    Getopt g;
    g.get_options(argc, argv);
    
    ifstream fin;
    fin.open(argv[1]);
    if (!fin.is_open()) {
        cerr << "open failed" << endl;
        return 1;
    }
    
    vector<log_entry> vec_entries;
    
    
    int entry_count = 0;
    string ts, cat, msg;
    while(getline(fin, ts, '|')){
        getline(fin, cat, '|');
        getline(fin, msg);
        log_entry log;
        log.timestamp_string = ts;
        log.int_ts = get_time_num(ts);
        log.category = cat;
        log.message = msg;
        log.entryID = entry_count;
        vec_entries.emplace_back(log);
        //stable.emplace_back(log);
        entry_count++;
    }
    
    vector<int> ids(vec_entries.size());
    cout << entry_count << " entries read\n";
    sort(vec_entries.begin(), vec_entries.end(), timeOrder());
    
    Runner r;
    r.fillA(vec_entries, ids);
    r.fillC(vec_entries);
    r.fillK(vec_entries); 
    
    char cmd;
    do{
        cout << "% ";
        cin >> cmd;
        if(cmd == 't'){
            string junk;
            string time1;
            getline(cin, junk, ' ');
            getline(cin, time1, '|');
            string time2;
            getline(cin, time2);
            if(time1.size() != 14 || time2.size() != 14){
                cerr << "provide times that are 14 characters\n";
            }
            else{
                int64_t time_start = get_time_num(time1);
                int64_t time_end = get_time_num(time2);
                r.commandT(vec_entries, time_start, time_end);
            }
        }
        else if(cmd == 'm'){
            string time;
            cin >> time;
            if(time.size() != 14){
                cerr << "provide a time thats 14 characters\n";
            }
            else{
                int64_t t = get_time_num(time);
                r.commandM(vec_entries, t);
            }
        }
        else if(cmd == 'c'){
            string cat;
            string junk;
            getline(cin, junk, ' ');
            getline(cin, cat);
            r.commandC(cat);
        }
        else if(cmd == 'k'){
            string msg;
            string junk;
            getline(cin, junk, ' ');
            getline(cin, msg);
            r.commandK(msg);
        }
        else if(cmd == 'a'){
            int index;
            cin >> index;
            r.commandA(ids, index);
        }
        else if(cmd == 'r'){
            r.commandR();
        }
        else if(cmd == 'd'){
            int index;
            cin >> index;
            r.commandD(index);
        }
        else if(cmd == 'b'){
            int index;
            cin >> index;
            r.commandBE(index, cmd);
        }
        else if(cmd == 'e'){
            int index;
            cin >> index;
            r.commandBE(index, cmd);
        }
        else if(cmd == 's'){
            r.commandS(vec_entries);
        }
        else if(cmd == 'l'){
            r.commandL(vec_entries);
        }
        else if(cmd == 'g'){
            r.commandG(vec_entries);
        }
        else if(cmd == 'p'){
            r.commandP(vec_entries);
        }
        else if(cmd == '#'){
            string comment;
            getline(cin, comment);
        }
        else{
            if(cmd != 'q'){
                cerr << "enter in a valid command\n";
            }
        }
    } while(cmd != 'q');
    return 0;
}
