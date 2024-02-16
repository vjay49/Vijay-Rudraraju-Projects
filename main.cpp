#include "csvstream.h"
#include <iostream>
#include <map>
#include <string>
#include <cstring>
#include <set>
#include <fstream>
#include <sstream>
#include <cmath>
#include <vector>


using namespace std;

class Classifier{
public:
    Classifier(string file1_in, string file2_in, bool debug_in)
    :filename1(file1_in), filename2(file2_in), debug(debug_in){}
    
    
    void record(){
        cout.precision(3);
        csvstream csvin(filename1);
        if(debug == true){
            cout << "training data:" << endl;
        }
        while (csvin >> amount){
            cout.precision(3);
            count++;
            labeler(amount["tag"]);
            word_unique_count(amount["content"]);
            wordlabeler(amount["tag"], amount["content"]);
            if(debug == true){
                cout << "  " << "label = " << amount["tag"] << ", content = "
                << amount["content"] << endl;
            }
        }
        cout << "trained on " << count << " examples" << endl;
        count_unique = (int) word_count_content.size();
        if(debug == true){
            cout << "vocabulary size = " << count_unique << endl;
        }
        cout << endl;
    }
    
    void word_unique_count(string post){
        cout.precision(3);
        //count and map
        set<string> words = unique_words(post);
        
        //map word
        std::set<string>::iterator it2;
        //csvstream csvin2(filename1);
        for (it2 = words.begin(); it2 != words.end(); ++it2){
            word_count_content[*it2] +=1;
        }
    }

    
    void labeler(string lab1){
        cout.precision(3);
        //map labels
        label[lab1] += 1;
    }
    
    void wordlabeler(string lab1, string post){
        //words and labels map
        cout.precision(3);
        set<string> unique = unique_words(post);
        std::set<string>::iterator it;
        for (it = unique.begin(); it != unique.end(); ++it){
            label_word_map[{lab1, *it}] +=1;
        }
    }
    
////////////////////////////////////
//PREDICTERRR///////////////////////
    
    void log_prob(){
        cout.precision(3);
        double newcount = count;
        if(debug == true){
            cout << "classes:" << endl;
        }
        for (auto &p : label) {
            double log_prior = log(label[p.first] / newcount);
            if(debug == true){
            cout << "  " << p.first << ", " << label[p.first]
            << " examples, log-prior = " << log_prior << endl;
            }
            logpriors_label_log[p.first] = log_prior;
        }
        //likelihood
        if(debug == true){
            cout << "classifier parameters:" << endl;
        }
        for (auto &d : label_word_map) {
            double val = label_word_map[d.first];
            double log_likely = log( val / label[d.first.first]);
            if(debug == true){
            cout << "  " << d.first.first << ":" << d.first.second << ", count = "
            << d.second << ", log-likelihood = " << log_likely << endl;
            }
            loglikely_label_word_log[d.first] = log_likely;
        }
        if(debug == true){
        cout << endl;
        }
    }
    
    void finalcalc(){
        cout.precision(3);
        int num_correct = 0;
        int num_post = 0;
        csvstream csvin(filename2);
        cout << "test data:" << endl;
        while (csvin >> finish){
            set<string> words = unique_words(finish["content"]);
            std::set<string>::iterator it;
            for (auto &p : label) {
                double score = logpriors_label_log[p.first];
                for (it = words.begin(); it!= words.end(); ++it){
                    score += conditional(p.first, *it);
                    //cout << score;
                }
                scores[p.first] = score;
            }
            string max_key = "";  //find max
            double max = std::numeric_limits<double>::lowest();
            for(auto &d : scores){
                if(scores[d.first] > max){
                    max_key = d.first;
                    max = scores[d.first];
                }
            }
            //if(count == 0){
                //max = score;
            //}
            cout << "  correct = " << finish["tag"] << ", predicted = " << max_key
            << ", log-probability score = " << max << endl;
            cout << "  content = " << finish["content"] << endl << endl;
            if(finish["tag"] == max_key){
                ++num_correct;
            }
            ++num_post;
            scores.clear();
        }
        cout << "performance: " << num_correct << " / " << num_post
        << " posts predicted correctly" << endl;
    }
    
    
    //helper//
    double conditional(string label, string word){
        cout.precision(3);
        if(label_word_map.find({label, word}) == label_word_map.end()
           && word_count_content.find(word) != word_count_content.end()){
            double newcount = count;
            return log(word_count_content[word] / newcount);
        }
        else if(word_count_content.find(word) == word_count_content.end()){
            double newcount = count;
            return log(1 / newcount);
        }
        else{
            return loglikely_label_word_log[{label, word}];
        }
    }
   
    set<string> unique_words(const string &str) {
        //cout.precision(3);
        istringstream source(str);
        set<string> words;
        string word;
        
        while (source >> word) {
            words.insert(word);
        }
        return words;
    }
    

private:
    int count = 0;
    int count_unique = 0;
    string filename1;
    string filename2;
    map<string, string> amount; //initial in
    map<string, string> finish; //second in
    
    map<string, int> label; //label and number of times
    map<string, int> word_count_content; //words and number of times
    map<pair<string, string>, int> label_word_map; //label/words and num times
    bool debug;
    
    map<string, double> logpriors_label_log; //log prior stored here
    map<pair<string, string>, double> loglikely_label_word_log; //log like stored here
    
    map<string, double> scores; //final scores per label
};




int main(int argc, const char * argv[]) {
    cout.precision(3);
    if(argc < 3 || argc > 4){
        cout << "Usage: main.exe TRAIN_FILE TEST_FILE [--debug]" << endl;
        return 1;
    }
    string vall = "--debug";
    const char * val = vall.c_str();
    if(argc == 4 && strcmp(argv[3], val)){
        cout << "Usage: main.exe TRAIN_FILE TEST_FILE [--debug]" << endl;
        return 1;
    }
    bool debug_t = false;
    
    if(argc == 4){
        debug_t = true;
    }
    
    string filename1 = argv[1];
    string filename2 = argv[2];
    
    ifstream fin;
    fin.open(filename1);
    if (!fin.is_open()) {
        cout << "Error opening file: " << filename1 << endl;
        return 1;
    }
    ifstream fin1;
    fin1.open(filename2);
    if (!fin1.is_open()) {
        cout << "Error opening file: " << filename2 << endl;
        return 1;
    }
    
    Classifier c{argv[1], argv[2], debug_t};
    c.record();
    c.log_prob();
    c.finalcalc();
}


