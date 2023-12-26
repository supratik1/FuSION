#include "../include/GraphManagerNew.h"
#define oo 10000000 l

int maxflow() {
    
}

void ghtree(GraphNew * graph) {
    int n = graph->get_node_ids().size();
    int m = graph->get_edge_ids().size();
    int ** G = new int[n][n];   // check this . THis is wrong
    

    
    for(std::set<int>:: iterator i = graph->get_edge_ids().begin(); i != graph->get_edge_ids().end(); i++) {
        int p = graph->get_edge_from_eid(*i)->get_source();
        int q = graph->get_edge_from_eid(*i)->get_target();
        G[p][q] += 1;
        G[q][p] += 1;
    }
    
    vector <int> parent(n+1, 1);
    vector < vector <int> > answer(n+1, vector<int> (n+1, oo));
    vector < vector <bool> > visited(n+1, vector <bool> (n+1, false));
    
    // Gusfield algorithm loop 
    // From topcoder discussions of Gomory Hu tree using Gusfield algorithm
    for(int i=2; i<=n; i+) {
        source = i;
        sink = parent[i];
        int ** tmp = new int[n][n];
        
        // Storing graph for future use
        for(int u = 0; u<n; u++)
            for(int v = 0; v<n; v++)
                tmp[u][v] = G[u][v] ;
        
        int F = find_max_flow();                // Need to give proper arguments over here !!
        dfs(i, source, visited) ;
        
        for(int j = i+1; j<=n; j++)
            if(visited[i][j] && parent[j] == parent[i])
                patent[j] = i;
        answer[i][ parent[j] ] = answer[ parent[j] ][i] = F;
        
        for(int j=1; j<i; j++) {
            answer[i][j] = answer[j][i] = min(F, answer[ parent[i] ][j]);
        }
        
        // Restoring graph as it gets converted to Residual graph during the flow calculation
        for(int u=0; u<n; u++) {
            for (int v=0; v<n; v++) {
                G[u][v] = temp[u][v];
            }
        }
    }
    
    // parent array helps to make the gomory hu tree
    // answer stores the flow value b/w any two points
    // visited[i] stores the array of nodes in set S in the ith cut.
}
