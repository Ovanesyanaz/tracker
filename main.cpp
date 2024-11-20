#include "Track_GNN.h"
#include <tuple>
#include <vector>
#include <Eigen/Dense>
#include <iostream>
using Eigen::MatrixXd;
int main(){
    Track_GNN Trc = Track_GNN();
    Trc.isConfirmed = true;
    std::cout << Trc.isConfirmed; 
}