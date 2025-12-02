#ifndef ANT_COLONY_H
#define ANT_COLONY_H

#include <vector>
#include <cmath>
#include <limits>
#include <chrono>
#include <stdexcept>
#include <iostream>
#include <random>
#include <fstream>
#include <string>
#include <algorithm>

#include "graph.h"

using std::string;

class AntColonyTSP
{
public:
    AntColonyTSP(
        Graph& graph,
        int numAnts,
        double maxIterations,
        double alpha,
        double beta,
        double evaporationRate,
        double initialPheromone,
        bool useCAS,
        double stopMinutes,
        string name
    );

    int solve();                                // zwraca czas wykonania [ms]
    const std::vector<int>& getBestPath() const { return bestPath; }
    int getBestCost() const { return bestCost; }

private:
    int numCities;
    std::vector<std::vector<int>> dist;          // macierz sąsiedztwa
    std::vector<std::vector<double>> pheromone;  // feromony
    std::vector<std::vector<double>> heuristic;  // 1 / dystans

    int numAnts;
    int optimum;
    double Q;
    double maxIterations;
    double alpha;
    double beta;
    double evaporationRate;
    double initialPheromone;
    bool useCAS;
    double stopTimeMilliseconds;
    string instanceName;

    std::vector<int> bestPath;
    int bestCost = std::numeric_limits<int>::max();

    std::mt19937 rng; // generator liczb losowych

    // granice feromonu
    double minPheromone = 1e-6;
    double maxPheromone = 1e6;

    std::vector<int> constructSolution(int antIndex);
    int computeCost(const std::vector<int>& path);
    int chooseNextCity(int current, const std::vector<bool>& visited);

    inline double probabilityWeight(int from, int to)
    {
        double t = pheromone[from][to];
        double h = heuristic[from][to];
        if (t <= 0.0) return 0.0;
        if (h <= 0.0) return std::pow(t, alpha);
        return std::pow(t, alpha) * std::pow(h, beta);
    }
};


// ============================================================
//                      KONSTRUKTOR ACO
// ============================================================

AntColonyTSP::AntColonyTSP(
    Graph& graph,
    int numAnts,
    double maxIterations,
    double alpha,
    double beta,
    double evaporationRate,
    double initialPheromone,
    bool useCAS,
    double stopMinutes,
    string name)
    :
    numAnts(numAnts),
    maxIterations(maxIterations),
    alpha(alpha),
    beta(beta),
    evaporationRate(evaporationRate),
    initialPheromone(initialPheromone),
    useCAS(useCAS),
    rng(std::random_device{}())
{
    numCities = graph.getSize();
    dist = graph.neighMatrix();
    this->optimum = graph.getBestKnow();
    this->instanceName = name;

    Q = optimum;

    if (numCities == 0)
        throw std::invalid_argument("Graf jest pusty.");

    // sprawdź czy jakiekolwiek miasto ma przynajmniej jedną krawędź
    for (int i = 0; i < numCities; i++)
    {
        bool ok = false;
        for (int j = 0; j < numCities; j++)
            if (dist[i][j] > 0) ok = true;
        if (!ok)
            throw std::invalid_argument("Graf zawiera miasto bez żadnych krawędzi.");
    }

    // przelicz limit czasu
    stopTimeMilliseconds = stopMinutes * 60000.0;

    // inicjalizacja feromonów
    pheromone.assign(numCities, std::vector<double>(numCities, initialPheromone));

    // obliczenie macierzy heurystycznej (1/dystans)
    heuristic.assign(numCities, std::vector<double>(numCities, 0.0));
    for (int i = 0; i < numCities; i++)
    {
        for (int j = 0; j < numCities; j++)
        {
            int d = dist[i][j];
            heuristic[i][j] = (i != j && d > 0) ? 1.0 / static_cast<double>(d) : 0.0;
        }
    }
}


// ============================================================
//      WYBÓR KOLEJNEGO MIASTA – wersja probabilistyczna
//      (std::discrete_distribution)
// ============================================================

int AntColonyTSP::chooseNextCity(int current, const std::vector<bool>& visited)
{
    // zbierz indeksy dozwolone i odpowiadające im wagi
    std::vector<double> weights;
    std::vector<int> indices;
    weights.reserve(numCities);
    indices.reserve(numCities);

    for (int j = 0; j < numCities; j++)
    {
        if (!visited[j] && dist[current][j] > 0)
        {
            double w = probabilityWeight(current, j);
            // jeśli waga 0, pomiń (discrete_distribution by to obsłużyła, ale lepiej pominąć)
            if (w > 0.0) {
                weights.push_back(w);
                indices.push_back(j);
            }
        }
    }

    if (weights.empty()) {
        // brak wag > 0: spróbuj wybrać dowolne legalne przejście równomiernie
        std::vector<int> feasible;
        for (int j = 0; j < numCities; ++j) {
            if (!visited[j] && dist[current][j] > 0) feasible.push_back(j);
        }
        if (feasible.empty()) return -1;
        std::uniform_int_distribution<int> ud(0, static_cast<int>(feasible.size()) - 1);
        return feasible[ud(rng)];
    }

    std::discrete_distribution<int> dd(weights.begin(), weights.end());
    int sel = dd(rng);
    return indices[sel];
}


// ============================================================
//             KONSTRUKCJA PEŁNEGO CYKLU DLA MRÓWKI
// ============================================================

