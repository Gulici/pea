#ifndef TSP_B
#define TSP_B

#include "graph.h"
#include <vector>
#include <unordered_map>
#include <list>
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

    int maxIter = 10000;
    bool debug = true;
    int cycle_res_sum;

    public:
    Tsp_b(Graph& g, bool debug, int MAX_ITER);
    void solve();
    void hamil_cycle_step(int vi);
};

Tsp_b::Tsp_b(Graph& g, bool DEBUG, int MAX_ITER) {
    this->_g = &g;
    debug = DEBUG;
    maxIter = MAX_ITER;
    gSize = g.getSize();
    nodes = g.getNodes();
}

inline void Tsp_b::solve() {
    int bestKnow = _g->getBestKnow();
    int iterCount = 0;

    for(int i = 0; i < gSize; i ++) {
        si = i;
        visited_count++;

        if(iterCount >= maxIter || bestRes == bestKnow) {
            break;
        }

        visited = new int[gSize];
        for(int i = 0; i < gSize; i++) {
            visited[i] = 0;
        }

        hamil_cycle_step(si);
        
        iterCount++;

        //print debug
        if(debug && (iterCount % 1 == 0)) {
            std::cout << iterCount << "/" << maxIter << " best yet: " << bestRes << endl;
        }
    }

    cout << "Brute-force\nNajlepszy znaleziony cykl:";
    for(int v : best_cycle) cout << v << " ";
    cout << " Koszt: " << bestRes << endl;
}

inline void Tsp_b::hamil_cycle_step(int vi)
{
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
        else {

        }
    }
    
    visited_count--;
    visited[vi] = 0;
    currentNode.weight = 0;
    currentNode.parentIndex = -1;
}

#endif