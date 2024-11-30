#ifndef TRACK_GNN_H
#define TRACK_GNN_H

#include "Filter.h"
#include <vector>
#include <Eigen/Dense>

using Eigen::MatrixXd;

class Track_GNN
{
private:
    int windowSize;
    int numHist;
    Filter filter;
    std::vector<MatrixXd> hist;
    std::vector<double> histTime;

public:      

    int nLost;  
    std::vector<int> measIDHist;
    int branchID;
    int trackID;
    MatrixXd vec();
    MatrixXd cov();
    double tUpdate;
    int trackID;
    double score;
    bool isConfirmed = false;
    Track_GNN();
    void Reset();
    void Predict(double t);
    void Update(std::vector<double>);
    void FillHistory();
    void Update(MatrixXd meas);
    void Init(double t,std::vector<double> meas);
};

#endif // TRACK_GNN_H