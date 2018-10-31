#include <bits/stdc++.h>
#include <iostream>
#include <list>
#include <stack>
#include <cmath>
#define NIL -1
using namespace std;

void die(const std::string& msg) {
    std::cerr << msg << std::endl;
    exit(1);
}

struct Clause {
    int firstVar;
    int secondVar;
};

class Graph {

public:
    vector<int> result;
    int numVar;
    int numVertices;
    int numEdges;
    vector<list<int>> adj;

    void getSCC(int u, vector<int> disc, vector<int> low,
                stack<int>& st, vector<bool> stackMem);

    Graph(int n, int m);
    int convertIndex(int i);
    void clauseToEdge(int v, int w);
    void SCC();
    bool isSatisfiable(vector<int>& result);
    void showGraph();
};

Graph::Graph(int n, int m){
    this->numVar = n;
    this->numVertices = 2*n;
    this->numEdges = 2*m;
    adj.resize(numVertices);
}

int Graph::convertIndex(int i){
    if (i<0){
        return -1*i+numVar-1;
    } else{
        return i-1;
    }
}

void Graph::clauseToEdge(int l1, int l2){
    int u, v;
    //cout<<"clause to edge:"<<endl;
    u = convertIndex(-l1);
    v = convertIndex(l2);
    //cout<<"    check edge: u, v = "<<u<<", "<<v<<endl;
    //die("line 57");
    adj[u].push_back(v);
    u = convertIndex(-l2);
    v = convertIndex(l1);
    //cout<<"    check edge: u, v = "<<u<<", "<<v<<endl;
    adj[u].push_back(v);
}

void Graph::showGraph(){
    for (int i = 0; i<numVertices;i++){
        cout<<"at i = "<<i<<"| ";
        for(auto v : adj[i]){
            cout<<v<<", ";
        }
        cout<<endl;
    }
}

void Graph::getSCC(int u, vector<int> disc, vector<int> low,
                   stack<int>& st, vector<bool> stackMem){
    int time = 0;

    time++;
    disc[u] = time;
    low[u] = time;
    st.push(u);
    stackMem[u] = true;

    list<int>::iterator i;
    for(i = adj[u].begin(); i!=adj[u].end(); i++ ){
        int v = *i;

        if(disc[v] == NIL){
            getSCC(v, disc, low, st, stackMem);
            low[u] = min(low[u], disc[v]);
        }
        else if (stackMem[v] == true){
            low[u] = min(low[u], disc[v]);
        }
    }
    int w = 0;
    if (low[u] == disc[u]){
        while (st.top() != u){
            w = st.top();
            cout<<w<<" ";
            stackMem[w] = false;
            st.pop();
        }
        w = st.top();
        cout<< w<<endl;
        stackMem[w] = false;
        st.pop();
    }
}

void Graph::SCC(){
    vector<int> disc(numVertices, NIL);
    vector<int> low(numVertices, NIL);
    vector<bool> stackMem(numVertices, false);
    stack<int> st;

    for(int i = 0; i< numVertices; i++){
        if(disc[i] == NIL){
            getSCC(i, disc, low, st, stackMem);
        }
    }
}

bool Graph::isSatisfiable(vector<int>& result) {
    return false;
}

struct TwoSatisfiability {
    int numVars;
    vector<Clause> clauses;

    TwoSatisfiability(int n, int m) :
        numVars(n),
        clauses(m)
    {  }

    bool isSatisfiable(vector<int>& result) {
        // This solution tries all possible 2^n variable assignments.
        // It is too slow to pass the problem.
        // Implement a more efficient algorithm here.
        for (int mask = 0; mask < (1 << numVars); ++mask) {
            for (int i = 0; i < numVars; ++i) {
                result[i] = (mask >> i) & 1;
            }

            bool formulaIsSatisfied = true;

            for (const Clause& clause: clauses) {
                bool clauseIsSatisfied = false;
                if (result[abs(clause.firstVar) - 1] == (clause.firstVar < 0)) {
                    clauseIsSatisfied = true;
                }
                if (result[abs(clause.secondVar) - 1] == (clause.secondVar < 0)) {
                    clauseIsSatisfied = true;
                }
                if (!clauseIsSatisfied) {
                    formulaIsSatisfied = false;
                    break;
                }
            }

            if (formulaIsSatisfied) {
                return true;
            }
        }
        return false;
    }
};

int main() {
    ios::sync_with_stdio(false);

    int n, m;
    cin >> n >> m;
    //TwoSatisfiability twoSat(n, m);
    Graph twoSat(n, m);
    int firstVar, secondVar;
    for (int i = 0; i < m; ++i) {
        //cin >> twoSat.clauses[i].firstVar >> twoSat.clauses[i].secondVar;
        cin >> firstVar >> secondVar;
        twoSat.clauseToEdge(firstVar, secondVar);
    }

    twoSat.showGraph();
    twoSat.SCC();

    vector<int> result(n);
    if (twoSat.isSatisfiable(result)) {
        cout << "SATISFIABLE" << endl;
        for (int i = 1; i <= n; ++i) {
            if (result[i-1]) {
                cout << -i;
            } else {
                cout << i;
            }
            if (i < n) {
                cout << " ";
            } else {
                cout << endl;
            }
        }
    } else {
        cout << "UNSATISFIABLE" << endl;
    }

    return 0;
}
