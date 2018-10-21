#include <algorithm>
#include <iostream>
#include <vector>
#include <cstdio>
#include <stdio.h>
#include <cmath>
#include <cstdint>
#include <limits>
#include <math.h>

using namespace std;

const double EPS = 1e-3;
const int PRECISION = 20;
const double MAX_VAL = 1e9;
const double MIN_DOUBLE = -std::numeric_limits<double>::max();


typedef vector<vector<double>> matrix;
typedef std::vector<double> Column;
typedef std::vector<double> Row;
typedef std::vector<Row> Matrix;



void die(const std::string& msg) {
    std::cerr << msg << std::endl;
    exit(1);
}

struct Equation {
    Equation(const Matrix &a, const Column &b):
        a(a),
        b(b)
    {}

    Matrix a;
    Column b;

    public:
    void showEq(){
        std::cout<<"showEq:\n";
        for (int i = 0; i<a.size(); i++){
            for(int j=0; j<a[i].size();j++){
                std::cout<<a[i][j]<<" ";
            }
            std::cout<<"| "<<b[i]<<"\n";
        }
    }
};

struct Position {
    Position(int column, int row):
        column(column),
        row(row)
    {}

    int column;
    int row;
};

Equation ReadEquation() {
    int size;
    std::cin >> size;
    Matrix a(size, std::vector <double> (size, 0.0));
    Column b(size, 0.0);
    for (int row = 0; row < size; ++row) {
        for (int column = 0; column < size; ++column)
            std::cin >> a[row][column];
        std::cin >> b[row];
    }
    return Equation(a, b);
}

Position SelectPivotElement(
    const Matrix &a,
    std::vector <bool> &used_rows,
    std::vector <bool> &used_columns) {
    int row, column;
    row = 0;
    column = 0;

    while (used_rows[row])
        ++row;
    while (used_columns[column])
        ++column;

    //scan the rest of the matrix and find the row with the left-most element
    int mostLeftCol = std::numeric_limits<std::int32_t>::max();
    int rowIDmLC = -1; // this -1 value is critical to handling cases of no solution
    for (int i = row; i<a.size(); i++){
        for(int j = column; j<a[i].size(); j++){
            double curEle = a[i][j];
            if (curEle != 0.0 && j < mostLeftCol) {
                rowIDmLC = i;
                mostLeftCol = j;
                break;
            }
        }
    }
    Position pivot_element(mostLeftCol, rowIDmLC);
    return pivot_element;
}

void SwapLines(Matrix &a, Column &b, std::vector <bool> &used_rows, Position &pivot_element) {
    std::swap(a[pivot_element.column], a[pivot_element.row]);
    std::swap(b[pivot_element.column], b[pivot_element.row]);
    std::swap(used_rows[pivot_element.column], used_rows[pivot_element.row]);
    pivot_element.row = pivot_element.column;
}

void ProcessPivotElement(Matrix &a, Column &b, const Position &pivot_element) {

    //Rescale Pivot Row
    double denominator = a[pivot_element.row][pivot_element.column];
    for (int i = 0; i<a[pivot_element.row].size(); i++){
        a[pivot_element.row][i] = a[pivot_element.row][i] / denominator;
    }
    b[pivot_element.row] = b[pivot_element.row] / denominator;

    //use pivot to subtract all rows below pivot
    for (int i = pivot_element.row+1; i<a.size(); i++){
        double multiplier = a[i][pivot_element.column];
        for (int j = pivot_element.column; j<a[i].size(); j++){
            a[i][j] = a[i][j] - multiplier * a[pivot_element.row][j];
        }
        b[i] = b[i] - multiplier * b[pivot_element.row];
    }

    //use pivot to subtract all rows above pivot
    for (int i = pivot_element.row-1; i>=0; i--){
        double multiplier = a[i][pivot_element.column];
        for (int j = pivot_element.column; j<a[i].size(); j++){
            a[i][j] = a[i][j] - multiplier * a[pivot_element.row][j];
        }
        b[i] = b[i] - multiplier * b[pivot_element.row];
    }
}

void MarkPivotElementUsed(const Position &pivot_element, std::vector <bool> &used_rows, std::vector <bool> &used_columns) {
    used_rows[pivot_element.row] = true;
    used_columns[pivot_element.column] = true;
}

