#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <set>
#include <iterator>

typedef struct {
    unsigned source;
    unsigned target;
} edge;

bool cmp(edge e1, edge e2)
{
    if (e1.source != e2.source) {
        return e1.source < e2.source;
    }
    return e1.source < e2.target;
    
}

std::set<edge, decltype(cmp)*> fused(cmp);

void readEdgelist(std::string filename)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Unable to open file" << std::endl;
        return;
    }
    edge e;
    while(file >> e.source >> e.target)
    {
        fused.insert(e);
    }
    
}

void fuseGraph()
{
    std::ofstream file;
    file.open("fusedGraph.txt");
    for(auto &x: fused)
    {
        file << x.source << " " << x.target << std::endl;
    }
    file.close();
}

int main()
{
    std::string files[5] = {"graph0.txt", "graph1.txt", "graph2.txt", "graph3.txt", "graph4.txt"};
    for(auto &x: files)
    {
        readEdgelist(x);
    }
    fuseGraph();
}
