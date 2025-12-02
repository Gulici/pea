
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include "utils.h"
#include "graph.h"
#include "sa.h"
#include "aco.h"
#include "libs\json.hpp"

using json = nlohmann::json;
using namespace std;

bool DEBUG = true;
int MAX_TIME = 15;

string METHOD = "SA";

// SA parametry
double alfa = 0.9;
double tmax = 100;
double tmulti = 100;
double lmulti = 100;
double tmin = 1;
bool tconst = true;
bool geo = true;

// ACO parametry
double ACO_ITER = 0;
double ACO_ALPHA = 0;
double ACO_BETA = 0;
double ACO_EVAP = 0;
double ACO_INIT_PHER = 0;
bool ACO_USE_CAS = false;

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

    for (auto& gfile : graphs)
    {
        string filename = "data_tsp/";
        filename.append(gfile);

        Instance instance = loadInstance(filename);
        Graph g(instance);

        cout << gfile << " Najlepsza znana wartosc:" << instance.optimum << "\n";

        if (METHOD == "SA")
        {
            Tsp_sa sa(g, DEBUG, MAX_TIME, alfa, tmax, tmulti, lmulti, tconst, geo, tmin, gfile);

            int time = sa.solve();
            cout << "Czas SA: " << time << endl;

            vector<int> res;
            res.push_back(time);
            res.push_back(sa.result);
            results[gfile] = res;
        }
        else if (METHOD == "ACO")
        {
            AntColonyTSP aco(
                g,
                g.getSize(),
                ACO_ITER,
                ACO_ALPHA,
                ACO_BETA,
                ACO_EVAP,
                ACO_INIT_PHER,
                ACO_USE_CAS,
                MAX_TIME / 60.0,
                gfile
            );

            int time = aco.solve();
        }
        else
        {
            cerr << "Nieznana metoda w configu: " << METHOD << "\n";
        }
    }


    if (METHOD == "SA")
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

    METHOD   = j.value("method", "SA");

    DEBUG    = j.value("debug", false);
    geo      = j.value("geo", false);
    MAX_TIME = j.value("max_time", 0);

    // SA parametry
    alfa     = j.value("alfa", 1.0);
    tmax   = j.value("tmax", 0.0);
    lmulti   = j.value("lmulti", 0.0);
    tmulti   = j.value("tmulti", 0.0);
    tconst   = j.value("tconst", false);
    tmin     = j.value("tmin", 1.0);

    // ACO parametry
    ACO_ITER       = j.value("aco_iterations", 1000.0);
    ACO_ALPHA      = j.value("aco_alpha", 1.0);
    ACO_BETA       = j.value("aco_beta", 5.0);
    ACO_EVAP       = j.value("aco_evap", 0.5);
    ACO_INIT_PHER  = j.value("aco_initial_pheromone", 0.001);
    ACO_USE_CAS    = j.value("aco_use_cas", true);

    graphs.clear();
    for (const auto& item : j["graphs"])
        graphs.push_back(item.at("file").get<std::string>());
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