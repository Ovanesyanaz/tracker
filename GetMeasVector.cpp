#include <tuple>
#include <vector>
#include <Eigen/Dense>
#include <iostream>
using Eigen::MatrixXd;
std::tuple<MatrixXd, MatrixXd> GetMeasVector(std::vector<double> meas){
    MatrixXd y(1,1);
    y << meas[0];
    return {y, MatrixXd::Constant(2,2,0)};
}

int main(){
    std::vector<double> meas{1,0};
    const auto [y, R] = GetMeasVector(meas);
    std::cout << y << std::endl << R;
}