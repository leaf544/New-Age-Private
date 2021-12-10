#ifndef CATEGORY_H
#define CATEGORY_H

/* STD */
#include <iostream>
#include <vector>
#include <vector>
#include <iterator>
#include <string>
#include <map>
#include <sstream>
#include <fstream>

/* C */
#include <Windows.h> 
#include <stdlib.h>
#include <conio.h>
#include <string.h>

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::map;

class Category {
public:
    map<string, vector<string>> Variables;
    bool hasVariables = false;
    
    string FetchDomRaw (string iden) {
        return this->Variables[iden].size() ? this->Variables[iden][0] : "0";
    }

    int FetchDomInt (string iden) {
        return atoi(FetchDomRaw(iden).c_str());
    }

    string FetchValueRaw (string iden, int at) {
        return this->Variables[iden][at] != "" ? this->Variables[iden][at] : "0";
    }

    int FetchValueInt (string iden, int at) {
        return atoi(FetchValueRaw(iden, at).c_str());
    }
    
    // std::string FetchValue (std::string iden) {
    //     return this->Variables[iden] != "" ? this->Variables[iden] : "0";
    // }
    
    // int FetchValueInt (std::string iden) {
    //     return atoi(FetchValue(iden).c_str());
    // }
    
    void ListVariables () {
        for (auto it = this->Variables.begin(); it != this->Variables.end(); it++) {
            if (it->first != "" && it->second.size()) {
                cout << "IDEN: " << it->first << " / " << it->second[0] << endl;
            }
        }
    }
    
};

#endif
