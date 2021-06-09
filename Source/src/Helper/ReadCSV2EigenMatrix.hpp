#ifndef READCSV2EIGENMATRIX_HPP
#define READCSV2EIGENMATRIX_HPP

#include <iostream>
#include <fstream>
#include <Eigen/Dense>

Eigen::MatrixXd read_csv_as_eigen_matrix(const std::string a_File, const int a_Rows, const int a_Cols);

#endif //READCSV2EIGENMATRIX_HPP
