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

class Tsp_sa 
{
    int gSize;
    std::vector<vector<int>> neightmatrix;
    bool DEBUG;
    bool geo;
    bool lconst;

    double Tmax, Tmin;
    double tmulti;              // wspolczynnik Tmax
    double alfa;                // wspolczynnik chlodzenia geometrycznego
    int L = 100;                // liczba transformacji dla Tk
    std::vector<int> solution;  // znalezione rozwiazanie

    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::chrono::duration<double> diff;
    std::chrono::minutes maxtime = std::chrono::minutes(1);
    bool overrun = false;

    public:
    int result = 0;
    Tsp_sa(Graph& g, bool debug, int max_time, double alfa, double tmulti, int lmulti, bool lconst, bool geo, double tmin);
    int solve();
    std::vector<int> startSolution();
    std::vector<int> neighSolution(const std::vector<int>& path);
    inline std::vector<int> neighSolutionSwap(std::vector<int> current);
    int calDistance(std::vector<int> path);
    bool isLegalRoute(const std::vector<int>& p);
};

Tsp_sa::Tsp_sa(Graph& g, bool debug, int max_time, double alfa, double tmulti, int lmulti, bool lconst, bool geo, double tmin) 
{
    gSize = g.getSize();
    neightmatrix = g.neighMatrix();
    DEBUG = debug;
    this->lconst = lconst;
    maxtime = std::chrono::minutes(max_time);

    //wyznaczenie temperatury maksymalnej
    // Tmax = g.avgWeight * tmulti;
    Tmax = gSize * tmulti;
    // wyznaczenie temperatury minimalnej
    Tmin = tmin;
    // wyznaczenie początkowej ilości powtórzeń
    L = gSize * lmulti;

    assert (alfa > 0 && alfa < 1);
    this->alfa = alfa;
}

int Tsp_sa::solve() 
{   
    // wyznaczenie poczatkowego rozwiazania
    solution = startSolution();
    int solDist = calDistance(solution);
    int Lk = L;
    int Lmin = L * 0.1;
    double T = Tmax;

    start = std::chrono::system_clock::now();
    std::srand((unsigned)time(NULL));

    int k = 1;
    while(T >= Tmin && !overrun) {
        for(int i = 0; i < Lk; i++) {
            std::vector<int> n = neighSolution(solution);
            int nDist = calDistance(n);
            // przyjmujemy mniejsze rozwiazanie jesli jego koszt jest mniejszy
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
        // obnizenie liczby iteracji
        if(!lconst){
            if(Lk > Lmin) Lk *= alfa;
            if(Lk < Lmin) Lk = Lmin;
        }

        k++; 
    }

    end = std::chrono::system_clock::now();
    diff = end - start;
    auto tms = std::chrono::duration_cast<std::chrono::milliseconds>(diff);

    std::cout << "SA\nNajlepszy znaleziony cykl:";
    for(int v : solution) std::cout << v << " ";
    std::cout << " Koszt: " << solDist << endl;
    result = solDist;

    return tms.count();
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