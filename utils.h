#ifndef UTILS
#define UTILS

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <random>

using namespace std;

vector<vector<int>> readFromTxt(const string &filename) {
    vector<vector<int>> vec;
    ifstream file(filename);

    if(file.is_open()) {
        int num;
        vector<int> row;

        for(int i = 0; i < 2; i++) {
            file >> num;
            row.push_back(num);
        }
        vec.push_back(row);

        int eCount = row[1];
        for(int i = 0; i < eCount; i++) {
            vector<int> row;
            int v1,v2,w;
            file >> v1 >> v2 >> w;
            row.push_back(v1);
            row.push_back(v2);
            row.push_back(w);
            vec.push_back(row);
        }

        file.close();
    }
    else {
        std::cerr << "Nie udało się otworzyć pliku do odczytu\n";
    } 
    
    return vec;
}

int random(int begin, int end) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distribution(begin, end);

    return distribution(gen);
}

#endif