#include "Source.h"
#include <math.h>
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

std::tuple<double, double> SPRT_thresholds(double falseTrackConfirmProb, double trueTrackDeletionProb, double initScore)
{
    double t1, t2;
    double a = falseTrackConfirmProb;
    double b = trueTrackDeletionProb;

    t1 = log(b / (1 - a)) + initScore;
    t2 = log((1 - b) / a) + initScore;
    return {t1, t2};
}

double ScoreIncrement(bool isTrackUpdated, double Pd, double betaFA, double M, double detS, double d2)
{
    double dLLR;
    if (isTrackUpdated)
    {
        dLLR = log(Pd / betaFA / (pow((2 * 3.14), (M/2))) / sqrt(detS)) - d2 / 2;
        if (dLLR < 0)
        {
            std::cerr << "Score increment is negative" << std::endl;
            exit(1);
        }
    }
    else
    {
        dLLR = log(1 - Pd);
    }
    return dLLR;
}
