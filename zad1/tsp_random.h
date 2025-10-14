#ifndef TSP_R
#define TSP_R

#include "graph.h"
#include <vector>
#include <list>
#include "utils.h"

class Tsp_random {
    Graph *_g;
    int gSize;
    int *visited;
    int visited_count = 0;
    bool found = false;
    int si;
    std::list<int> res_cycle;
    std::list<int> best_cycle;

    int maxIter = 10000;
    bool debug;
    int cycle_res_sum;

    public:
    Tsp_random(Graph& g, bool DEBUG, int MAX_ITER);
    void solve();
    void hamil_cycle();
    void hamil_cycle_step(int vi);
    int random_neigh(int vi);
};

Tsp_random::Tsp_random(Graph& g, bool DEBUG, int MAX_ITER) {
    debug = DEBUG;
    maxIter = MAX_ITER;
    this->_g = &g;
    gSize = g.getSize();
}

inline void Tsp_random::solve() {
    int bestRes = INT_MAX;
    int bestKnow = _g->getBestKnow();
    int iterCount = 0;

    while(1) {
        if(iterCount >= maxIter || bestRes == bestKnow) {
            break;
        }

        hamil_cycle();
        
        if(cycle_res_sum < bestRes) {
            bestRes = cycle_res_sum;
            best_cycle.clear();
            best_cycle.assign(best_cycle.begin(),best_cycle.end());
        }
        iterCount++;

        //print debug
        if(debug && (iterCount % 10000 == 0)) {
            std::cout << iterCount << "/" << maxIter << " best yet: " << bestRes << endl;
        }
    }

    //print result
    cout << "Random\nNajlepszy znaleziony cykl:";
    for(int v : res_cycle) cout << v << " ";
    cout << " Koszt: " << bestRes << endl;
}

inline void Tsp_random::hamil_cycle()
{
    cycle_res_sum = INT_MAX;
    res_cycle.clear();
    found = false;

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
        // std::reverse(res_cycle);
        // for(int v : res_cycle) std::cout << v << " ";
        // cout << " Koszt: " << cycle_res;
    }
    else std::cout << "nie znaleziono cyklu";
}

inline void Tsp_random::hamil_cycle_step(int vi)
{
    //czy ma  nieodwiedzonych sasiadow?
    int ni = si;
    while(!found) {
        ni = random_neigh(vi);
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

    visited_count--;
    visited[vi] = 0;
}

inline int Tsp_random::random_neigh(int vi)
{   
    const std::vector<vector<int>> neightmatrix = _g->matrix;
    std::vector<int> v_neighs;
    std::vector<int> v_row = neightmatrix.at(vi);

    //vi neight from matrix
    for(int i = 0; i < gSize; i++) {
        int n = v_row.at(i);
        if(n != 0 || n != -1) v_neighs.push_back(i);
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