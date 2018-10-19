#include <algorithm>
#include <iostream>
#include <vector>
#include <cstdio>
#include <stdio.h>
using namespace std;

typedef vector<vector<double>> matrix;

//define function to get subsets
//void combinUtil(vector<int> mySet, vector<int> resultSets, int start, int last, int index, int m);

void die(const std::string& msg) {
    std::cerr << msg << std::endl;
    exit(1);
}

void combinUtil(vector<int> mySet, vector<int> resultSets, int start, int last, int index, int m){
    if (index == m){
        for (int i=0; i<m; i++){
            cout<<resultSets[i]<<" ";
        }
        cout<<"\n";
    } else {
        for (int j=start; j<=last && last-j+1 >= m-index; j++){
            resultSets[index] = mySet[j];
            combinUtil(mySet, resultSets, j+1, last, index+1, m);
        }
    }
}

void printCombin(vector<int> mySet, int n, int m){
    vector<int> resultSets (m);
    cout<<"printing combin:\n";
    combinUtil(mySet, resultSets, 0, n-1, 0, m);
}

pair<int, vector<double>> solve_diet_problem(
    int n,
    int m,
    matrix A,
    vector<double> b,
    vector<double> c) {

    // solve subsets of equations for all vertices
    vector<int> rowID(n+m+1);
    std::iota(std::begin(rowID),std::end(rowID),0 );
    printCombin(rowID, n+m+1, m);
    die("line 47");
  return {0, vector<double>(m, 0)};
}

int main(){
  int n, m;
  cin >> n >> m;
  matrix A(n, vector<double>(m));
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < m; j++) {
      cin >> A[i][j];
    }
  }
  vector<double> b(n);
  for (int i = 0; i < n; i++) {
    cin >> b[i];
  }
  vector<double> c(m);
  for (int i = 0; i < m; i++) {
    cin >> c[i];
  }

  pair<int, vector<double>> ans = solve_diet_problem(n, m, A, b, c);

  switch (ans.first) {
    case -1:
      printf("No solution\n");
      break;
    case 0:
      printf("Bounded solution\n");
      for (int i = 0; i < m; i++) {
        printf("%.18f%c", ans.second[i], " \n"[i + 1 == m]);
      }
      break;
    case 1:
      printf("Infinity\n");
      break;
  }
  return 0;
}
