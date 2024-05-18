#include<iostream>
#include<vector>
#include<fstream>
#include<sstream>
#include<string>
#include<algorithm>
#include<set>
#include<iterator>


typedef struct
{
    std::string name;
    double danceability;
    double energy;
    double loudness;
    double valence;
    
}node;

typedef struct
{
    unsigned source;
    unsigned target;
}edge;

typedef struct
{
  std::vector<edge> edges; // list of edges
  std::vector<node> nodes; // list of nodes
  std::vector<unsigned> ns;             // ns[l]: number of nodes in G_l
  std::vector<unsigned> d; // d[l]: degrees of G_l
  std::vector<unsigned> cd;             // cumulative degree: (starts with 0) length=n+1
  std::vector<unsigned> adj;            // truncated list of neighbors
  std::vector<unsigned> lab;              // lab[i] label of node i
  std::vector<std::vector<unsigned>> sub; // sub[l]: nodes in G_l
  unsigned n() const{
    return nodes.size(); //no. of nodes
  }
  unsigned e() const{
    return edges.size(); //no. of edges
  }

} graph;

void buildGraph(graph &g)
{
    node n;
    std::ifstream file("playlist_audio_features.txt");
    if (!file.is_open())
    {
        std::cerr << "unable to open file" << std::endl;
    }
    std::string line;
    while(std::getline(file, line))
    {
        std::istringstream iss(line);
        std::string name, danceability, energy, loudness, valence;
        if(std::getline(iss,name,','))
        {
            if(iss >> danceability >> energy >> loudness >> valence)
            {
                n.name = name;
                n.danceability = stod(danceability);
                n.energy = stod(energy);
                n.loudness = stod(loudness);
                n.valence = stod(valence);
                g.nodes.push_back(n);
            }
        }

    }
    file.close();
}

void createEdgelist(graph &g, int mood)
{
    for(unsigned i = 0; i < g.n(); i++)
    {
        const node& node_i = g.nodes[i];
        bool condition = false;
        switch(mood)
        {
            case 0:
                condition = (node_i.energy >= 0.8 && node_i.loudness >= -7.0 && node_i.valence <= 0.4);
                break;

            case 1:
                condition = (node_i.danceability >= 0.5 && node_i.energy >= 0.5 && node_i.valence >= 0.7);
                break;

            case 2:
                condition = (node_i.danceability >= 0.4 && node_i.energy >= 0.4 && node_i.valence >= 0.3 && node_i.valence < 0.7);
                break;

            case 3:
                condition = (node_i.danceability <= 0.4 && node_i.energy <= 0.4 && node_i.valence <= 0.2);
                break;

            case 4:
                condition = (node_i.danceability <= 0.3 && node_i.valence <= 0.1);
                break;            
        }
        if(condition)
        {
            for(unsigned j = i+1; j < g.n(); j++)
            {
                const node& node_j = g.nodes[j];
                bool condition_j = false;
                switch(mood)
                {
                    case 0:
                        condition = (node_i.energy >= 0.8 && node_i.loudness >= -7.0 && node_i.valence <= 0.4);
                        break;

                    case 1:
                        condition = (node_i.danceability >= 0.5 && node_i.energy >= 0.5 && node_i.valence >= 0.7);
                        break;

                    case 2:
                        condition = (node_i.danceability >= 0.4 && node_i.energy >= 0.4 && node_i.valence >= 0.3 && node_i.valence < 0.7);
                        break;

                    case 3:
                        condition = (node_i.danceability <= 0.4 && node_i.energy <= 0.4 && node_i.valence <= 0.2);
                        break;

                    case 4:
                        condition = (node_i.danceability <= 0.3 && node_i.valence <= 0.1);
                        break;            
                }
                if(condition_j)
                {
                    edge e;
                    e.source = i;
                    e.target = j;
                    g.edges.push_back(e);
                }
            }
        }
    }
    
}

