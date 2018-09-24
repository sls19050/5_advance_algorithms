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

//private:
public:
    /* List of all - forward and backward - edges */
    vector<Edge> edges;

    /* These adjacency lists store only indices of edges in the edges list */
    vector<vector<int> > graph;

public:
    explicit FlowGraph(int n): graph(n) {}

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

    int size() const {
        return graph.size();
    }

    const vector<int>& get_ids(int from) const {
        return graph[from];
    }


    const Edge& get_edge(int id) const {
        return edges[id];
    }

    void add_flow(int id, int flow) {
        /* To get a backward edge for a true forward edge (i.e id is even), we should get id + 1
         * due to the described above scheme. On the other hand, when we have to get a "backward"
         * edge for a backward edge (i.e. get a forward edge for backward - id is odd), id - 1
         * should be taken.
         *
         * It turns out that id ^ 1 works for both cases. Think this through! */
        edges[id].flow += flow;
        //edges[id ^ 1].flow -= flow;
    }

    void compResi(){

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
        showEdge(graph, j);
    }
    std::cout << "show connecting_edges \n";
    for(int j = 0; j<graph.size();j++){
        std::cout<<"\n connecting edges from node j="<<j<<": ";
        vector<int> connecting_edges = graph.get_ids(j);
        for (int i = 0; i<connecting_edges.size();i++){
            std::cout<<connecting_edges[i]<<" ";
        }
    }
    std::cout<<"\n";
}

void showDict(std::map<int, std::tuple<int,int>> nodeParents){
    for(auto elem : nodeParents){
        std::tuple<int,int> dummy = elem.second;
        std::cout <<"child, parent = "<< elem.first <<", "<< std::get<0>(dummy) << " " << std::get<1>(dummy)<<"\n";
        //std::cout <<"child, parent = "<< elem.first <<"\n";
        //std::cout<<std::get<1>(dummy)<<"\n";
    }
}

FlowGraph getResi(FlowGraph& graph) {
    FlowGraph resiGraph = graph;
    for(int j = 0; j<graph.edges.size();j++){
        //showEdge(graph, j);
        resiGraph.edges[j].flow = graph.edges[j].capacity - graph.edges[j].flow;
        j++;
        resiGraph.edges[j].flow = graph.edges[j].flow;
    }
    //std::cout<<"show resi Graph\n";
    //showGraph(resiGraph);
    return resiGraph;
}

FlowGraph read_data() {
    int vertex_count, edge_count;
    std::cin >> vertex_count >> edge_count;
    FlowGraph graph(vertex_count);
    for (int i = 0; i < edge_count; ++i) {
        int u, v, capacity;
        std::cin >> u >> v >> capacity;
        graph.add_edge(u - 1, v - 1, capacity);
    }
    return graph;
}

void getMinFlow(std::map<int, std::tuple<int,int>>& nodeParents, FlowGraph& resiGraph, std::queue<int>& listEdges, int& minFlow ){
    int endNode = resiGraph.size()-1;
    int childNode, parentNode, flow, edgeID;
    std::tuple<int,int> parent;
    childNode = endNode;
    minFlow = 2147483647;
    do{
        parent = nodeParents[childNode];
        parentNode = std::get<0>(parent);
        edgeID = std::get<1>(parent);
        listEdges.push(edgeID);
        flow = resiGraph.edges[edgeID].flow;
        if (flow < minFlow) {minFlow = flow;}
        childNode = parentNode;
    }   while (parentNode != 0);
    //return minFlow;
}

