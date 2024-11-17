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
    MatrixXd vec();
    MatrixXd cov();
    int score = -1;
    bool isConfirmed = false;
    Track_GNN(){
    };
    void Predict(double t){};
    void Update(){};
    void FillHistory(){};
    void Update(MatrixXd meas){};
};