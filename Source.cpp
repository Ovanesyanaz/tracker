#include "Source.h"

double MahalDist(MatrixXd dx, MatrixXd K)
{
    // d2 = dx' * inv(K) * dx;
    double d2 = (dx.transpose() * K.inverse() * dx)(0, 0);
    return d2;
}

std::tuple<MatrixXd, MatrixXd> GetPredictedMeas(MatrixXd x)
{
    MatrixXd z(1, 1);
    z << x(0, 0);
    MatrixXd H(1, 2);
    H << 1, 0;
    return {z, H};
}

std::tuple<MatrixXd, MatrixXd> GetMeasVector(std::vector<double> meas)
{
    MatrixXd y(1, 1);
    y << meas[0];
    return {y, MatrixXd::Constant(2, 2, 0)};
}