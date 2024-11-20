#include <tuple>
#include <vector>
#include <Eigen/Dense>
#include <iostream>
using Eigen::MatrixXd;

int MahalDist(){
    MatrixXd a(2,2);
    MatrixXd b(2,2);
    a << 1,2,
         3,4;
    b << 5,6,
         7,8;
    MatrixXd c = a.transpose() * b.inverse() * a;
    MatrixXd d = a.transpose() * (b * a.inverse());
    std::cout << c << std::endl << std::endl << d;
}