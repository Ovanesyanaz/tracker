#pragma once
#include <Eigen/Dense>
#include <iostream>
#include <cmath>

using Eigen::MatrixXd;

// HELP FUNCTIONS

std::vector<int64_t> find_indexes_rows_in_cols(MatrixXd matrix, Eigen::ArrayXd arr)
{
    std::vector<int64_t> res;

    for (int col = 0; col < matrix.cols(); col++)
    {
        for (int row = 0; row < matrix.rows(); row++)
        {
            if (matrix(row, col) == arr[col])
                res.push_back(row);
        }
    }
    return res;
}

MatrixXd matrix_eq_col_mask(MatrixXd matrix, MatrixXd col_mask, double value, int64_t col)
{

    MatrixXd res = matrix;

    for (int i = 0; i < col_mask.rows(); i++)
    {
        if (col_mask(i) == 1)
            res(i, col) = value;
    }
    return res;
}

MatrixXd matrix_eq_row_mask(MatrixXd matrix, MatrixXd row_mask, double value, int64_t row)
{

    MatrixXd res = matrix;

    for (int i = 0; i < row_mask.rows(); i++)
    {
        if (row_mask(i) == 1)
            res(i, row) = value;
    }
    return res;
}

MatrixXd row_index_eq(MatrixXd matrix, int64_t row, double value)
{
    for (int col = 0; col < matrix.cols(); col++)
    {
        matrix(row, col) = value;
    }
    return matrix;
}

MatrixXd col_index_eq(MatrixXd matrix, int64_t col, double value)
{
    for (int row = 0; row < matrix.rows(); row++)
    {
        matrix(row, col) = value;
    }
    return matrix;
}

MatrixXd getColsMatrix(MatrixXd m, std::vector<int> ind){
     MatrixXd answ(m.rows(), ind.size());
     int j = 0;
     for (int i:ind){
          answ.col(j) = m.col(i);
          j = j + 1;
     }
     return answ.matrix();
}
// HELP FUNCTIONS