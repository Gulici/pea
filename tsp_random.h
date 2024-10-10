#ifndef TSP_R
#define TSP_R

#include "graph.h"
#include <vector>
#include "utils.h"

class Tsp_random {
    Graph *_g;
    int gSize;
    int *visited;
    int visited_count = 0;
    bool found = false;
    int si;
    vector<int> res_cycle;

    public:
    Tsp_random(Graph& g);
    void hamil_cycle();
    void hamil_cycle_step(int vi);
    int random_neigh(int vi);
};

Tsp_random::Tsp_random(Graph& g) {
    this->_g = &g;
    gSize = g.getSize();
    // std::vector<vector<int>> matrix = _g->neighMatrix();
    // neightmatrix = matrix;
    // neightmatrix.data(g.neighMatrix());
}

inline void Tsp_random::hamil_cycle()
{
    visited = new int[gSize];
    for(int i = 0; i < gSize; i++) {
        visited[i] = 0;
    }
    //random start point
    si = random(0,gSize-1);
    visited[si] = 1;
    visited_count++;
    hamil_cycle_step(si);
    if(found) {
        for(int v : res_cycle) cout << v << " ";
    }
    else cout << "nie znaleziono cyklu";
}

inline void Tsp_random::hamil_cycle_step(int vi)
{
    //czy ma  nieodwiedzonych sasiadow sasiadow?
    while(!found) {
        int ni = random_neigh(vi);
        if(ni != -1) {
            visited[ni] = 1;
            visited_count++;
            hamil_cycle_step(ni);
        }
        else if (visited_count != gSize) {
            break;
        }
        else if (visited_count == gSize) {
            // czy wsrod sasiadow n start?
            const std::vector<vector<int>> neightmatrix = _g->neighMatrix();
            std::vector<int> v_neighs;
            std::vector<int> v_row = neightmatrix.at(vi);

            if(v_row.at(si) != 0) {
                found = true;
            }
            else break;
        }
    }

    if(found) {
        res_cycle.push_back(vi);
    }
    // TODO warunek wyjscia w razie nie istnienia cyklu hamiltona
    visited_count--;
    visited[vi] = 0;
}

inline int Tsp_random::random_neigh(int vi)
{   
    const std::vector<vector<int>> neightmatrix = _g->neighMatrix();
    std::vector<int> v_neighs;
    std::vector<int> v_row = neightmatrix.at(vi);

    //vi neight from matrix
    for(int i = 0; i < gSize; i++) {
        int n = v_row.at(i);
        if(n != 0) v_neighs.push_back(i);
    }

    int nsize;
    while((nsize = v_neighs.size()) > 0) {
        int randi = random(0,nsize-1);
        int ni = v_neighs.at(randi);
        if(visited[ni] == 0) {
            return ni;
        }
        else {
            v_neighs.erase(v_neighs.begin()+randi);
        }
    }

    //vi dont have unvis neigh
    return -1;
}
#endif