
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include "utils.h"
#include "graph.h"
#include "sa.h"
#include "libs\json.hpp"

using json = nlohmann::json;
using namespace std;

bool DEBUG = true;
int MAX_TIME = 15;
double alfa = 0.9;
double tmulti = 100;
double lmulti = 100;
double tmin = 1;
bool lconst = true;
bool geo = true;
std::vector<string> graphs;

inline void readConfig();
inline void readConfigJson(const std::string& path);
inline int parseInt(std::string line);
inline double parseDouble(std::string line);

int main() {

    auto start = std::chrono::high_resolution_clock::now();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;
    auto tms = std::chrono::duration_cast<std::chrono::milliseconds>(diff);

    map<string, vector<int>> results;

    readConfigJson("./configs/config1.json");

    for(auto& gfile : graphs) {
        string filename = "data_tsp/";
        filename.append(gfile);

        Instance instance = loadInstance(filename);

        Graph g = Graph(instance);

        vector<vector<int>> matrix = g.getMatrix();
        int size = g.getSize();

        cout << gfile << " Najlepsza znana wartosc:" << instance.optimum << "\n";
        if(size < 20) {
            for(int i = 0; i < size; i++) {
                for(int j = 0; j < size; j++) {
                    cout << matrix[i][j] << "\t";
                }
                cout << endl;
            }
        }

        int time;
        Tsp_sa sa = Tsp_sa(g, DEBUG, MAX_TIME,alfa,tmulti,lmulti,lconst, geo, tmin);
        time = sa.solve();
        std::cout << "Czas: " << time << std::endl;
        
        vector<int> res;
        res.push_back(time);
        res.push_back(sa.result);
        results[gfile] = res;
    }


    saveResults(results, "wyniki_sa.txt");

    return 0;
}

void readConfigJson(const std::string& path) 
{
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Nie udało się otworzyć: " << path << "\n";
        return;
    }

    json j;
    file >> j;

    DEBUG   = j.value("debug", false);
    MAX_TIME = j.value("max_time", 0);
    alfa     = j.value("alfa", 1.0);
    tmulti   = j.value("tmulti", 0.0);
    lmulti   = j.value("lmulti", 0.0);
    lconst   = j.value("lconst", false);
    tmin = j.value("tmin", 1.0);

    graphs.clear();
    for (const auto& item : j["graphs"]) {
        std::string filename = item.at("file").get<std::string>();
        graphs.push_back(filename);
    }
}

inline int parseInt(std::string line) {
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

inline double parseDouble(std::string line) {
    std::stringstream ss(line);
    std::string word;
    double number = 0;

    while (ss >> word) {
        try {
            number = std::stod(word);
            break; 
        } catch (const std::invalid_argument&) {
        }
    }
    return number;
}