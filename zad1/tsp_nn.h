#ifndef TSP_NN
#define TSP_NN

#include "graph.h"
#include <vector>
#include <unordered_map>
#include <list>
#include "utils.h"

class Tsp_nn {
    Graph *_g;
    int gSize;
    int *visited;
    int visited_count = 0;
    bool found = false;
    int si;
    std::list<int> res_cycle;
    std::list<int> best_cycle;

    int maxIter = 10000;
    bool debug = true;
    int cycle_res_sum;

    public:
    Tsp_nn(Graph& g, bool DEBUG, int MAX_ITER);
    void solve();
    void hamil_cycle(int si);
    void hamil_cycle_step(int vi);
    int nearest_neigh(int vi);
};

Tsp_nn::Tsp_nn(Graph& g, bool DEBUG, int MAX_ITER) {
    debug = DEBUG;
    maxIter = MAX_ITER;
    this->_g = &g;
    gSize = g.getSize();
}

inline void Tsp_nn::solve() {
    int bestRes = INT_MAX;
    int bestKnow = _g->getBestKnow();
    int iterCount = 0;

    for(int i = 0; i < gSize; i ++) {
        si = i;
        if(iterCount >= maxIter || bestRes == bestKnow) {
            break;
        }

        hamil_cycle(si);
        
        if(cycle_res_sum < bestRes) {
            bestRes = cycle_res_sum;
            best_cycle.clear();
            best_cycle.assign(best_cycle.begin(),best_cycle.end());
        }
        iterCount++;

        //print debug
        if(debug && (iterCount % 1 == 0)) {
            std::cout << iterCount << "/" << gSize << " best yet: " << bestRes << endl;
        }
    }

    //print result
    cout << "Nearest-neigh\nNajlepszy znaleziony cykl:";
    for(int v : res_cycle) cout << v << " ";
    cout << " Koszt: " << bestRes << endl;

}

inline void Tsp_nn::hamil_cycle(int si)
{
    cycle_res_sum = INT_MAX;
    res_cycle.clear();
    found = false;

    visited = new int[gSize];
    for(int i = 0; i < gSize; i++) {
        visited[i] = 0;
    }

    visited[si] = 1;
    visited_count++;
    hamil_cycle_step(si);

    if(found) {
        // std::reverse(res_cycle);
        // for(int v : res_cycle) std::cout << v << " ";
        // cout << " Koszt: " << cycle_res;
    }
    else std::cout << "nie znaleziono cyklu";
}

inline void Tsp_nn::hamil_cycle_step(int vi)
{
    //czy ma  nieodwiedzonych sasiadow?
    int ni = si;
    while(!found) {
        ni = nearest_neigh(vi);
        if(ni != -1) {
            visited[ni] = 1;
            visited_count++;
            hamil_cycle_step(ni);
        }
        else if (visited_count != gSize) {
            break;
        }
        else if (visited_count == gSize) {
            std::vector<int> v_row = _g->matrix.at(vi);

            if(v_row.at(si) != 0) {
                found = true;
                ni = si;
                cycle_res_sum = 0;
            }
            else break;
        }
    }

    if(found) {
        res_cycle.push_front(vi);
        cycle_res_sum += _g->matrix[vi][ni];
    }

    // TODO warunek wyjscia w razie nie istnienia cyklu hamiltona
    visited_count--;
    visited[vi] = 0;
}


inline int Tsp_nn::nearest_neigh(int vi)
{   
    const std::vector<vector<int>> neightmatrix = _g->matrix;
    std::unordered_map<int,int> v_neighs_map;
    std::list<int> nearest;

    std::vector<int> v_row = neightmatrix.at(vi);

    //vi neight from matrix
    for(int i = 0; i < gSize; i++) {
        int w = v_row.at(i);
        if(w != 0 || w != -1){
            v_neighs_map.insert({i , w});
        } 
    }

    int nsize;


    while((nsize = v_neighs_map.size()) > 0) {

        int nearestWeight = INT_MAX;
        int nearestN;

        //find lowest weight
        for(auto& it : v_neighs_map) {
            if(it.second < nearestWeight) {
                nearestWeight = it.second;
                nearestN = it.first;
            }
        }

        if(visited[nearestN] == 0) {
            return nearestN;
        }
        else {
            v_neighs_map.erase(nearestN);
        }
    }

    //vi dont have unvis neigh
    return -1;
}
#endif