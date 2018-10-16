#include <cmath>
#include <iostream>
#include <vector>

const double EPS = 1e-6;
const int PRECISION = 20;

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
    int mostLeftCol = 100;
    int rowIDmLC = 0;
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
        SwapLines(a, b, used_rows, pivot_element);
        ProcessPivotElement(a, b, pivot_element);
        MarkPivotElementUsed(pivot_element, used_rows, used_columns);
    }

    return b;
}

void PrintColumn(const Column &column) {
    int size = column.size();
    std::cout.precision(PRECISION);
    for (int row = 0; row < size; ++row)
        std::cout << column[row] << std::endl;
}

int main() {
    Equation equation = ReadEquation();
    Column solution = SolveEquation(equation);
    PrintColumn(solution);
    return 0;
}
