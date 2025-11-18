#ifndef TSP_BFS
#define TSP_BFS

#include "graph.h"
#include <vector>
#include <unordered_map>
#include <list>
#include <chrono>
#include <algorithm>
#include "utils.h"
#include <iostream>
#include <queue>
#include <vector>
#include <set>
#include <climits>

using namespace std;

struct State {
    vector<int> path;       
    int cost;               
    int level;              // Poziom w drzewie przeszukiwania
    int index;
};

class Tsp_bxb_bfs {
    Graph *_g;
    int gSize;
    vector<vector<int>> graph;
    int bestRes = INT_MAX;
    std::vector<int> best_cycle;

    bool debug = true;

    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::chrono::duration<double> diff;
    std::chrono::minutes maxtime = std::chrono::minutes(1);
    bool overrun = false;

    public:
    Tsp_bxb_bfs(Graph& g, bool DEBUG, int MAX_TIME);
    int solve();
    vector<int> nearestNeighbourTSP();
    void bfs_cycle(int i);
    int getBestRes();
};

Tsp_bxb_bfs::Tsp_bxb_bfs(Graph& g, bool DEBUG, int MAX_TIME){
    this->_g = &g;
    debug = DEBUG;
    maxtime = std::chrono::minutes(MAX_TIME);
    gSize = g.getSize();
    graph = g.getMatrix();
}

inline vector<int> Tsp_bxb_bfs::nearestNeighbourTSP()
{
    vector<int> path;
    vector<bool> visited(gSize, false);
    int currentCity = 0;
    path.push_back(currentCity);
    visited[currentCity] = true;
    int totalCost = 0;

    for (int i = 1; i < gSize; ++i) {
        int nearest = -1;
        int minDistance = INT_MAX;

        for (int j = 0; j < gSize; ++j) {
            if (!visited[j] && graph[currentCity][j] >= 0 && graph[currentCity][j] < minDistance) {
                minDistance = graph[currentCity][j];
                nearest = j;
            }
        }

        if (nearest != -1) {
            path.push_back(nearest);
            visited[nearest] = true;
            totalCost += minDistance;
            currentCity = nearest;
        }
    }
    totalCost += graph[currentCity][path[0]];
    bestRes = totalCost;
    return path;
}

inline void Tsp_bxb_bfs::bfs_cycle(int si)
{
    State root = {{si}, 0, 0, si};
    queue<State> q;
    q.push(root);

    while (!q.empty()) {
        //sprawdzenie czy nie przekroczony maksymalny czas
        end = std::chrono::system_clock::now();
        diff = end - start;
        auto tm = std::chrono::duration_cast<std::chrono::minutes>(diff);
        if(tm >= maxtime) {
            overrun = true;
            return;
        }

        State current = q.front();
        q.pop();

        //czy obecny wierzcholek jest koncowy
        if (current.level == gSize - 1) {
            // current.path.push_back(i);
            // czy wierzcholek koncowy sasiaduje z poczatkowym
            int cost = graph[current.index][si];
            if(cost > 0) {
                current.cost += cost;
                if (current.cost < bestRes) {
                    bestRes = current.cost;
                    best_cycle = current.path;
                }
            }
        } else {
            // dodaj sasiadow do kolejki
            for (int i = 0; i < gSize; ++i) {
                int cost = graph[current.index][i];
                if(cost > 0 && find(current.path.begin(),current.path.end(),i) == current.path.end()) {
                    State next;
                    next.path = current.path;
                    next.path.push_back(i);
                    next.level = current.level + 1;
                    next.cost = current.cost + cost;
                    next.index = i;

                    // jesli laczny koszt mniejszy niz obecnie najlepszy to dodaj do kolejki
                    if (next.cost < bestRes) {
                        q.push(next);
                    }
                }
            }
        }
    }
}

inline int Tsp_bxb_bfs::getBestRes()
{
    return bestRes;
}

inline int Tsp_bxb_bfs::solve() 
{   
    int bestKnow = _g->getBestKnow();
    int iterCount = 0;

    vector<int> best_cycle = nearestNeighbourTSP();

    start = std::chrono::system_clock::now();
    

    for(int i = 0; i < gSize; i ++) {
        // zakoncz jesli odnaleziono optymalne rozwiazanie
        if(bestRes == bestKnow) {
            break;
        }

        bfs_cycle(i);
        
        //zakoncz szukanie rozwiazan jesli przekroczono czas
        if(overrun) break;

        iterCount++;

        //print debug
        if(debug && (iterCount % 1 == 0)) {
            std::cout << iterCount << "/" << gSize << " best yet: " << bestRes << endl;
        }
    }

    end = std::chrono::system_clock::now();
    diff = end - start;
    auto tms = std::chrono::duration_cast<std::chrono::milliseconds>(diff);

    cout << "BxB BFS\nNajlepszy znaleziony cykl:";
    for(int v : best_cycle) cout << v << " ";
    cout << " Koszt: " << bestRes << endl;

    return tms.count();
}

#endif