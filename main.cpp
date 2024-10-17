
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include "utils.h"
#include "graph.h"
#include "tsp_random.h"
#include "tsp_nn.h"
#include "tsp_brute.h"

using namespace std;

bool DEBUG = true;
int MAX_ITER = 1000;
std::map<std::string, int> graphs;

inline void readConfig();
inline int parse(std::string line);

int main() {

    auto start = std::chrono::high_resolution_clock::now();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;
    auto tms = std::chrono::duration_cast<std::chrono::milliseconds>(diff);

    readConfig();

    for(auto& gfile : graphs) {
        string filename = "data/";
        filename.append(gfile.first);
        Graph g = Graph(filename, gfile.second);

        vector<vector<int>> matrix = g.getMatrix();
        int size = g.getSize();

        cout << gfile.first << " Najlepsza znana wartość:" << gfile.second << "\n";
        for(int i = 0; i < size; i++) {
            for(int j = 0; j < size; j++) {
                cout << matrix[i][j] << "\t";
            }
            cout << endl;
        }

        Tsp_random rnd = Tsp_random(g, DEBUG, MAX_ITER);
        Tsp_nn nn = Tsp_nn(g, DEBUG, MAX_ITER);
        Tsp_b brut = Tsp_b(g, DEBUG, MAX_ITER);

        start = std::chrono::high_resolution_clock::now();
        rnd.solve();
        end = std::chrono::high_resolution_clock::now();
        diff = end - start;
        tms = std::chrono::duration_cast<std::chrono::milliseconds>(diff);
        std::cout << "Czas: " << tms.count() << std::endl;

        start = std::chrono::high_resolution_clock::now();
        nn.solve();
        end = std::chrono::high_resolution_clock::now();
        diff = end - start;
        tms = std::chrono::duration_cast<std::chrono::milliseconds>(diff);
        std::cout << "Czas: " << tms.count() << std::endl;

        start = std::chrono::high_resolution_clock::now();
        brut.solve();
        end = std::chrono::high_resolution_clock::now();
        diff = end - start;
        tms = std::chrono::duration_cast<std::chrono::milliseconds>(diff);
        std::cout << "Czas: " << tms.count() << std::endl;
    }

    return 0;
}

inline void readConfig() {
    ifstream file("config.txt");

    if(file.is_open()) {
        std::string line;
        std::getline(file, line);
        
        if(!parse(line)) DEBUG = false;
        std::getline(file, line);
        MAX_ITER = parse(line);

        std::getline(file, line);
        int fileNum = parse(line);

        for(int i = 0; i < fileNum; i++) {
            std::getline(file, line);
            std::string filename;
            int cost;
            std::stringstream ss(line);
            ss >> filename;
            ss >> cost;
            graphs[filename] = cost;
        }

        file.close();
    }
    else {
        std::cerr << "Nie udało się otworzyć pliku konfiguracyjnego\n";
    } 
}

inline int parse(std::string line) {
    std::stringstream ss(line);
    std::string word;
    int number = 0;

    while (ss >> word) {
        try {
            number = std::stoi(word);
            break; 
        } catch (const std::invalid_argument&) {
        }
    }
    return number;
}