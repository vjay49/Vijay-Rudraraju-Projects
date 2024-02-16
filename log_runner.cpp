//01BD41C3BF016AD7E8B6F837DF18926EC3E83350
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <unordered_map>
#include <algorithm>

using namespace std;

struct log_entry{
    int64_t int_ts;
    string timestamp_string;
    string category;
    string message;
    int entryID;
};

/*
struct excerpt_entry{
    int index;
    //log_entry log; //maybe make this an index to the log entry vector?
};
*/
//functors
class timeOrder{
public:
    bool operator()(const log_entry &log1, const log_entry &log2) {
        string cat1 = log1.category;
        transform(cat1.begin(), cat1.end(), cat1.begin(), ::tolower);
        string cat2 = log2.category;
        transform(cat2.begin(), cat2.end(), cat2.begin(), ::tolower);
        if(log1.int_ts == log2.int_ts){
            if(cat1 == cat2){
                return log1.entryID < log2.entryID;
            }
            return cat1 < cat2;
        }
        return log1.int_ts < log2.int_ts;
    }
};

class onlyTime{
public:
    bool operator()(const log_entry &log1, const log_entry &log2) {
        return log1.int_ts < log2.int_ts;
    }
};

class timeOrderEx{
public:
    bool operator()(int &one, int &two/*const excerpt_entry &ex1, const excerpt_entry &ex2*/) {
        /*
        if(ex1.log.int_ts == ex2.log.int_ts){
            if(ex1.log.category == ex2.log.category){
                return ex1.log.entryID < ex2.log.entryID;
            }
            return ex1.log.category > ex2.log.category;
        }
        return ex1.log.int_ts < ex2.log.int_ts;
         */
        return one < two;
    }
};

//Class
class Runner{
public:
    void commandT(vector<log_entry> &entries, int64_t time_s, int64_t time_e){
        int count = 0;
        vector<log_entry>::iterator low,up;
        log_entry comp1;
        comp1.int_ts = time_s;
        log_entry comp2;
        comp2.int_ts = time_e;
        low = lower_bound(entries.begin(), entries.end(), comp1, onlyTime());
        up = upper_bound(entries.begin(), entries.end(), comp2, onlyTime());
        vector<int> temp;
        count = (int)(up - low);
        while(low != up){
            temp.emplace_back((int)(low - entries.begin()));
            low++;
        }

        cmd_holder = temp;
        cout << "Timestamps search: " << count << " entries found\n";
        search_occured = true;
    }
    
    void commandM(vector<log_entry> &entries, int64_t time){
        int count = 0;
        vector<log_entry>::iterator low,up;
        log_entry comp;
        comp.int_ts = time;
        low = lower_bound(entries.begin(), entries.end(), comp, onlyTime());
        up = upper_bound(entries.begin(), entries.end(), comp, onlyTime());
        vector<int> temp;
        //if(low != entries.end() && up != entries.end()){
            count = (int)(up - low);
            while(low != up){
                temp.emplace_back((int)(low - entries.begin()));
                low++;
            }
        
        //}
        cmd_holder = temp;
        cout << "Timestamp search: " << count << " entries found\n";
        search_occured = true;
    }
    
    void fillC(vector<log_entry> &entries){
        for(size_t i = 0; i < entries.size(); i++){
            string cat = entries[i].category;
            transform(cat.begin(), cat.end(), cat.begin(), ::tolower);
            //populate map with lower case keys or lowercase categories
            //so when C command is made and its lowercase it works out
            map[cat].emplace_back((int)i);
        }
    }
    
    void commandC(string &category){
        transform(category.begin(), category.end(), category.begin(), ::tolower);
        int count = 0;
        unordered_map<string, vector<int>>::const_iterator got = map.find(category);
        if(got != map.end()){
            cmd_holder = got->second;
        }
        else{
            vector<int> temp;
            cmd_holder = temp;
        }
        count = (int) cmd_holder.size();
        cout << "Category search: " << count << " entries found\n";
        search_occured = true;
    }
    
