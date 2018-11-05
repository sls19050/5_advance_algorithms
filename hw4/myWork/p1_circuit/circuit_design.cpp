#include <sys/resource.h>
#include <bits/stdc++.h>
#include <iostream>
#include <list>
#include <stack>
#include <cmath>
#include <queue>
#include <tuple>
#include <set>

using namespace std;

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

    void getSCC(int u, int disc[], int low[],
                   stack<int> *st, bool stackMem[], bool& pass);

    Graph(int n, int m);
    void clauseToEdge(int v, int w);
    bool isSatisfiable();
    void showGraph();
    bool assignAns(int curLit, set<int>& countPosVars, set<int>& countNegVars);
};

Graph::Graph(int n, int m){
    this->numVar = n;
    this->numVertices = 2*n;
    this->numEdges = 2*m;
    adj.resize(numVertices);
    result.assign(numVar,0);
}

bool Graph::assignAns(int curLit, set<int>& countPosVars, set<int>& countNegVars){

    int var, sign, oldCount;

    if (curLit>=numVar){
        var = curLit-numVar; //for negated literals
        sign = -1;
    } else{
        var = curLit;       //for literals w/o negation
        sign = 1;
    }

    if (sign > 0){
        countPosVars.insert(var);
    } else{
        countNegVars.insert(var);
    }

    bool is_inPos = countPosVars.find(var) != countPosVars.end();
    bool is_inNeg = countNegVars.find(var) != countNegVars.end();
    if (is_inPos && is_inNeg){ return false; }

    if (result[var] == 0){
        result[var] = sign;
    }
    return true;
}

void Graph::clauseToEdge(int l1, int l2){
    int u1, v2, u2, v1;

    if (l1<0){
        u1 = -l1-1;
        v2 = -l1+numVar-1;
    } else{
        u1 = l1+numVar-1;
        v2 = l1-1;
    }

    if (l2<0){
        u2 = -l2-1;
        v1 = -l2+numVar-1;
    } else{
        u2 = l2+numVar-1;
        v1 = l2-1;
    }

    adj[u1].push_back(v1);
    adj[u2].push_back(v2);
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

void Graph::getSCC(int u, int disc[], int low[],
                   stack<int> *st, bool stackMem[], bool& pass){

    if(pass){

    static int time = 0;

    time++;
    disc[u] = time;
    low[u] = time;
    st->push(u);
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
        set<int> countPosVars;
        set<int> countNegVars;

        while (st->top() != u){
            w = st->top();
            if(!assignAns(w, countPosVars, countNegVars)){
                pass = false;
                break;
            }
            stackMem[w] = false;
            st->pop();
        }

        if(pass){

        w = st->top();
        if(!assignAns(w, countPosVars, countNegVars)){
            pass = false;
        }
        stackMem[w] = false;
        st->pop();

        }
    }
    }
}

bool Graph::isSatisfiable(){

    int *disc = new int[numVertices];
    int *low = new int[numVertices];
    bool *stackMem = new bool[numVertices];
    stack<int> *st = new stack<int>();

    bool pass = true;

    for (int i = 0; i < numVertices; i++) {
        disc[i] = NIL;
        low[i] = NIL;
        stackMem[i] = false;
    }

    for(int i = 0; i< numVertices; i++){
        if(disc[i] == NIL){
            getSCC(i, disc, low, st, stackMem, pass);
        }
        if(!pass){return pass;}
    }
    return pass;
}

int main() {

    const rlim_t kStackSize = 64L * 1024L * 1024L;  // min stack size = 64 Mb
    struct rlimit rl;
    int result;
    result = getrlimit(RLIMIT_STACK, &rl);
    if (result == 0)
    {
        if (rl.rlim_cur < kStackSize)
        {
            rl.rlim_cur = kStackSize;
            result = setrlimit(RLIMIT_STACK, &rl);
            if (result != 0)
            {
                fprintf(stderr, "setrlimit returned result = %d\n", result);
            }
        }
    }

    ios::sync_with_stdio(false);

    int n, m;
    cin >> n >> m;

    Graph twoSat(n, m);
    int firstVar, secondVar;

    for (int i = 0; i < m; ++i) {
        cin >> firstVar >> secondVar;
        twoSat.clauseToEdge(firstVar, secondVar);
    }

    if (twoSat.isSatisfiable()) {
        cout << "SATISFIABLE" << endl;
        for (int i = 0; i < n; ++i) {
            cout<<twoSat.result[i]*(i+1)<<" ";
        }
    } else {
        cout << "UNSATISFIABLE" << endl;
    }

    return 0;
}