void makeAdj(graph &g)
{
    unsigned md = 0;
    std::vector<unsigned>d0(g.n(),0);
    for(auto &e:g.edges)
    {
        d0[e.source]++;
        d0[e.target]++;
    }
    for(auto &x: d0)
    {
        md = std::max(x,md);
    }
    std::vector<unsigned>bin(md+1,0);
    std::vector<unsigned>pos(g.n());
    std::vector<unsigned>vert(g.n());
    for(unsigned i = 0; i < g.n()+1; i++)
    {
        bin[d0[i]]++;
    }
    unsigned start = 0;
    for(unsigned i = 0; i <= md; i++)
    {
        unsigned num = bin[i];
        bin[i] = start;
        start += num;
    }
    for(unsigned i = 0; i < g.n()+1; i++)
    {
        pos[i] = bin[d0[i]];
        vert[pos[i]] = i;
        bin[d0[i]]++;
    }
    g.d = vert;
    std::vector<unsigned> cd0(g.n()+1,0);
    std::vector<unsigned> adj0(2*g.e(),0);
    g.adj.resize(g.e());
    for(unsigned i = 1; i < g.n()+1; i++)
    {
        cd0[i] = cd0[i-1] + d0[i-1];
        d0[i-1] = 0;
    }
    for(unsigned i = 0; i < g.e(); i++)
    {
        adj0[cd0[g.edges[i].source] + d0[g.edges[i].source]++] = g.edges[i].target;
        adj0[cd0[g.edges[i].target] + d0[g.edges[i].target]++] = g.edges[i].source;
    }
    g.adj = adj0;
    g.cd = cd0;


}

std::set<int> neighbors(graph &g, unsigned n)
{
    std::set<int> neighbor;
    unsigned start = g.cd[n];
    unsigned end = g.cd[n+1];
    for(unsigned i = start; i < end; i++)
    {
        unsigned j = g.adj[i];
        if(j != n)
        {
            neighbor.insert(j);
        }
    }
    return neighbor;
}

void bk_pivot(graph &g, std::set<int> R, std::set<int> P, std::set<int> X, std::vector<std::set<int>> &cliques)
{
    if(P.empty() && X.empty())
    {
        cliques.push_back(R);
        return;
    }
    std::set<int> P_prime, P_union_X;
    for(int i = g.n(); i >= 0; i--)
    {
        std::set_union(P.begin(), P.end(), X.begin(), X.end(),inserter(P_union_X, P_union_X.begin()));
        std::set<int> N_u = neighbors(g, g.d[i]);
        std::set_difference(P.begin(), P.end(), N_u.begin(),N_u.end(),inserter(P_prime, P_prime.begin()));
        for(const auto v: P_prime)
        {
            std::set<int> R_v, P_Nv, X_Nv,Nv;
            R_v = R;
            R_v.insert(v);
            Nv = neighbors(g, v);
            std::set_intersection(P.begin(), P.end(), Nv.begin(), Nv.end(), inserter(P_Nv, P_Nv.begin()));
            std::set_intersection(X.begin(), X.end(), Nv.begin(), Nv.end(), inserter(X_Nv, X_Nv.begin()));
            bk_pivot(g,R_v, P_Nv, X_Nv, cliques);
            P.erase(v);
            X.insert(v);
        }

    }
    
}

int main()
{
    int mood;
    graph g;
    std::cin >> mood;
    buildGraph(g);
    std::vector<std::set<int>> cliques;
    createEdgelist(g, mood);
    makeAdj(g);
    std::set<int> R, P, X;
    for (unsigned i = 0; i < g.n(); ++i)
    {
        P.insert(i);
    }
    bk_pivot(g, R, P, X, cliques);
    std::cout << "Maximal Cliques:" << std::endl;
    for (const auto &clique : cliques)
    {
        std::cout << "{ ";
        for (int node : clique)
        {
            std::cout << g.nodes[node].name << " ";
        }
        std::cout << "}" << std::endl;
    }
}

