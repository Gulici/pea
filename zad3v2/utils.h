#ifndef UTILS
#define UTILS

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <random>
#include "libs\json.hpp"

using namespace std;
using json = nlohmann::json;

struct Instance {
    int size;
    int optimum;
    std::vector<std::vector<int>> matrix;
    double avgWeight;
};

Instance loadInstance(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        throw std::runtime_error("Nie mogę otworzyć pliku: " + filename);
    }

    json j;
    file >> j;

    Instance inst;
    inst.size = j.at("size").get<int>();
    inst.optimum = j.at("optimum").get<int>();
    inst.matrix = j.at("matrix").get<std::vector<std::vector<int>>>();

    // --- obliczenie średniej wagi krawędzi ---
    long long sum = 0;
    long long cnt = 0;

    for (int i = 0; i < inst.size; i++) {
        for (int j = 0; j < inst.size; j++) {

            int w = inst.matrix[i][j];

            // pomijamy zera = brak krawędzi
            if (w > 0) {
                sum += w;
                cnt++;
            }
        }
    }

    if (cnt == 0) {
        throw std::runtime_error("Instancja nie zawiera żadnych krawędzi > 0.");
    }

    inst.avgWeight = static_cast<double>(sum) / cnt;

    return inst;
}

vector<vector<int>> readFromTxtList(const string &filename) {
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

vector<vector<int>> readFromTxtMatrix(const string &filename) {
    vector<vector<int>> vec;
    ifstream file(filename);

    if(file.is_open()) {
        int size;
        file >> size;
        vector<int> row;

        row.push_back(size);
        vec.push_back(row);

        for(int i = 0; i < size; i ++) {
            vector<int> row;
            for(int j = 0; j < size; j++) {
                int num;
                file >> num;
                row.push_back(num);
            }
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

void saveResults(const map<string, vector<int>> &res_map, const string &filename) {
    ofstream file(filename);

    if(file.is_open()) {
        for (const auto& x : res_map) {
            file << x.first << " ";
            vector<int> res = x.second;
            for (const auto num : res) {
                file << num << " ";
            }
            file << endl;
        }
        file.close();
        cout << "Zapisano do pliku: " << filename << endl;
    }
    else {
        cerr << "Nie udało się otworzyć pliku." << endl;
    }
}

template <typename T> void saveVecToTxt(const vector<T> &vec, const string &filename) {
    ofstream file(filename);

    if(file.is_open()) {
        for (const auto& number : vec) {
            file << number << endl;
        }
        file.close();
        cout << "Zapisano do pliku: " << filename << endl;
    }
    else {
        cerr << "Nie udało się otworzyć pliku." << endl;
    }
}

#endif