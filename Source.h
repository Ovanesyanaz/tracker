#ifndef SOURCE_H
#define SOURCE_H

#include <vector>
#include <tuple>
#include <Eigen/Dense>

using Eigen::MatrixXd;

double MahalDist(MatrixXd dx, MatrixXd K);

std::tuple<MatrixXd, MatrixXd> GetPredictedMeas(MatrixXd meas) {};

std::tuple<MatrixXd, MatrixXd> GetMeasVector(std::vector<double> meas) {};

#endif // SOURCE_H