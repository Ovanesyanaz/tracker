#include "Filter.h"
#include "KalmanFilter.h"
void Filter::Predict(double t){
    double dt = t - this->t;
    if (dt < 1e-6){
        return;
    }
    this->t = t;
    this->F(0,1) = dt;
    double dt2 = dt*dt;
    this->Q(0,0) = dt*dt2 / 3;
    this->Q(0,1) = dt2/2;
    this->Q(1,0) = this->Q(0,1);
    this->Q(1,1) = dt;

    this->vec = this->F * this->vec;

    this->cov = this->F * this->cov * this->F.transpose() + this->diffus * this->Q;
}

void Filter::Update(double t, std::vector<double> meas){
    double r = meas[0];
    double vr = meas[2];
    double sig_r = meas[1];
    double sig_vr = meas[3];
    if (t - this->t > 1e-6){
        Predict(t);
    }
    this->tUpd = t;

    this->y(0) = r;
    this->y(1) = vr;
    this->z = this->vec;

    this->R(0,0) = sig_r * sig_r;
    this->R(1,1) = sig_vr * sig_vr;

    auto [x, P] = KalmanFilter(this->vec, this->cov, this->H, this->z, this->y, this->R);
    this->vec = x;
    this->cov = P;
}

void Filter::Init(double t, std::vector<double> meas){
    this->Construct();
    this->vec(0,0) = meas[0];
    this->vec(1,0) = meas[2];

    this->cov(0,0) = meas[1] * meas[1];
    this->cov(1,1) = meas[3] * meas[3];
    this->cov(1,0) = 0;
    this->cov(0,1) = 0;

    this->t = t;
    this->tUpd = t;
}

void Filter::Construct(){
    this->vec = MatrixXd::Zero(2,1);
    this->cov = MatrixXd::Zero(2,2);
    this->F = MatrixXd::Identity(2,2);
    this->H = MatrixXd::Identity(2,2);
    this->R = MatrixXd::Identity(2,2);
    this->y = MatrixXd::Zero(2,1);
    this->z = MatrixXd::Zero(2,1);
    this->Q = MatrixXd::Zero(2,2);
}