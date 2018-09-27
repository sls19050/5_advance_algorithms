#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <tuple>

using std::vector;

void die(const std::string& msg) {
    std::cerr << msg << std::endl;
    exit(1);
}

/* This class implements a bit unusual scheme for storing edges of the graph,
 * in order to retrieve the backward edge for a given edge quickly. */
class FlowGraph {
public:
    struct Edge {
        int from, to, capacity, flow;
    };

public:
    /* List of all - forward and backward - edges */
    vector<Edge> edges;

    /* These adjacency lists store only indices of edges in the edges list */
    vector<vector<size_t> > graph;

public:
    explicit FlowGraph(size_t n): graph(n) {}

    void add_edge(int from, int to, int capacity) {
        /* Note that we first append a forward edge and then a backward edge,
         * so all forward edges are stored at even indices (starting from 0),
         * whereas backward edges are stored at odd indices in the list edges */
        Edge forward_edge = {from, to, capacity, 0};
        Edge backward_edge = {to, from, 0, 0};
        graph[from].push_back(edges.size());
        edges.push_back(forward_edge);
        graph[to].push_back(edges.size());
        edges.push_back(backward_edge);
    }

    size_t size() const {
        return graph.size();
    }

    const vector<size_t>& get_ids(int from) const {
        return graph[from];
    }

    const Edge& get_edge(size_t id) const {
        return edges[id];
    }

    void add_flow(size_t id, int flow) {
        /* To get a backward edge for a true forward edge (i.e id is even), we should get id + 1
         * due to the described above scheme. On the other hand, when we have to get a "backward"
         * edge for a backward edge (i.e. get a forward edge for backward - id is odd), id - 1
         * should be taken.
         *
         * It turns out that id ^ 1 works for both cases. Think this through! */
        edges[id].flow += flow;
        edges[id ^ 1].flow -= flow;
    }
};

void showEdge(FlowGraph& graph, int j){
    std::cout << graph.edges[j].from <<" ";
    std::cout << graph.edges[j].to <<" ";
    std::cout << graph.edges[j].capacity <<" ";
    std::cout << graph.edges[j].flow <<"\n";
}

void showGraph(FlowGraph& graph){
    std::cout << "show graph edges: \n";
    for(int j = 0; j<graph.edges.size();j++){
        std::cout<<"this is j = "<<j<<"\n";
        showEdge(graph, j);
    }
    std::cout << "show connecting_edges \n";
    for(int j = 0; j<graph.size();j++){
        std::cout<<"\n connecting edges from node j="<<j<<": ";
        vector<size_t> connecting_edges = graph.get_ids(j);
        for (int i = 0; i<connecting_edges.size();i++){
            std::cout<<connecting_edges[i]<<" ";
        }
    }
    std::cout<<"\n";
}

void showQueue(std::queue<size_t> myQueue){
    while(!myQueue.empty()){
        size_t curEdgeID = myQueue.front();
        myQueue.pop();
        std::cout<< "EdgeID = "<<curEdgeID<<"\n";
    }
}

FlowGraph read_data() {
    int vertex_count, edge_count;
    std::cin >> vertex_count >> edge_count;
    FlowGraph graph(vertex_count);
    for (int i = 0; i < edge_count; ++i) {
        int u, v, capacity;
        std::cin >> u >> v >> capacity;
        if (u == v){continue;}
        graph.add_edge(u - 1, v - 1, capacity);
    }
    return graph;
}

//This is a while-loop version of the getMinFlow function, which failed the requirements...
/*void getMinFlow(vector<vector<int>>& nodeParents, FlowGraph& graph, std::queue<size_t>& listEdges, int& minFlow, size_t lastEdge){

    int childNode, parentNode, flow, edgeID;

    vector<int> parent(2,-1);
    minFlow = graph.edges[lastEdge].capacity - graph.edges[lastEdge].flow; //residual flow
    listEdges.push(lastEdge);

    int childEdge = lastEdge;
    do{
        parent = nodeParents[childEdge];
        parentNode = parent[0];
        edgeID = parent[1];
        if (edgeID == -1){break;}
        listEdges.push(edgeID);
        flow = graph.edges[edgeID].capacity - graph.edges[edgeID].flow;
        if (flow < minFlow) {minFlow = flow;}
        childEdge = edgeID;
    }   while (parentNode != 0);
    //return minFlow;
}*/

