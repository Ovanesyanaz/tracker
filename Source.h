#ifndef SOURCE_H
#define SOURCE_H

#include <vector>
#include <tuple>
#include <cmath>
#include <Eigen/Dense>

using Eigen::MatrixXd;

double MahalDist(MatrixXd dx, MatrixXd K);

std::tuple<MatrixXd, MatrixXd> GetPredictedMeas(MatrixXd meas);

std::tuple<MatrixXd, MatrixXd> GetMeasVector(std::vector<double> meas);

std::tuple<double, double> SPRT_thresholds(double falseTrackConfirmProb, double trueTrackDeletionProb, double initScore);

double ScoreIncrement(bool isTrackUpdated, double Pd, double betaFA, double M, double detS, double d2);

#endif // SOURCE_H