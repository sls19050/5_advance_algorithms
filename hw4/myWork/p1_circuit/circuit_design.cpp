#include <bits/stdc++.h>
#include <iostream>
#include <list>
#include <stack>
#include <cmath>
#include <limits>
#include <queue>
#include <tuple>

using namespace std;

//const int MAX_INT = std::numeric_limits<int>::max();
const int NIL = -1;

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
    queue<int> reverseTopOrderNodeID;

    void getSCC(int u, vector<int>& disc, vector<int>& low,
                stack<int>& st, vector<bool> stackMem, bool& pass);

    Graph(int n, int m);
    int convertIndex(int i);
    void clauseToEdge(int v, int w);
    void SCC();
    bool failSCC(int w, vector<int>& countVars);
    bool isSatisfiable(vector<int>& result);
    void showGraph();
    tuple<int,int> getVarIDandSign(int curLit);
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

bool Graph::failSCC(int w, vector<int>& countVars){

    int varID, value, oldCount;
    if (w>=numVar){
        varID = w-numVar; //for negated literals
        value = -1;
    } else{
        varID = w;
        value = 1;
    }
    oldCount = abs(countVars[varID]);
    countVars[varID] = countVars[varID] + value;
    if (oldCount > abs(countVars[varID])){
        return true;
    }
    return false;
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

void Graph::getSCC(int u, vector<int>& disc, vector<int>& low,
                   stack<int>& st, vector<bool> stackMem, bool& pass){

    if(pass){

    static int time = 0;

    //cout<<"recursive, at u = "<<u<<endl;

    time++;
    disc[u] = time;
    low[u] = time;
    st.push(u);
    stackMem[u] = true;

    list<int>::iterator i;
    for(i = adj[u].begin(); i!=adj[u].end(); i++ ){
        int v = *i;

        if(disc[v] == NIL){
            getSCC(v, disc, low, st, stackMem, pass);
            low[u] = min(low[u], low[v]);
        }
        else if (stackMem[v] == true){
            low[u] = min(low[u], disc[v]);
        }
    }
    int w = 0;
    if (low[u] == disc[u]){
        vector<int> countVars(numVar, 0);
        while (st.top() != u){
            w = st.top();
            //cout<<w<<" ";
            reverseTopOrderNodeID.push(w);
            if(failSCC(w, countVars)){
                pass = false;
                //cout<<"\nfailed SCC! breaking now!\n";
                break;
            }
            stackMem[w] = false;
            st.pop();
        }
        w = st.top();
        //cout<< w<<endl;
        reverseTopOrderNodeID.push(w);
        if(failSCC(w, countVars)){
            pass = false;
            //cout<<"\nfailed SCC! breaking now!\n";
        }
        stackMem[w] = false;
        st.pop();
    }
    }
}

tuple<int,int> Graph::getVarIDandSign(int curLit){
    //cout<<"curLit = "<<curLit<<endl;
    if (curLit>=numVar){
        return make_tuple(curLit - numVar, -1);
    }
    return make_tuple(curLit, 1);
}

bool Graph::isSatisfiable(vector<int>& result){
    vector<int> disc(numVertices, NIL);
    vector<int> low(numVertices, NIL);
    vector<bool> stackMem(numVertices, false);
    bool pass = true;
    stack<int> st;

    for(int i = 0; i< numVertices; i++){
        //cout<<"loop in SCC, i = "<<i<<endl;
        if(disc[i] == NIL){
            getSCC(i, disc, low, st, stackMem, pass);
        }
        if(!pass){return pass;}
    }

    result.resize(numVar);
    while (!reverseTopOrderNodeID.empty()){
        int curLit, var, sign;
        curLit = reverseTopOrderNodeID.front();
        reverseTopOrderNodeID.pop();

        tie(var, sign) = getVarIDandSign(curLit);
        if (result[var] == 0){
            result[var] = sign;
        }
    }

    return pass;
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

    //twoSat.showGraph();
    //twoSat.SCC();

    vector<int> result(n);
    if (twoSat.isSatisfiable(result)) {
        cout << "SATISFIABLE" << endl;
        for (int i = 0; i < n; ++i) {
            if(result[i]==0){
                cout<<i+1;
            } else{
                cout<<result[i]*(i+1);
                //cout<<result[i];
            }


            if (i < n-1) {
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
