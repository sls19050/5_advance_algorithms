#include <iostream>
#include <vector>
#include <algorithm>
#include <sys/resource.h>

void die(const std::string& msg) {
    std::cerr << msg << std::endl;
    exit(1);
}

struct Vertex {
    int weight;
    std::vector <int> children;
};
typedef std::vector<Vertex> Graph;
typedef std::vector<int> Sum;

using namespace std;

Graph ReadTree() {
    int vertices_count;
    std::cin >> vertices_count;

    Graph tree(vertices_count);

    for (int i = 0; i < vertices_count; ++i)
        std::cin >> tree[i].weight;

    for (int i = 1; i < vertices_count; ++i) {
        int from, to, weight;
        std::cin >> from >> to;
        tree[from - 1].children.push_back(to - 1);
        tree[to - 1].children.push_back(from - 1);
    }

    return tree;
}

void dfs(const Graph &tree, int vertex, int parent, std::vector<int>& Dv) {

    if (Dv[vertex] == -1){

    if(tree[vertex].children.size() == 1 && tree[vertex].children[0] == parent){
        Dv[vertex] = tree[vertex].weight;
    } else{
        int case1 = tree[vertex].weight;
        for (int child : tree[vertex].children){
            if (child != parent){
                for (int grandchild : tree[child].children){
                    if (grandchild != vertex){
                        dfs(tree, grandchild, child, Dv);
                        case1 = case1 + Dv[grandchild];
                    }
                }
            }
        }
        int case2 = 0;
        for (int child : tree[vertex].children){
            if (child != parent){
                dfs(tree, child, vertex, Dv);
                case2 = case2 + Dv[child];
            }
        }
        Dv[vertex] = max(case1, case2);
    }

    }

}

int MaxWeightIndependentTreeSubset(const Graph &tree) {
    size_t size = tree.size();
    std::vector<int> Dv(size, -1);
    if (size == 0)
        return 0;
    dfs(tree, 0, -1, Dv);
    return Dv[0];
}

int main() {
    // This code is here to increase the stack size to avoid stack overflow
    // in depth-first search.

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

    // Here begins the solution
    Graph tree = ReadTree();
    int weight = MaxWeightIndependentTreeSubset(tree);
    std::cout << weight << std::endl;
    return 0;
}
