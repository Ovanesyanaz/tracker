#include "Filter.h"
#include <vector>
#include <Eigen/Dense>
using Eigen::MatrixXd;
class Track_GNN
{
private:
    int branchID = 0;
    int trackID = 0;
    int nLost = 0;   
    int windowSize = 8;
    int measIDHist;
    int numHist = 250;
    Filter filter;
    std::vector<MatrixXd> hist;
    std::vector<double> histTime;
public:    
    bool isConfirmed = false;
    int score = -1;
    Track_GNN(){

    };
    void Predict(double t){
        this->filter.Predict();
    }

    MatrixXd vec(){
        return this->filter.vec;
    }

    MatrixXd cov(){
        return this->filter.cov;
    }

    void FillHistory(){
        this->hist.push_back(this->filter.vec);
        this->histTime.push_back(this->filter.tUpd);
    };
    
    void Update(MatrixXd meas){
        this->filter.Update();
        this->FillHistory();
    };
};