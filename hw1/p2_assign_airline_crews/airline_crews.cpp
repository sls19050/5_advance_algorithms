#include <iostream>
#include <vector>
#include <algorithm>
#include <memory>

using std::vector;
using std::cin;
using std::cout;


void die(const std::string& msg) {
    std::cerr << msg << std::endl;
    exit(1);
}

class MaxMatching {

 public:

  struct Edge {
    int from, to, capacity, flow;
  };

  /* List of all - forward and backward - edges */
  vector<Edge> edges;

  /* These adjacency lists store only indices of edges in the edges list */
  vector<vector<int> > graph;

  /* for printing out answer */
  vector<int> answer;

  void Solve() {
    ReadData();
    vector<int> matching = FindMatching();
    WriteResponse(matching);
  }

 private:
  void add_edge(int from, int to) {
    /* Note that we first append a forward edge and then a backward edge,
    * so all forward edges are stored at even indices (starting from 0),
    * whereas backward edges are stored at odd indices in the list edges */
    Edge forward_edge = {from, to, 1, 0};
    Edge backward_edge = {to, from, 0, 0};
    graph[from].push_back(edges.size());
    edges.push_back(forward_edge);
    graph[to].push_back(edges.size());
    edges.push_back(backward_edge);
  }

  void ReadData() {
    int num_left, num_right;
    cin >> num_left >> num_right;
    answer.resize(num_left);
    //graph.resize(num_left+num_right+2); //resize to total number of nodes
    //vector<vector<bool>> adj_matrix(num_left, vector<bool>(num_right));
    graph.resize(2+num_left+num_right);

    for (int i = 1; i <= num_left; ++i) {
        add_edge(0, i);
    }

    for (int i = 1; i <= num_left; ++i) {
      for (int j = num_left+1; j <= num_left+num_right; ++j) {
        int bit;
        cin >> bit;
        //adj_matrix[i][j] = (bit == 1);
        if (bit){
          //cout<<"adding edge i,j:"<<i<<" "<<j<<"\n";
          add_edge(i, j);
        }
      }
    }

    for (int j = num_left+1; j <= num_left+num_right; ++j) {
        add_edge(j, num_left+num_right+1);
    }
  }

  void WriteResponse(const vector<int>& matching) {
    for (int i = 0; i < matching.size(); ++i) {
        cout << matching[i]<<" ";
    }
    cout << "\n";
  }

  void showEdge(vector<Edge> edges, int j){
    std::cout << edges[j].from <<" ";
    std::cout << edges[j].to <<" ";
    std::cout << edges[j].capacity <<" ";
    std::cout << edges[j].flow <<"\n";
  }

  void showGraph( vector<vector<int> > graph, vector<Edge> edges){
    std::cout << "show graph edges: \n";
    for(int j = 0; j<edges.size();j++){
        std::cout<<"this is j = "<<j<<"\n";
        showEdge(edges, j);
    }
    std::cout << "show connecting_edges \n";
    for(int j = 0; j<graph.size();j++){
        std::cout<<"\n connecting edges from node j="<<j<<": ";
        vector<int> connecting_edges = graph[j];
        for (int i = 0; i<connecting_edges.size();i++){
            std::cout<<connecting_edges[i]<<" ";
        }
    }
    std::cout<<"\n";
  }

  void showTrack(vector<int> trackEdges){
    cout<< "trackEdges: ";
    for (int i = 0; i<trackEdges.size(); ++i){
        cout<<trackEdges[i]<<" \n";
    }
  }

  void add_flow(int id) {
   /* To get a backward edge for a true forward edge (i.e id is even), we should get id + 1
    * due to the described above scheme. On the other hand, when we have to get a "backward"
    * edge for a backward edge (i.e. get a forward edge for backward - id is odd), id - 1
    * should be taken.
    *
    * It turns out that id ^ 1 works for both cases. Think this through! */
    edges[id].flow++;
    edges[id ^ 1].flow--;
  }

  void updateGraph(vector<int>& trackEdges,  int curEdge){
      //die("line135");
      if (curEdge != -1){
        add_flow(curEdge);
        int newEdge = trackEdges[curEdge];
        //cout<<"newEdge = "<< newEdge<<"\n";
        updateGraph(trackEdges, newEdge);
      }
  }

  void dfs( int oldEdge, int curNode, vector<int>& visited, vector<int>& trackEdges, int& breaker, int& lastEdge) {
    //die("line 121");
    visited[curNode] = 1;
    //cout<<"curNode = "<<curNode<<"\n";
    //cout<<"breaker = "<<breaker<<"\n";
    //cout<<"true to break? : "<<curNode - graph.size() + 1<<"\n";
    if(!breaker){
        if(curNode == graph.size()-1){
            //cout<<"breaking!!!!!!!!!!!!!!\n";
            lastEdge = oldEdge;
            breaker = 1;
        }
        for (int i = 0; i<graph[curNode].size(); ++i){
            int edgeID = graph[curNode][i];
            int endNode = edges[edgeID].to;
            int resiFlow = edges[edgeID].capacity-edges[edgeID].flow;
            //cout<<"save edge:"<<edgeID<<"\n";
            trackEdges[edgeID] = oldEdge;

            if (visited[endNode] || !resiFlow) {continue;}
            dfs(edgeID, endNode, visited, trackEdges, breaker, lastEdge);
        }
    }
  }
  vector<int> returnMatch(){

      for(int i = 0; i<answer.size(); ++i){
        bool noEdge = true;
        for (int j = 0; j<graph[i+1].size();++j){
          int edgeID = graph[i+1][j];
          int toNode = edges[edgeID].to;
          if(edges[edgeID].flow == 1){
            answer[i] = toNode-answer.size();
            noEdge = false;
            continue;
          }
        }
        if (noEdge){answer[i] = -1;}
      }
      return answer;
  }
  vector<int> FindMatching() {
    int breaker = 1;
    while (breaker) {
        vector<int> trackEdges(edges.size(),-1);

        vector<int> visited( graph.size(),0);

        visited[0] = 1;
        breaker = 0;
        int lastEdge;
        dfs(-1, 0, visited, trackEdges, breaker, lastEdge);
        //cout<<"checkVal = "<< checkVal<<"\n";
        //cout<<"lastEdge = "<<lastEdge<<"\n";
        updateGraph(trackEdges, lastEdge);
        //cout<<"done update graph\n";
        //showTrack(trackEdges);
    }

    return returnMatch();
  }
};

int main() {
  std::ios_base::sync_with_stdio(false);
  MaxMatching max_matching;
  max_matching.Solve();
  return 0;
}
