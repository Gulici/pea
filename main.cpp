
#include <iostream>
#include <vector>
#include "utils.h"
#include "graph.h"
#include "tsp_random.h"

using namespace std;

int main() {
    vector<vector<int>> grapData;
    string filename = "g1.txt";
    grapData = readFromTxt(filename);

    Graph g = Graph(grapData);

    vector<vector<int>> matrix = g.neighMatrix();
    int size = g.getSize();

    for(int i = 0; i < size; i++) {
        for(int j = 0; j < size; j++) {
            cout << matrix[i][j];
        }
        cout << endl;
    }
    
    cout<<endl;

    Tsp_random tsp = Tsp_random(g);
    tsp.hamil_cycle();

    return 0;
}