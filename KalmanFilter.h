#include <iostream>
#include <Eigen/Dense>
using Eigen::MatrixXd;

std::tuple<MatrixXd, MatrixXd> KalmanFilter( MatrixXd& x_pred,
                                            MatrixXd& P_pred, 
                                            MatrixXd& H, 
                                            MatrixXd& z, 
                                            MatrixXd& y, 
                                            MatrixXd& R)
{
    MatrixXd S = H * P_pred * H.transpose() + R;
    MatrixXd K = P_pred * H.transpose() * S.inverse();
    MatrixXd x = x_pred + K * (y - z);
    MatrixXd I = MatrixXd::Identity(x_pred.size(), x_pred.size());
    MatrixXd P = (I - K*H) * P_pred;
    return {x,P};
}