    void fillK(vector<log_entry> &entries){
        for(size_t i = 0; i < entries.size(); i++){
            string word_cat;
            string cat = entries[i].category;
            transform(cat.begin(), cat.end(), cat.begin(), ::tolower);
            
            for(size_t k = 0; k < cat.size(); k++){
                if(isalnum(cat[k]) != 0){
                    word_cat = word_cat + cat[k];
                }
                else{
                    if(word_cat.size() != 0){
                        if(!mapk[word_cat].empty() && mapk[word_cat].back() != (int)i){
                            mapk[word_cat].emplace_back((int)i);
                        }
                        else if(mapk[word_cat].empty()){
                            mapk[word_cat].emplace_back((int)i);
                        }
                        word_cat.clear();
                    }
                }
            }
            if(word_cat.size() != 0){
                if(!mapk[word_cat].empty() && mapk[word_cat].back() != (int)i){
                    mapk[word_cat].emplace_back((int)i);
                }
                else if (mapk[word_cat].empty()){
                    mapk[word_cat].emplace_back((int)i);
                }
            }
            
            //message
            string msg = entries[i].message;
            transform(msg.begin(), msg.end(), msg.begin(), ::tolower);
            
            string word;
            for(size_t j = 0; j < msg.size(); j++){
                if(isalnum(msg[j]) != 0){
                    word = word + msg[j];
                }
                else{
                    if(word.size() != 0){
                        if(!mapk[word].empty() && mapk[word].back() != (int)i){
                            mapk[word].emplace_back((int)i);
                        }
                        else if (mapk[word].empty()){
                            mapk[word].emplace_back((int)i);
                        }
                        word.clear();
                    }
                }
            }
            if(word.size() != 0){
                if(!mapk[word].empty() && mapk[word].back() != (int)i){
                    mapk[word].emplace_back((int)i);
                }
                else if (mapk[word].empty()){
                    mapk[word].emplace_back((int)i);
                }
            }
        }
    }
    
    void commandK(string &search_words){
        transform(search_words.begin(), search_words.end(), search_words.begin(), ::tolower);
        int count = 0;
        bool intered = false;
        vector<string> words;
        string word;
        for(size_t i = 0; i < search_words.size(); i++){
            if(isalnum(search_words[i]) != 0){
                 word = word + search_words[i];
            }
            else{
                if(word.size() != 0){
                    words.emplace_back(word);
                    word.clear();
                }
            }
        }
        if(word.size() != 0){
            words.emplace_back(word);
        }
        for(size_t j = 0; j < words.size(); j++){
            unordered_map<string, vector<int>>::const_iterator got = mapk.find(words[j]);
            if(got != mapk.end() && intered == false){
                cmd_holder = got->second;
                intered = true;
            }
            else if(got != mapk.end() && intered == true){
                vector<int> v_intersection;
                set_intersection(cmd_holder.begin(), cmd_holder.end(),
                got->second.begin(), got->second.end(), back_inserter(v_intersection));
                cmd_holder = v_intersection;
            }
            else{
                vector<int> temp;
                cmd_holder = temp;
                break;
            }
        }
        count = (int) cmd_holder.size();
        cout << "Keyword search: " << count << " entries found\n";
        search_occured = true;
    }
    
    void fillA(vector<log_entry> &entries, vector<int> &ids){
        for(size_t i = 0; i < entries.size(); i++){
            ids[(size_t)entries[i].entryID] = (int)i;
        }
    }
    
    void commandA(vector<int> &ids, int val){
        if((size_t) val >= ids.size()){
            cerr << "index is not valid\n";
        }
        else{
            //excerpt_entry newEx;
            //newEx.index = ids[(size_t)val];
            excerpt_list.emplace_back(ids[(size_t)val]);
            cout << "log entry " << val << " appended\n";
        }
    }
    
    void commandR(){
        if(search_occured == true){
            for(size_t i = 0; i < cmd_holder.size(); i++){
                //excerpt_entry newEx;
                //newEx.index = cmd_holder[i];
                //newEx.excerpt_num = (int) excerpt_list.size();
                excerpt_list.emplace_back(cmd_holder[i]);
            }
            cout << cmd_holder.size() << " log entries appended\n";
        }
        else{
            cerr << "search did not occur\n";
        }
    }
    
    void commandD(int val){
        if((size_t)val >= excerpt_list.size()){
            cerr << "index is not valid\n";
        }
        else{
            int size = (int) excerpt_list.size();
            for(int i = 0; i < size; i++){
                if(i < val){
                    int top = excerpt_list.front();
                    excerpt_list.pop_front();
                    excerpt_list.emplace_back(top);
                }
                else if(i == val){
                    excerpt_list.pop_front();
                }
                else{
                    int top = excerpt_list.front();
                    //top.excerpt_num--;
                    excerpt_list.pop_front();
                    excerpt_list.emplace_back(top);
                }
            }
            cout << "Deleted excerpt list entry " << val << "\n";
        }
    }
    
