#ifndef SA
#define SA

#include "graph.h"
#include <vector>
#include <assert.h>
#include <unordered_map>
#include <list>
#include <chrono>
#include "utils.h"
#include <math.h>
#include <fstream>
#include <string>
#include <iomanip>

class Tsp_sa 
{
    int gSize;
    std::vector<vector<int>> neightmatrix;
    bool DEBUG;
    bool geo;
    bool lconst;

    double Tmax, Tmin;
    double alfa;                // wspolczynnik chlodzenia geometrycznego
    int L = 100;                // liczba transformacji dla Tk
    std::vector<int> solution;  // znalezione rozwiazanie

    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::chrono::duration<double> diff;
    std::chrono::minutes maxtime = std::chrono::minutes(1);
    bool overrun = false;

    int optimum = -1;
    std::string instanceName;

    public:
    int result = 0;
    Tsp_sa(Graph& g, bool debug, int max_time, double alfa, double tmax, int tmulti, int lmulti, bool tconst, bool geo, double tmin, string instanceName);
    int solve();
    std::vector<int> startSolution();
    std::vector<int> neighSolution(const std::vector<int>& path);
    inline std::vector<int> neighSolutionSwap(std::vector<int> current);
    int calDistance(std::vector<int> path);
    bool isLegalRoute(const std::vector<int>& p);
};

Tsp_sa::Tsp_sa(Graph& g, bool debug, int max_time, double alfa, double tmax, int tmulti, int lmulti, bool tconst, bool geo, double tmin, string instanceName) 
{
    gSize = g.getSize();
    neightmatrix = g.neighMatrix();
    DEBUG = debug;
    this->lconst = lconst;
    maxtime = std::chrono::minutes(max_time);
    this->optimum = g.getBestKnow();
    this->geo = geo;

    this->instanceName = instanceName;

    // wyznaczenie temperatury maksymalnej
    Tmax = tmax;
    if(!tconst) {
        Tmax = gSize * tmulti;
    }
    // wyznaczenie temperatury minimalnej
    Tmin = tmin;
    // wyznaczenie początkowej ilości powtórzeń
    L = gSize * lmulti;

    assert (alfa > 0 && alfa < 1);
    this->alfa = alfa;
}

// pomocnicza funkcja do sformatowanego zapisu błędu (np. 0.0123 -> 1.23%)
static inline std::string format_percent(double v) {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(4) << (v * 100.0) << "%";
    return ss.str();
}

int Tsp_sa::solve() 
{   
    // wyznaczenie poczatkowego rozwiazania
    solution = startSolution();
    int solDist = calDistance(solution);
    int Lk = L;
    int Lmin = static_cast<int>(L * 0.1);
    double T = Tmax;

    start = std::chrono::system_clock::now();
    std::srand((unsigned)time(NULL));

    int k = 1;
    while(T >= Tmin && !overrun) {
        for(int i = 0; i < Lk; i++) {
            std::vector<int> n = neighSolution(solution);
            int nDist = calDistance(n);
            // przyjmujemy mniejsze rozwiazanie
            if(nDist <= solDist) {
                solution = n;
                solDist = nDist;
            }
            else {
                // obliczenie prawdopodobienstwa wybrania rozwiazania o wiekszym koszcie
                // p tym mniejsze im mniejsza temperatura i im wieksza roznica kosztow
                double p = std::exp((double)(solDist - nDist)/T);
                double r = (double) rand() / RAND_MAX;

                if(p > r) {
                    solution = n;
                    solDist = nDist;
                }
            }

            //sprawdzenie czy nie przekroczono maksymalnego czasu
            end = std::chrono::system_clock::now();
            diff = end - start;
            auto tm = std::chrono::duration_cast<std::chrono::minutes>(diff);
            if(tm >= maxtime) {
                overrun = true;
                break;
            }
        }
        // obnizenie temperatury
        if (geo) T *= alfa;
        else T *= pow(alfa, k);

        k++;
    }

    end = std::chrono::system_clock::now();
    diff = end - start;
    auto tms = std::chrono::duration_cast<std::chrono::milliseconds>(diff);
    long long elapsed_ms = tms.count();

    std::cout << "SA\nNajlepszy znaleziony cykl:";
    for(int v : solution) std::cout << v << " ";
    std::cout << " Koszt: " << solDist << std::endl;
    result = solDist;

    // ---------------------------
    // obliczenie błędu względnego i zapis do CSV
    // ---------------------------
    double rel_err = 0.0;
    if (optimum > 0 && solDist < std::numeric_limits<int>::max()/4) {
        rel_err = (static_cast<double>(solDist) - static_cast<double>(optimum)) / static_cast<double>(optimum);
    }

    // zapisz do pliku CSV (doklej)
    std::string outCsv = "wyniki_sa.csv";
    std::ofstream f(outCsv, std::ios::app);
    if (f) {
        // kolumny: instance,n,method,best_cost,rel_err,alpha,tmulti,lmulti,lconst,tmin,time_ms
        f << (instanceName.empty() ? "unknown" : instanceName) << ","
          << gSize << ","
          << optimum << ","
          << (geo ? "Geo" : "Geo2") << ","
          << solDist << ","
          << rel_err << ","
          << alfa << ","
          << L << ","
          << (lconst ? "true" : "false") << ","
          << Tmin << ","
          << Tmax << ","
          << elapsed_ms
          << "\n";
    }

    std::cout << "=== WYNIK SA ===\n"
            << "Instancja:        " << (instanceName.empty() ? "unknown" : instanceName) << "\n"
            << "Liczba miast:     " << gSize << "\n"
            << "Schemat chlodz.:  " << (geo ? "Geo" : "Geo2") << "\n"
            << "Najlepszy koszt:  " << solDist << "\n"
            << "Optimum:          " << (optimum >= 0 ? std::to_string(optimum) : std::string("brak")) << "\n"
            << "Blad wzgledny:    " << std::fixed << std::setprecision(6) << rel_err << "  (" << format_percent(rel_err) << ")\n"
            << "Alfa (chlodzenie): " << alfa << "\n"
            << "Tmax (start):     " << Tmax << "\n"
            << "Tmin (stop):      " << Tmin << "\n"
            << "L:                " << L << "\n"
            << "Czas [ms]:        " << elapsed_ms << "\n"
            << "=================\n\n";

    return static_cast<int>(elapsed_ms);
}

