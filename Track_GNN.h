#include "Filter.h"
#include <vector>
#include <Eigen/Dense>
using Eigen::MatrixXd;
class Track_GNN
{
private:
    int branchID;
    int nLost;   
    int windowSize;
    int measIDHist;
    int numHist;
    Filter filter;
    std::vector<MatrixXd> hist;
    std::vector<double> histTime;
public:
    MatrixXd vec();
    MatrixXd cov();
    double tUpdate;
    int trackID;
    double score;
    bool isConfirmed = false;
    Track_GNN(){
    };
    void Predict(double t);
    void Update();
    void FillHistory();
    void Update(MatrixXd meas);
};