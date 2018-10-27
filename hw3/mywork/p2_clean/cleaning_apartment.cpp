#include <bits/stdc++.h>
using namespace std;

struct Edge {
    int from;
    int to;
};

struct ConvertHampathToSat {
    int numVertices;
    vector<Edge> edges;
    int nodeConnectCounter = 0;
    vector<vector<int>> ConnectAB;
    vector<vector<int>> Nji;
    vector<bool> visited;

    ConvertHampathToSat(int n, int m) :
        numVertices(n),
        edges(m),
        ConnectAB(n),
        Nji(n, vector<int>(n)),
        visited(n, false)
    {  }

    void dfs(int curNode){
        visited[curNode] = true;
        for(int j = 0; j<ConnectAB[curNode].size();j++){
            int toNode = ConnectAB[curNode][j];
            if(!visited[toNode]){
                dfs(toNode);
            }
        }
    }

    bool isDiconnected(){
        dfs(0);
        bool checkVisitedAll = false;
        for(int j = 0; j<visited.size();j++){
            if(!visited[j]){
                checkVisitedAll = true;
                break;
            }
        }
        return checkVisitedAll;
    }


    int printEquisatisfiableSatFormula() {
        //build matrix to all nodes
        for(int j = 0; j<numVertices; j++){
            for(int i = 0; i<numVertices; i++){
                Nji[j][i] = j*numVertices+1+i;
            }
        }

        //if graph is disconnected, return false
        if (isDiconnected()){
            cout<<"2 1\n1 0\n-1 0\n";
            return 0;
        }

        int totClause = numVertices+                                //first batch of clauses
                        numVertices*numVertices*(numVertices-1)/2+  //second batch of clauses
                        //numVertices+                              //third batch of clauses
                        numVertices*numVertices*(numVertices-1)/2+  //fourth batch of clauses
                        (nodeConnectCounter*(numVertices-1));       //fifth batch of clauses

        int totNode = numVertices*numVertices;
        cout << totClause <<" "<< totNode<< endl;

        // first batch of clause: each node appear once in H path
        for(int j = 0; j<numVertices; j++){
            for(int i = 0; i<numVertices; i++){
                cout<<Nji[j][i]<<" ";
            }
            cout<<"0\n";
        }

        // second batch of clause: no node appear twice in H path
        for(int j = 0; j<numVertices; j++){
            for(int i1 = 0; i1<numVertices; i1++){
                for(int i2 = i1+1; i2<numVertices; i2++){
                    cout<<Nji[j][i1]*-1<<" "<<Nji[j][i2]*-1<<" 0\n";
                }
            }
        }

        //third batch of clause: all positions of the H path must be occupied
        //turns out this case was not needed...
        /*
        for(int i = 0; i<numVertices; i++){
            for(int j = 0; j<numVertices; j++){
                cout<<Nji[j][i]<<" ";
            }
            cout<<"0\n";
        }*/

        //forth batch of clause: No two nodes j and k have the same position in H path
        for(int i = 0; i<numVertices; i++){
            for(int j1 = 0; j1<numVertices; j1++){
                for(int j2 = j1+1; j2<numVertices; j2++){
                    cout<<Nji[j1][i]*-1<<" "<<Nji[j2][i]*-1<<" 0\n";
                }
            }
        }

        //fifth batch of clause: a pair of connected nodes must be neighbors if they are adjacent on the H path
        for(int j1 = 0; j1<numVertices; j1++){
            if(ConnectAB[j1].empty()){
                continue;
            }
            for(int i = 1; i<numVertices; i++){
                cout<<Nji[j1][i-1]*-1<<" ";
                for(int e = 0; e<ConnectAB[j1].size(); e++){
                    int toJNode = ConnectAB[j1][e];
                    cout<<Nji[toJNode][i]<<" ";
                }
                cout<<"0\n";
            }
        }
        return 0;
    }
};

int main() {
    ios::sync_with_stdio(false);

    int n, m;
    cin >> n >> m;

    // if only 1 node, return true
    if(n==1){
        cout<<"1 1\n1 0\n";
        return 0;
    }

    // if no edges and > 1 node, return false
    if(m == 0){
        cout<<"2 1\n1 0\n-1 0\n";
        return 0;
    }

    ConvertHampathToSat converter(n, m);
    int from, to;
    for (int i = 0; i < m; ++i) {
        cin >> converter.edges[i].from >> converter.edges[i].to;
        from = converter.edges[i].from;
        to = converter.edges[i].to;

        //count connected nodes for computing total number of clauses
        if (converter.ConnectAB[from-1].empty()){converter.nodeConnectCounter++;}
        if (converter.ConnectAB[to-1].empty()){converter.nodeConnectCounter++;}

        //build adjacency matrix
        converter.ConnectAB[from-1].push_back(to-1);
        converter.ConnectAB[to-1].push_back(from-1);
    }

    converter.printEquisatisfiableSatFormula();

    return 0;
}
