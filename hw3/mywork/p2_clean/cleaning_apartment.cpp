#include <bits/stdc++.h>
using namespace std;

struct Edge {
    int from;
    int to;
};



struct ConvertHampathToSat {
    int numVertices;
    vector<Edge> edges;
    vector<vector<bool>> ConnectAB;
    vector<vector<int>> Nji;

    ConvertHampathToSat(int n, int m) :
        numVertices(n),
        edges(m),
        ConnectAB(n, vector<bool>(n, false)),
        Nji(n, vector<int>(n))
    {  }

    void printEquisatisfiableSatFormula() {
        //Nji.resize(numVertices);
        for(int j = 0; j<numVertices; j++){
            //Nji[j].resize(numVertices);
            for(int i = 0; i<numVertices; i++){
                Nji[j][i] = j*numVertices+1+i;
            }
        }

        int totClause = numVertices+
                        numVertices*numVertices*(numVertices-1)/2+
                        numVertices+
                        numVertices*numVertices*(numVertices-1)/2+
                        (numVertices*(numVertices-1)-2*edges.size())*(numVertices-1);

        int totNode = numVertices*numVertices;
        cout << totClause <<" "<< totNode<< endl; // need to fix!!!!!!!!!!!!

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

        for(int i = 0; i<numVertices; i++){
            for(int j = 0; j<numVertices; j++){
                cout<<Nji[j][i]<<" ";
            }
            cout<<"0\n";
        }

        //forth batch of clause: No two nodes j and k have the same position in H path
        for(int i = 0; i<numVertices; i++){
            for(int j1 = 0; j1<numVertices; j1++){
                for(int j2 = j1+1; j2<numVertices; j2++){
                    cout<<Nji[j1][i]*-1<<" "<<Nji[j2][i]*-1<<" 0\n";
                }
            }
        }

        //fifth batch of clause: if no edge, then the pair cannot be neighbors in H path
        for(int j1 = 0; j1<numVertices; j1++){
            for(int j2 = j1+1; j2<numVertices; j2++){
                //cout<<"ConnectAB"<<j1<<","<<j2<<"="<<ConnectAB[j1][j2]<<endl;
                if(!ConnectAB[j1][j2]){
                    for(int i = 1; i<numVertices; i++){
                        cout<<Nji[j1][i]*-1<<" "<<Nji[j2][i-1]*-1<<" 0\n";
                        cout<<Nji[j2][i]*-1<<" "<<Nji[j1][i-1]*-1<<" 0\n";
                    }
                }
            }
        }

        // This solution prints a simple satisfiable formula
        // and passes about half of the tests.
        // Change this function to solve the problem.
        //cout << "3 2" << endl;
        //cout << "1 2 0" << endl;
        //cout << "-1 -2 0" << endl;
        //cout << "1 -2 0" << endl;
    }
};

int main() {
    ios::sync_with_stdio(false);

    int n, m;
    cin >> n >> m;
    ConvertHampathToSat converter(n, m);
    int from, to;
    for (int i = 0; i < m; ++i) {
        cin >> converter.edges[i].from >> converter.edges[i].to;
        from = converter.edges[i].from;
        to = converter.edges[i].to;
        converter.ConnectAB[from-1][to-1] = true;
        converter.ConnectAB[to-1][from-1] = true;
    }

    converter.printEquisatisfiableSatFormula();

    return 0;
}
