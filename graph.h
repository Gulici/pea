#ifndef GRAPH
#define GRAPH

#include <vector>
#include <assert.h>

struct Edge {
    int _v1,_v2,_w;
    Edge(int v1,int v2,int w) {
        _v1 = v1;
        _v2 = v2;
        _w = w;
    }
};

struct Node {
    int index;
    int parentIndex;
    std::vector<int> childIndexes;
    std::vector<Edge> edges;
};

class Graph {
    int _size;
    std::vector<Node> nodes;
    std::vector<Edge> edges;
    bool isDirected = false;

    public:
        Graph(std::vector<std::vector<int>> data);
        ~Graph();
        void addEdge(int v1, int v2, int weight);
        std::vector<std::vector<int>> neighMatrix();
        int getSize();
        std::vector<Edge>& getEdges();
        std::vector<Node>& getNodes();
};

Graph::Graph(std::vector<std::vector<int>> data) {
    std::vector<int> row = data[0];
    _size = row[0];
    int nE = row[1];

    for(int i = 0; i < _size; i++) {
        Node n;
        n.index = i;
        nodes.push_back(n);
    }

    for(int i = 1; i <= nE; i++) {
        row = data[i];
        int v1 = row[0];
        int v2 = row[1];
        int weight = row[2]; 
        addEdge(v1,v2,weight);
    }
}

inline Graph::~Graph() {
}

void Graph::addEdge(int v1,int v2,int weight) {
    assert (v1 >= 0 && v1 < _size);
    assert (v2 >= 0 && v2 < _size);
    
    Node &n1 = nodes.at(v1);   
    Node &n2 = nodes.at(v2);

    Edge e1 = Edge(v1,v2,weight);
    edges.push_back(e1);
    n1.edges.push_back(e1);

    if(!isDirected) {
        Edge e2 = Edge(v2,v1,weight);
        edges.push_back(e2);
        n2.edges.push_back(e2);
    }
}

std::vector<std::vector<int>> Graph::neighMatrix() {   
    std::vector<std::vector<int>> matrix;
    for(int i = 0; i < _size; i++) {
        std::vector<int> row;
        std::vector<Edge> iedges = nodes.at(i).edges;

        for(int j = 0; j < _size; j++) {
            row.push_back(0);
        }

        for(int j = 0; j < iedges.size(); j++) {
            Edge &e = iedges.at(j);
            row[e._v2] = e._w;
        }
        matrix.push_back(row);
    }

    return matrix;
}

inline int Graph::getSize()
{
    return _size;
}

inline std::vector<Edge> &Graph::getEdges()
{
    return edges;

}

inline std::vector<Node> &Graph::getNodes()
{
    return nodes;
}

#endif