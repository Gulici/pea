
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include "utils.h"
#include "graph.h"
#include "tsp_bxb_dfs.h"
#include "tsp_bxb_bfs.h"
#include "tsp_bxb_best.h"

using namespace std;

bool DEBUG = true;
int MAX_TIME = 15;
std::map<std::string, int> graphs;

inline void readConfig();
inline int parse(std::string line);

int main() {

    auto start = std::chrono::high_resolution_clock::now();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;
    auto tms = std::chrono::duration_cast<std::chrono::milliseconds>(diff);

    readConfig();

    std::vector<int> dfs_wyniki;
    std::vector<int> bfs_wyniki;
    std::vector<int> best_wyniki;

    for(auto& gfile : graphs) {
        string filename = "data2/";
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

        int time;

        // Tsp_bxb_dfs dfs = Tsp_bxb_dfs(g, DEBUG, MAX_TIME);
        // time =  dfs.solve();
        // std::cout << "Czas: " << time << std::endl;
        // dfs_wyniki.push_back(size);
        // dfs_wyniki.push_back(time);
        // dfs_wyniki.push_back(dfs.getBestRes());


        Tsp_bxb_bfs bfs = Tsp_bxb_bfs(g, DEBUG, MAX_TIME);
        time = bfs.solve();
        std::cout << "Czas: " << time << std::endl;
        bfs_wyniki.push_back(size);
        bfs_wyniki.push_back(time);
        bfs_wyniki.push_back(bfs.getBestRes());
        
        // Tsp_bxb_best best = Tsp_bxb_best(g, DEBUG, MAX_TIME);
        // time = best.solve();
        // std::cout << "Czas: " << time << std::endl;
        // best_wyniki.push_back(size);
        // best_wyniki.push_back(time);
        // best_wyniki.push_back(best.getBestRes());
    }

    // saveVecToTxt(dfs_wyniki, "dfs_wyniki.txt");
    saveVecToTxt(bfs_wyniki, "bfs_wyniki.txt");
    // saveVecToTxt(best_wyniki, "best_wyniki.txt");

    return 0;
}

inline void readConfig() {
    ifstream file("config.txt");

    if(file.is_open()) {
        std::string line;
        std::getline(file, line);
        
        if(!parse(line)) DEBUG = false;
        std::getline(file, line);
        MAX_TIME = parse(line);

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