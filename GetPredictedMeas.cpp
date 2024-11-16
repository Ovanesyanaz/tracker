#include <tuple>
#include <vector>
#include <Eigen/Dense>
#include <iostream>
using Eigen::MatrixXd;
std::tuple<MatrixXd, MatrixXd> GetPredictedMeas(MatrixXd x){
    MatrixXd z(1,1);
    z << x(0,0);
    MatrixXd H(1,2);
    H << 1, 0;
    return {z, H};
}

int main(){
    MatrixXd x(1,2);
    x << 10,20;
    auto [z, H] = GetPredictedMeas(x);
    std::cout << z << std::endl << H;
}