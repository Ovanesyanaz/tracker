#include <tuple>
#include <vector>
#include <Eigen/Dense>
#include <iostream>
using Eigen::MatrixXd;

class Filter
{
public:
    MatrixXd vec;
    MatrixXd cov;
    double tUpd;
    void Predict();
    void Update();
};