// rozwiazanie dla grafow pelnych
inline std::vector<int> Tsp_sa::startSolution()
{   
    vector<int> path;
    vector<bool> visited(gSize, false);
    int currentCity = 0;
    path.push_back(currentCity);
    visited[currentCity] = true;

    for (int i = 1; i < gSize; ++i) {
        int nearest = -1;
        int minDistance = INT_MAX;

        for (int j = 0; j < gSize; ++j) {
            int dist = neightmatrix[currentCity][j];
            if (!visited[j] && dist > 0 && dist < minDistance) {
                minDistance = dist;
                nearest = j;
            }
        }

        if (nearest != -1) {
            path.push_back(nearest);
            visited[nearest] = true;
            currentCity = nearest;
        }
    }
    return path;
}

bool Tsp_sa::isLegalRoute(const std::vector<int>& p)
{
    for (int i = 0; i < gSize - 1; i++) {
        if (neightmatrix[p[i]][p[i+1]] == 0)
            return false;
    }
    // Powrót do startu (pełny cykl)
    if (neightmatrix[p[gSize-1]][p[0]] == 0)
        return false;

    return true;
}

inline std::vector<int> Tsp_sa::neighSolutionSwap(std::vector<int> current)
{   
    const int MAX_TRIES = 50;
    for (int attempt = 0; attempt < MAX_TRIES; attempt++) {
        std::vector<int> path = current;
        int v1 = random(0,gSize-1);
        int v2 = random(0,gSize-1);
        while(v1 == v2) v2 = random(0,gSize-1);
        
        std::swap(path[v1], path[v2]);
        if (!isLegalRoute(path))
            continue;
        return path;
    }
    return current;
}


inline std::vector<int> Tsp_sa::neighSolution(const std::vector<int>& current)
{
    // Maksymalna liczba prób, aby uniknąć zapętlenia
    const int MAX_TRIES = 50;
    for (int attempt = 0; attempt < MAX_TRIES; attempt++)
    {
        std::vector<int> path = current;

        int i = random(0, gSize - 2);       // i w [0, n-2]
        int k = random(i + 1, gSize - 1);   // k w [i+1, n-1]

        std::reverse(path.begin() + i + 1, path.begin() + k + 1);

        if (!isLegalRoute(path))
            continue;

        return path;
    }

    // Jeśli nie znaleziono legalnego ruchu, zwracamy wejście bez zmian
    return current;
}

inline int Tsp_sa::calDistance(std::vector<int> path)
{   
    int cost = 0;
    for (int i = 0; i < gSize-1; i++) {
        cost += neightmatrix[path[i]][path[i+1]];
    }
    cost += neightmatrix[path[gSize-1]][path[0]];
    return cost;
}

#endif
