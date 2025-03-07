#ifndef FILTER_H
#define FILTER_H
#include <tuple>
#include <vector>
#include <iostream>
#include <Eigen/Dense>
using Eigen::MatrixXd;

class Filter
{
public:
    int deg = 1;
    int dim = 2;
    double diffus = 0.1;
    double t;
    MatrixXd vec;
    MatrixXd cov;
    double tUpd;
    MatrixXd F;
    MatrixXd H;
    MatrixXd R;
    MatrixXd y;
    MatrixXd z;
    MatrixXd Q;
    void Init(double t, std::vector<double> meas);
    void Predict(double t);
    void Update(double t, std::vector<double> meas);
    void Construct();
};
#endif //FILTER_H