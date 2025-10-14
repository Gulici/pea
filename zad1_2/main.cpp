
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

    std::vector<int> rand_wyniki;
    std::vector<int> nn_wyniki;
    std::vector<int> brut_wyniki;

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

        Tsp_random rnd = Tsp_random(g, DEBUG, MAX_TIME);
        Tsp_nn nn = Tsp_nn(g, DEBUG, MAX_TIME);
        Tsp_b brut = Tsp_b(g, DEBUG, MAX_TIME);

        int sum = 0;
        for(int i = 0; i < 10; i++) {
            time = rnd.solve();
            sum += time;
        }
        int avg = sum/10;
        std::cout << "Czas: " << avg << std::endl;
        rand_wyniki.push_back(size);
        rand_wyniki.push_back(avg);

        time = nn.solve();
        std::cout << "Czas: " << time << std::endl;
        nn_wyniki.push_back(size);
        nn_wyniki.push_back(time);
        nn_wyniki.push_back(nn.getBestRes());

        time = brut.solve();
        std::cout << "Czas: " << time << std::endl;
        brut_wyniki.push_back(size);
        brut_wyniki.push_back(time);
        brut_wyniki.push_back(brut.getBestRes());
        
    }

    saveVecToTxt(rand_wyniki, "rand_wyniki.txt");
    saveVecToTxt(nn_wyniki, "nn_wyniki.txt");
    saveVecToTxt(brut_wyniki, "brut_wyniki.txt");

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