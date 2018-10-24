#include <ios>
#include <iostream>
#include <vector>

using namespace std;

struct Edge {
    int from;
    int to;
};

struct ConvertGSMNetworkProblemToSat {
    int numVertices;
    vector<Edge> edges;
    //vector<vector<int>> vColorID;

    ConvertGSMNetworkProblemToSat(int n, int m) :
        numVertices(n),
        edges(m)
    {  }
    int transformNodeIDToBlue(int nodeID){
        return nodeID*3;
    }
    void printEquisatisfiableSatFormula() {
        //first line input 1) the number of clauses, and 2) number of vertices
        cout << numVertices*4+edges.size()*3<<" "<< numVertices*3 << endl;

        // for each node, print clauses to restrain to only 1 color
        int blue, green, red;
        for (int i = 0;i<numVertices;i++){
            blue = transformNodeIDToBlue(i+1);
            green = blue - 1;
            red = green - 1;
            cout<<red<<" "<<green<<" "<<blue<<" 0\n";
            cout<<red*-1<<" "<<blue*-1<<" 0\n";
            cout<<red*-1<<" "<<green*-1<<" 0\n";
            cout<<green*-1<<" "<<blue*-1<<" 0\n";
        }

        //for each edge, print clauses to restrain connected nodes have different colors
        int toBlue, toGreen, toRed, fromBlue, fromGreen, fromRed;
        for (int i = 0;i<edges.size();i++){
            toBlue = transformNodeIDToBlue(edges[i].to);
            toGreen = toBlue-1;
            toRed = toGreen-1;

            fromBlue = transformNodeIDToBlue(edges[i].from);
            fromGreen = fromBlue-1;
            fromRed = fromGreen-1;

            cout<<toRed*-1<<" "<<fromBlue<<" "<<fromGreen<<" 0\n";
            cout<<toBlue*-1<<" "<<fromRed<<" "<<fromGreen<<" 0\n";
            cout<<toGreen*-1<<" "<<fromBlue<<" "<<fromRed<<" 0\n";

        }
    }
};

int main() {
    ios::sync_with_stdio(false);

    int n, m;
    cin >> n >> m;
    ConvertGSMNetworkProblemToSat converter(n, m);
    for (int i = 0; i < m; ++i) {
        cin >> converter.edges[i].from >> converter.edges[i].to;
    }

    converter.printEquisatisfiableSatFormula();

    return 0;
}