std::vector<int> AntColonyTSP::constructSolution(int /*antIndex*/)
{
    std::vector<int> path;
    path.resize(numCities);
    std::vector<bool> visited(numCities, false);

    // losowy punkt startu (różnicuje mrówki)
    std::uniform_int_distribution<int> distInt(0, numCities - 1);
    int current = distInt(rng);
    path[0] = current;
    visited[current] = true;

    for (int step = 1; step < numCities; step++)
    {
        int next = chooseNextCity(current, visited);
        if (next == -1)
        {
            // nieudana ścieżka: zwracamy pusty wektor jako sygnał błędu
            return std::vector<int>();
        }

        // DAS: gęstościowy → Δτ = Q (stała wartość)
        if (!useCAS)
        {
            pheromone[current][next] = std::min(pheromone[current][next] + Q, maxPheromone);
            pheromone[next][current] = pheromone[current][next];
        }

        path[step] = next;
        visited[next] = true;
        current = next;
    }

    // sprawdź domknięcie cyklu (edge z powrotem do startu)
    if (dist[path.back()][path.front()] == 0) {
        // brak krawędzi powrotnej — traktujemy jako nieudane
        return std::vector<int>();
    }

    return path;
}


// ============================================================
//                OBLICZANIE KOSZTU CYKLU
// ============================================================

int AntColonyTSP::computeCost(const std::vector<int>& p)
{
    if (p.empty()) return std::numeric_limits<int>::max() / 4;

    long long cost = 0;
    for (int i = 0; i < numCities - 1; i++) {
        int u = p[i], v = p[i+1];
        cost += dist[u][v];
    }
    cost += dist[p.back()][p[0]]; // domknięcie cyklu

    if (cost > std::numeric_limits<int>::max() / 8) return std::numeric_limits<int>::max() / 4;
    return static_cast<int>(cost);
}


// ============================================================
//                             ACO
// ============================================================

int AntColonyTSP::solve()
{
    auto start = std::chrono::high_resolution_clock::now();

    bestPath.clear();
    bestCost = std::numeric_limits<int>::max();

    // prealokacja struktur (unikanie realokacji w pętli)
    std::vector<std::vector<int>> ants(numAnts);
    std::vector<int> costs(numAnts);

    for (int iter = 0; iter < static_cast<int>(maxIterations); iter++)
    {
        // limit czasu
        auto now = std::chrono::high_resolution_clock::now();
        double elapsed_check = std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
        if (elapsed_check >= stopTimeMilliseconds)
            break;

        // konstrukcja rozwiązań
        for (int k = 0; k < numAnts; k++)
        {
            ants[k] = constructSolution(k);

            if (ants[k].empty()) {
                costs[k] = std::numeric_limits<int>::max() / 4; // oznacz nieudane
            } else {
                costs[k] = computeCost(ants[k]);
                if (costs[k] < bestCost)
                {
                    bestCost = costs[k];
                    bestPath = ants[k];
                }
            }
        }

        // parowanie feromonu (evaporation)
        for (int i = 0; i < numCities; i++) {
            for (int j = 0; j < numCities; j++) {
                pheromone[i][j] *= (1.0 - evaporationRate);
                if (pheromone[i][j] < minPheromone) pheromone[i][j] = minPheromone;
                if (pheromone[i][j] > maxPheromone) pheromone[i][j] = maxPheromone;
            }
        }

        // aktualizacja feromonu CAS
        if (useCAS)
        {
            for (int k = 0; k < numAnts; k++)
            {
                if (costs[k] <= 0 || costs[k] >= std::numeric_limits<int>::max() / 4)
                    continue;

                double add = Q / static_cast<double>(costs[k]); // CAS: Q/Lk
                const auto &p = ants[k];

                for (int e = 0; e < numCities - 1; ++e) {
                    int a = p[e], b = p[e + 1];
                    pheromone[a][b] = std::min(pheromone[a][b] + add, maxPheromone);
                    pheromone[b][a] = pheromone[a][b];
                }
                int a = p.back(), b = p.front();
                pheromone[a][b] = std::min(pheromone[a][b] + add, maxPheromone);
                pheromone[b][a] = pheromone[a][b];
            }
        }
    } // koniec pętli iteracji

    // zakończenie pomiaru czasu
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    // zapis wyniku (pojedynczy wiersz)
    string outCsv = "wyniki_aco.csv";

    // obliczenie błędu względnego (bezpiecznie)
    double rel_err = 0.0;
    if (optimum > 0 && bestCost < std::numeric_limits<int>::max() / 4) {
        rel_err = (static_cast<double>(bestCost) - static_cast<double>(optimum)) / static_cast<double>(optimum);
    }

    std::ofstream f(outCsv, std::ios::app);
    if (f) {
        f << instanceName << ","
          << numCities << ","
            << optimum << ","
          << numAnts << ","
          << maxIterations << ","
          << (useCAS ? "CAS" : "DAS") << ","
          << bestCost << ","
          << rel_err << ","
          << alpha << ","
          << beta << ","
          << evaporationRate << ","
          << elapsed
          << "\n";
    }

    // --- czytelny output na konsolę ---
    std::cout << "=== WYNIK ACO ===\n"
            << "Instancja:          " << instanceName << "\n"
            << "Liczba miast:       " << numCities << "\n"
            << "Reguła aktualizacji:" << (useCAS ? "CAS" : "DAS") << "\n"
            << "Najlepszy koszt:    " << bestCost << "\n"
            << "Optimum:            " << optimum << "\n"
            << "Błąd względny:      " << rel_err << "\n"
            << "Alpha:              " << alpha << "\n"
            << "Beta:               " << beta << "\n"
            << "Parowanie (rho):    " << evaporationRate << "\n"
            << "Liczba mrówek (m):  " << numAnts << "\n"
            << "Iteracje:           " << maxIterations << "\n"
            << "Czas [ms]:          " << elapsed << "\n"
            << "====================\n\n";

    return static_cast<int>(elapsed);
}

#endif