int getMinFlow(vector<vector<int>>& nodeParents, FlowGraph& graph, std::queue<size_t>& listEdges, int& minFlow, int lastEdge){
    if (lastEdge == -1){return 0;}
    else {
        int flow;
        vector<int> parent(2);
        flow = graph.edges[lastEdge].capacity - graph.edges[lastEdge].flow; //residual flow
        if (flow < minFlow) {minFlow = flow;}
        listEdges.push(lastEdge);
        parent = nodeParents[lastEdge];
        lastEdge = parent[1];
        return getMinFlow(nodeParents, graph, listEdges, minFlow, lastEdge);
    }
}


void bfs(FlowGraph& graph, std::queue<size_t>& listEdges, int& minFlow, int& breaker) {

    std::queue<int> myQueue;
    vector<vector<int>> nodeParents(
        graph.edges.size(),
        vector<int> (2,-1));
    vector<int> visitedNode(graph.size(),0);
    int endNode = graph.size()-1;
    visitedNode[0] = 1;
    vector<size_t> connecting_edges = graph.get_ids(0);

    int fromNode, toNode, capacity, flow;
    size_t curEdgeID;

    for(int i = 0; i<connecting_edges.size(); i++){
        curEdgeID = connecting_edges[i];
        flow = graph.edges[curEdgeID].capacity - graph.edges[curEdgeID].flow; //defining the residual flow
        toNode = graph.edges[curEdgeID].to;

        if (flow == 0 || visitedNode[toNode]){ continue; }

        myQueue.push(curEdgeID);
        nodeParents[curEdgeID] = {0,-1};
    }

    int lastEdge;
    breaker = 1;

    while(!myQueue.empty()){

        curEdgeID = myQueue.front();
        myQueue.pop();

        toNode = graph.edges[curEdgeID].to;
        capacity = graph.edges[curEdgeID].capacity;
        flow = capacity - graph.edges[curEdgeID].flow;
        fromNode = graph.edges[curEdgeID].from;

        visitedNode[toNode] = 1;

        if(toNode == endNode){
            breaker = 0;
            lastEdge = curEdgeID;
            break;
        }

        connecting_edges = graph.get_ids(toNode);
        size_t childEdge;

        for(int i = 0; i<connecting_edges.size(); i++){
            childEdge = connecting_edges[i];
            toNode = graph.edges[childEdge].to;
            flow = graph.edges[childEdge].capacity - graph.edges[childEdge].flow;

            if (flow == 0 || visitedNode[toNode] ){ continue; }

            nodeParents[childEdge] = {fromNode,curEdgeID};
            myQueue.push(connecting_edges[i]);
        }
    }

    if(!breaker){
        minFlow = graph.edges[lastEdge].capacity  - graph.edges[lastEdge].flow;
        getMinFlow(nodeParents, graph, listEdges, minFlow, lastEdge);
    }
}


void addFlow(FlowGraph& graph, std::queue<size_t>& listEdges, int minFlow){
    while(!listEdges.empty()){
        size_t curEdgeID = listEdges.front();
        listEdges.pop();
        graph.add_flow(curEdgeID, minFlow);
    }
}

int max_flow(FlowGraph& graph) {
    int flow = 0;
    vector<size_t> connecting_edges = graph.get_ids(0);
    for (int i = 0; i<connecting_edges.size();i++){
        size_t edgeID = connecting_edges[i];
        flow = flow + graph.edges[edgeID].flow;
    }
    return flow;
}

int main() {
    std::ios_base::sync_with_stdio(false);
    FlowGraph graph = read_data();

    std::queue<size_t> listEdges;

    int breaker = 0;

    while (!breaker){
        int minFlow;
        bfs(graph, listEdges, minFlow, breaker);
        addFlow(graph, listEdges, minFlow);
    }

    std::cout << max_flow(graph) << "\n";

    return 0;
}