void bfs(FlowGraph resiGraph, FlowGraph graph, std::queue<int>& listEdges, int& minFlow, int& breaker) {
    breaker = 1;
    //std::cout<<"\nstart_bfs\n";
    std::queue<int> myQueue;
    //std::vector<int> nodeLevels(graph.size(),2147483647);
    std::map<int, std::tuple<int,int>> nodeParents;
    vector<int> visited(resiGraph.size(),0);
    int endNode = resiGraph.size()-1;
    //nodeLevels[0] = 0;

    visited[0]=1;
    vector<int> connecting_edges = resiGraph.get_ids(0);
    //std::cout<<"connecting_edges size = "<<connecting_edges.size()<<"\n";

    int curEdgeID, fromNode, toNode, capacity, flow;
    for(int i = 0; i<connecting_edges.size(); i++){
        curEdgeID = connecting_edges[i];
        flow = resiGraph.edges[curEdgeID].flow;
        toNode = resiGraph.edges[curEdgeID].to;
        //std::cout<<"Did i continue? ";
        if (flow == 0 || visited[toNode]){
            //std::cout<<"flow = 0\n";
            continue;
        }
        //std::cout<<"flow>0\n";

        //std::cout<<"toNode = "<<toNode<<"\n";
        myQueue.push(curEdgeID);
        nodeParents[toNode] = std::make_tuple(0,curEdgeID);

    }
    //die("line 145");
    while(!myQueue.empty()){
        curEdgeID = myQueue.front();
        myQueue.pop();
        //std::cout<<"curEdgeID = "<<curEdgeID<<"\n";
        toNode = resiGraph.edges[curEdgeID].to;
        capacity = resiGraph.edges[curEdgeID].capacity;
        flow = resiGraph.edges[curEdgeID].flow;
        fromNode = resiGraph.edges[curEdgeID].from;
        //std::cout<<"toNode, flow = "<<toNode<<", "<<flow<<"\n";

        nodeParents[toNode] = std::make_tuple(fromNode,curEdgeID);

        //showDict(nodeParents);
        visited[toNode] = 1;
        if(toNode == endNode){
            //std::cout<<"breaking\n";
            breaker = 0;
            break;
        }
        connecting_edges = resiGraph.get_ids(toNode);
        for(int i = 0; i<connecting_edges.size(); i++){
            curEdgeID = connecting_edges[i];
            flow = resiGraph.edges[curEdgeID].flow;
            toNode = resiGraph.edges[curEdgeID].to;
            if (flow == 0 || visited[toNode]){
                //std::cout<<"flow = 0\n";
                continue;
            }
            myQueue.push(connecting_edges[i]);
        }

    }

    //showDict(nodeParents);
    if(!breaker){
        getMinFlow(nodeParents, resiGraph, listEdges, minFlow);
    }
    //std::cout<<"minFlow = "<< minFlow<<"\n";
}


void addFlow(FlowGraph& graph, std::queue<int>& listEdges, int minFlow){
    while(!listEdges.empty()){
        int curEdgeID = listEdges.front();
        listEdges.pop();
        //std::cout<<"adding edge, flow = "<< curEdgeID<<" "<<minFlow<<"\n";
        graph.add_flow(curEdgeID, minFlow);
    }
}

int max_flow(FlowGraph& graph) {
    int flow = 0;
    vector<int> connecting_edges = graph.get_ids(0);
    for (int i = 0; i<connecting_edges.size();i++){
        flow = flow + graph.edges[connecting_edges[i]].flow;
    }
    return flow;
}

int main() {
    std::ios_base::sync_with_stdio(false);
    FlowGraph graph = read_data();

    //showGraph(graph);
    FlowGraph resiGraph = getResi(graph);

    std::queue<int> listEdges;
    int minFlow = 0;
    int breaker = 0;

    bfs(resiGraph, graph, listEdges, minFlow, breaker);
    addFlow(graph, listEdges, minFlow);
    //showGraph(graph);
    resiGraph = getResi(graph);

    while (!breaker){
        bfs(resiGraph, graph, listEdges, minFlow, breaker);
        addFlow(graph, listEdges, minFlow);
        //showGraph(graph);
        resiGraph = getResi(graph);
    }

    std::cout << max_flow(graph) << "\n";
    return 0;
}
