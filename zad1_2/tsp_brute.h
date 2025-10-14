#ifndef TSP_B
#define TSP_B

#include "graph.h"
#include <vector>
#include <unordered_map>
#include <list>
#include <chrono>
#include "utils.h"

class Tsp_b {
    Graph *_g;
    int gSize;
    int *visited;
    int visited_count = 0;
    bool found = false;
    int si;
    int bestRes = INT_MAX;
    std::list<int> res_cycle;
    std::list<int> best_cycle;

    std::vector<Node> nodes;

    bool debug = true;
    int cycle_res_sum;

    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::chrono::duration<double> diff;
    std::chrono::minutes maxtime = std::chrono::minutes(1);
    bool overrun = false;

    public:
    Tsp_b(Graph& g, bool debug, int MAX_TIME);
    int solve();
    void hamil_cycle_step(int vi);
    int getBestRes();
};

Tsp_b::Tsp_b(Graph& g, bool DEBUG, int MAX_TIME) {
    this->_g = &g;
    debug = DEBUG;
    maxtime = std::chrono::minutes(MAX_TIME);
    gSize = g.getSize();
    nodes = g.getNodes();
}

inline int Tsp_b::solve() {
    int bestKnow = _g->getBestKnow();
    int iterCount = 0;
    start = std::chrono::system_clock::now();
    

    for(int i = 0; i < gSize; i ++) {
        si = i;
        visited_count++;

        // zakoncz jesli odnaleziono optymalne rozwiazanie
        if(bestRes == bestKnow) {
            break;
        }

        visited = new int[gSize];
        for(int i = 0; i < gSize; i++) {
            visited[i] = 0;
        }

        hamil_cycle_step(si);
        
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

    cout << "Brute-force\nNajlepszy znaleziony cykl:";
    for(int v : best_cycle) cout << v << " ";
    cout << " Koszt: " << bestRes << endl;

    return tms.count();
}

inline void Tsp_b::hamil_cycle_step(int vi)
{   
    //sprawdzenie czy nie przekroczony maksymalny czas
    end = std::chrono::system_clock::now();
    diff = end - start;
    auto tm = std::chrono::duration_cast<std::chrono::minutes>(diff);
    // jesli tak to wyjdz z rekurencji
    if(tm >= maxtime) {
        overrun = true;
        return;
    }

    Node &currentNode = nodes[vi];
    const std::vector<vector<int>> neightmatrix = _g->matrix;
    std::vector<int> v_neighs;
    std::vector<int> v_row = neightmatrix.at(vi);

    int ni = si;

    //vi neight from matrix
    for(int i = 0; i < gSize; i++) {
        int w = v_row.at(i);
        if(w > 0){
            v_neighs.push_back(i);
        } 
    }

    for(int ni : v_neighs) {
        if(visited[ni] == 0 && ni != si) {
            Node &nextNode = nodes[ni];
            nextNode.weight = currentNode.weight + v_row[ni];
            nextNode.parentIndex = vi;

            visited[ni] = 1;
            visited_count++;
            hamil_cycle_step(ni);
            if(overrun) return;
        }
        else if (visited_count == gSize && ni == si) {
            //hamil cycle found
            int cycle_cost = currentNode.weight + v_row[ni];
            if (cycle_cost < bestRes) {
                bestRes = cycle_cost;
                best_cycle.clear();
                
                int btnode = vi;
                int pi = nodes[vi].parentIndex;
                while(pi != -1) {
                    best_cycle.push_front(btnode);
                    btnode = pi;
                    pi = nodes[btnode].parentIndex;
                }
                best_cycle.push_front(btnode);

                if(debug) {
                    for( int v : best_cycle) cout << v << " ";
                    cout << "koszt: " << cycle_cost << "\n";
                }
            }
        }
    }
    
    visited_count--;
    visited[vi] = 0;
    currentNode.weight = 0;
    currentNode.parentIndex = -1;
}

inline int Tsp_b::getBestRes()
{
    return bestRes;
}

#endif