    void commandBE(int val, char &cmd){
        if((size_t) val >= excerpt_list.size()){
            cerr << "index is not valid\n";
        }
        else{
            int size = (int) excerpt_list.size();
            int first = excerpt_list.front();
            int last = excerpt_list.back();
            for(int i = 0; i < size; i++){
                if(cmd == 'b'){
                    if(i < val){
                        int top = excerpt_list.front();
                        //top.excerpt_num++;
                        excerpt_list.pop_front();
                        excerpt_list.emplace_back(top);
                    }
                    else if(i == val){
                        first = excerpt_list.front();
                        //first.excerpt_num = 0;
                        excerpt_list.pop_front();
                    }
                    else{
                        int top = excerpt_list.front();
                        excerpt_list.pop_front();
                        excerpt_list.emplace_back(top);
                    }
                }
                else{
                    if(i < val){
                        int top = excerpt_list.front();
                        excerpt_list.pop_front();
                        excerpt_list.emplace_back(top);
                    }
                    else if(i == val){
                        last = excerpt_list.front();
                        //last.excerpt_num = (int) excerpt_list.size() - 1;
                        excerpt_list.pop_front();
                    }
                    else{
                        int top = excerpt_list.front();
                        //top.excerpt_num--;
                        excerpt_list.pop_front();
                        excerpt_list.emplace_back(top);
                    }
                }
            }
            if(cmd == 'b'){
                excerpt_list.emplace_front(first);
            }
            else{
                excerpt_list.emplace_back(last);
            }
            cout << "Moved excerpt list entry " << val << "\n";
        }
    }

    void commandS(vector<log_entry> &entries){
        if(excerpt_list.empty()){
            cout << "excerpt list sorted\n" << "(previously empty)\n";
        }
        else{
            cout << "excerpt list sorted\n" << "previous ordering:\n";
            int front = excerpt_list.front();
            cout << 0 << "|" << entries[(size_t)front].entryID
            << "|" << entries[(size_t)front].timestamp_string << "|"
            << entries[(size_t)front].category << "|"
            << entries[(size_t)front].message << "\n...\n";
            
            int back = excerpt_list.back();
            cout << excerpt_list.size() - 1 << "|" << entries[(size_t)back].entryID
            << "|" << entries[(size_t)back].timestamp_string << "|"
            << entries[(size_t)back].category << "|" << entries[(size_t)back].message
            << "\n";
            
            sort(excerpt_list.begin(), excerpt_list.end(), timeOrderEx());
            
            cout << "new ordering:\n";
            int fronts = excerpt_list.front();
            cout << 0 << "|" << entries[(size_t)fronts].entryID
            << "|" << entries[(size_t)fronts].timestamp_string << "|"
            << entries[(size_t)fronts].category << "|" << entries[(size_t)fronts].message
            << "\n...\n";
            
            int backs = excerpt_list.back();
            cout << excerpt_list.size() - 1 << "|" << entries[(size_t)backs].entryID
            << "|" << entries[(size_t)backs].timestamp_string << "|"
            << entries[(size_t)backs].category << "|" << entries[(size_t)backs].message
            << "\n";
        }
    }
    
    void commandL(vector<log_entry> &entries){
        if(excerpt_list.empty()){
            cout << "excerpt list cleared\n" << "(previously empty)\n";
        }
        else{
            cout << "excerpt list cleared\n" << "previous contents:\n";
            int front = excerpt_list.front();
            cout << 0 << "|" << entries[(size_t)front].entryID
            << "|" << entries[(size_t)front].timestamp_string << "|"
            << entries[(size_t)front].category << "|" << entries[(size_t)front].message
            << "\n...\n";
            
            int back = excerpt_list.back();
            cout << excerpt_list.size() - 1 << "|" << entries[(size_t)back].entryID
            << "|" << entries[(size_t)back].timestamp_string << "|"
            << entries[(size_t)back].category << "|" << entries[(size_t)back].message
            << "\n";
            
            excerpt_list.clear();
        }
    }
    
    void commandG(vector<log_entry> &entries){
        if(search_occured == true){
            for(size_t i = 0; i < cmd_holder.size(); i++){
                log_entry cur = entries[(size_t) cmd_holder[i]];
                cout << cur.entryID << "|" << cur.timestamp_string << "|"
                << cur.category << "|" << cur.message
                << "\n";
            }
        }
        else{
            cerr << "search did not occur\n";
        }
    }
    
    void commandP(vector<log_entry> &entries){
        for(size_t i = 0; i < excerpt_list.size(); i++){
            int front = excerpt_list.front();
            excerpt_list.pop_front();
            cout << i << "|" << entries[(size_t)front].entryID
            << "|" << entries[(size_t)front].timestamp_string << "|"
            << entries[(size_t)front].category << "|"
            << entries[(size_t)front].message << "\n";
            excerpt_list.emplace_back(front);
        }
    }
    
private:
    bool search_occured = false;
    vector<int> cmd_holder;
    deque<int> excerpt_list;
    unordered_map<string, vector<int>> map;
    unordered_map<string, vector<int>> mapk;
};
