#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <stdio.h>
#include "Ngram.h"
using namespace std;

#define MAX_TEST_LINES 2048
#define MAX_LINE_LENGTH 256

// ===== Copied from TA's ngram_test.cpp =====[Begin of copy]
int ngram_order = 2;
Vocab voc;
Ngram lm(voc, ngram_order);

// Get P(W2 | W1) -- bigram
double getBigramProb(const char *w1, const char *w2)
{
    VocabIndex wid1 = voc.getIndex(w1);
    VocabIndex wid2 = voc.getIndex(w2);
    // cout << wid1 << " " << wid2 << endl;

    if(wid1 == Vocab_None)  //OOV
        wid1 = voc.getIndex(Vocab_Unknown);
    if(wid2 == Vocab_None)  //OOV
        wid2 = voc.getIndex(Vocab_Unknown);

    VocabIndex context[] = { wid1, Vocab_None };
    return lm.wordProb( wid2, context);
}
// ===== Copied from TA's ngram_test.cpp =====[End of copy]

int main(int argc, char *argv[]) {
    // Load bigram.lm to Ngram model.
    // argv[6] == "bigram.lm"
    File lmFile(argv[6], "r");
    lm.read(lmFile);
    lmFile.close();

    // Load testing data to lines (string[]).
    // argv[2] == "./testdata/xx.txt"
    ifstream testFile(argv[2]);
    string lines[MAX_TEST_LINES];
    string line;
    int test_num_line = 0;
    while(getline(testFile, line)) {
        lines[test_num_line++] = line;
    }
    testFile.close();

    // argv[4] == "./ZhuYin-Big5.map"
    ifstream mapFile(argv[4]);
    map <string, string> ZhuYin_Big5_map;
    while(getline(mapFile, line)) {
        // ZhuYin-Big5 is split by space.
        size_t pos = line.find(" ");
        // Left element is ZhuYin (key).
        string key = line.substr(0, pos);
        // Right element is Big5 (values).
        string values = line.erase(0,pos + 1);
        // Push key-values pair to map.
        ZhuYin_Big5_map[key] = values;
    }
    mapFile.close();

    // Split each line to vector.
    vector <vector <string> > vec(MAX_LINE_LENGTH);
    for(int i = 0; i < test_num_line; ++i){
        // Fetch one line from testing data.
        string line = lines[i];
        // Push begin of sentence <s> to vector.
        vec[i].push_back("<s>");
        // Each word (big5) is split by space.
        string delimiter = " ";
        // Find position of delimiter (' ') in a line.
        size_t pos = 0;
        while ((pos = line.find(delimiter)) != std::string::npos) {
            // Fetch char(s) in a line.
            string big5 = line.substr(0, pos);
            // A word (big5) should be 2 chars.
            if(big5.length() == 2)
                // Push word (big5) to vector.
                vec[i].push_back(big5);
            // Erase char(s) we have read in a line.
            line.erase(0, pos + delimiter.length());
        }
        // Push end of sentence </s> to vector.
        vec[i].push_back("</s>");  
    }    

    // Viterbi forward throughout the sequences (line by line).
    for(int i = 0; i < test_num_line; ++i){
        // Score and path for a sequence (one line).
        vector< vector <double> > score (vec[i].size());
        vector< vector <int> > path (vec[i].size());
        // Viterbi forwar throughout a sequence (one line).
        for(int t = 1; t < vec[i].size(); ++t) {
            string prev_big5_string = ZhuYin_Big5_map[vec[i][t - 1]];
            string curr_big5_string = ZhuYin_Big5_map[vec[i][t]];

            // Second words' previous word is <s>.
            if(t == 1) prev_big5_string = "<s>";
            // Last word is </s>.
            if(t == vec[i].size() - 1) curr_big5_string = "</s>";
            // If current big5 string only have one value (big5).
            if(curr_big5_string.length() < 2) {
                // Assign word (big5) directly.
                curr_big5_string = vec[i][t];
            }        

            // Split prev_big5_string to vector of string.
            vector <string> prev_words;
            // Each word (big5) is split by space.
            string delimiter = " ";
            // Find position of delimiter (' ') in a line.
            size_t pos = 0;
            while ((pos = prev_big5_string.find(delimiter)) != std::string::npos) {
                // Fetch char(s) in a line.
                string big5 = prev_big5_string.substr(0, pos);
                // A word (big5) should be 2 chars.
                if(big5.length() == 2)
                    // Push word (big5) to vector.
                    prev_words.push_back(big5);
                // Erase char(s) we have read in a line.
                prev_big5_string.erase(0, pos + delimiter.length());
            }
            // Push end of sentence </s> to vector.
            prev_words.push_back(prev_big5_string);

            // Split curr_big5_string to vector of string.
            vector <string> curr_words;
            // Each word (big5) is split by space.
            delimiter = " ";
            // Find position of delimiter (' ') in a line.
            pos = 0;
            while ((pos = curr_big5_string.find(delimiter)) != std::string::npos) {
                // Fetch char(s) in a line.
                string big5 = curr_big5_string.substr(0, pos);
                // A word (big5) should be 2 chars.
                if(big5.length() == 2)
                    // Push word (big5) to vector.
                    curr_words.push_back(big5);
                // Erase char(s) we have read in a line.
                curr_big5_string.erase(0, pos + delimiter.length());
            }
            // Push end of sentence </s> to vector.
            curr_words.push_back(curr_big5_string);

            // Sencond word's (t == 1) previous word is <s>.
            if(t == 1){
                // Initialization for bigram Viterbi.
                for(int k = 0; k < curr_words.size(); ++k){
                    score[t].push_back(getBigramProb("<s>", curr_words[k].c_str()));
                    path[t].push_back(0);
                }
            }
            else{
                // Forward pass for biagram Viterbi.
                for(int k = 0; k < curr_words.size(); ++k) {
                    double max_prob = -99999999;
                    int max_prob_path = 0;
                    for(int j = 0; j < prev_words.size(); ++j) {
                        // Accumulate bigram probability.
                        double prob = score[t - 1][j] + getBigramProb(prev_words[j].c_str(), curr_words[k].c_str());
                        // Update max. probability and record it's path.
                        if(prob > max_prob && prob < 0) {
                            max_prob = prob;
                            max_prob_path = j;
                        }
                    }
                    // Record max. probability and it's path in vectors.
                    score[t].push_back(max_prob);
                    path[t].push_back(max_prob_path);
                }
            }
        }

        // Initialize index with '0' (</s>).
        int max_prob_index = 0;
        vector< vector <string> > revised_sequence(MAX_LINE_LENGTH);
        // Viterbi backtracking.
        for(int t = vec[i].size() - 1; t >= 1; --t){
            string curr_big5_string = ZhuYin_Big5_map[vec[i][t]]; 

            // Last word is </s>.
            if(t == vec[i].size() - 1) curr_big5_string = "</s>";

            // Split curr_big5_string to vector of string.
            vector <string> curr_words;
            // Each word (big5) is split by space.
            string delimiter = " ";
            // Find position of delimiter (' ') in a line.
            size_t pos = 0;
            while ((pos = curr_big5_string.find(delimiter)) != std::string::npos) {
                // Fetch char(s) in a line.
                string big5 = curr_big5_string.substr(0, pos);
                // A word (big5) should be 2 chars.
                if(big5.length() == 2)
                    // Push word (big5) to vector.
                    curr_words.push_back(big5);
                // Erase char(s) we have read in a line.
                curr_big5_string.erase(0, pos + delimiter.length());
            }
            // Push end of sentence </s> to vector.
            curr_words.push_back(curr_big5_string);

            // Record word with max. probability.
            revised_sequence[i].push_back(curr_words[max_prob_index]);
            // Update word's index with max. probability.
            max_prob_index = path[t][max_prob_index];
        }
        // Push begin of sentence <s> to vector.
        revised_sequence[i].push_back("<s>");
        
        // Print sequence (the whole line) from back to front.
        for(int k = revised_sequence[i].size() - 1; k >= 1; --k){
            cout << revised_sequence[i][k] << " ";
        }
        // Print the first element (</s>).
        cout << revised_sequence[i][0] << endl;       
    }
    return 0;
}