Column SolveEquation(Equation equation) {
    Matrix &a = equation.a;
    Column &b = equation.b;
    int size = a.size();

    std::vector <bool> used_columns(size, false);
    std::vector <bool> used_rows(size, false);
    for (int step = 0; step < size; ++step) {
        Position pivot_element = SelectPivotElement(a, used_rows, used_columns);
        //handle case when there is no solution (due to a degenerate equation or impossible to solve)
        if (pivot_element.row == -1){
            // returns an empty vector when no solution
            Column emptyVec;
            return {emptyVec};
        }
        SwapLines(a, b, used_rows, pivot_element);
        ProcessPivotElement(a, b, pivot_element);
        MarkPivotElementUsed(pivot_element, used_rows, used_columns);
    }

    return b;
}

/*    to check if the calculated solution satisfy all inequalities    */
bool passNInequal (Column tempAns, int n, matrix &A, vector<double>& b){

    for (int i = 0; i<A.size(); i++){

        double tempSum = 0;

        for (int j = 0; j<tempAns.size(); j++){
            tempSum = tempSum + A[i][j] * tempAns[j];
        }

        if (tempSum > b[i] + EPS){
            return false;
        }
    }

    return true;
}

/*    recursively solve for all subsets of inequalities    */
void combinUtil(vector<int> mySet, vector<int> resultSets, int start, int last, int index, int m, matrix& A, vector<double>& b, vector<double>& resultVec, double& resultVal, vector<double>& c){
    if (index == m){
        Matrix tempA;
        Column tempB(m, 0.0);

        for (int i=0; i<m; i++){
            tempA.push_back(A[resultSets[i]]);
            tempB[i] = b[resultSets[i]];
        }

        Equation tempEq(tempA, tempB);
        Column tempAns = SolveEquation(tempEq);
        double tempVal = 0.0;

        if (!tempAns.empty()){
            for (int i = 0; i< c.size(); i++){
                tempVal = tempVal + tempAns[i] * c[i];
            }
            //only keep the vertex that 1) passed all inequalities, and 2) has the greatest value
            if ( passNInequal(tempAns,b.size()-m-1, A, b) && tempVal > resultVal){
                resultVec = tempAns;
                resultVal = tempVal;
            }
        }
    } else {
        for (int j=start; j<=last && last-j+1 >= m-index; j++){
            resultSets[index] = mySet[j];
            combinUtil(mySet, resultSets, j+1, last, index+1, m, A, b, resultVec, resultVal, c);
        }
    }
}

/*    return solution after solving all combinations of inequalities    */
pair<int, vector<double>> solveCombin(vector<int> mySet, int n, int m, matrix& A, vector<double>& b, vector<double>& c){
    vector<int> resultSets (m);
    vector<double> resultVec;
    double resultVal = MIN_DOUBLE;
    combinUtil(mySet, resultSets, 0, n-1, 0, m, A, b, resultVec, resultVal, c);

    int caseID = 0;
    if (resultVal == MIN_DOUBLE){
        caseID = -1;
    } else {
        double sumAmt;

        for (int i = 0; i<resultVec.size(); i++){
            sumAmt += resultVec[i];
        }

        if (sumAmt >= MAX_VAL - EPS){
            caseID = 1;
        }
    }
    return {caseID, resultVec};
}

pair<int, vector<double>> solve_diet_problem(
    int n,
    int m,
    matrix A,
    vector<double> b,
    vector<double> c) {

    // create rowID to help generate subsets
    vector<int> rowID(n+m+1);
    std::iota(std::begin(rowID),std::end(rowID),0 );

    // return the optimal answer
  return solveCombin(rowID, n+m+1, m, A, b, c);
}

int main(){
  int n, m;
  cin >> n >> m;
  matrix A(n+m+1, vector<double>(m));
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < m; j++) {
      cin >> A[i][j];
    }
  }
  vector<double> b(n+m+1);
  for (int i = 0; i < n; i++) {
    cin >> b[i];
  }
  vector<double> c(m);
  for (int i = 0; i < m; i++) {
    cin >> c[i];
  }

  //add auxilary equations to martix A and vector B
  for (int i = n; i < n+m; i++) {
    b[i] = 0.0;
    for (int j = 0; j < m; j++) {
      if (i-n == j) {
        A[i][j] = -1.0;
      } else{
        A[i][j] = 0.0;
      }
    }
  }
  // add infinity bound
  b[n+m] = MAX_VAL;
  for (int j = 0; j < m; j++) {
      A[n+m][j] = 1.0;
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
