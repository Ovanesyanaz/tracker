#include <tuple>
#include <vector>
#include <Eigen/Dense>
#include <iostream>
using Eigen::MatrixXd;

double MahalDist(MatrixXd dx, MatrixXd K){
    // d2 = dx' * inv(K) * dx;
    double d2 = (dx.transpose() * K.inverse() * dx)(0,0);
    return d2;
}