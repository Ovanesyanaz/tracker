#include "Track_GNN.h"

Track_GNN::Track_GNN()
{
    this->score = -1;
}

void Track_GNN::Predict(double t)
{
    this->filter.Predict();
}

MatrixXd Track_GNN::vec()
{
    return this->filter.vec;
}

MatrixXd Track_GNN::cov()
{
    return this->filter.cov;
}

void Track_GNN::FillHistory()
{
    this->hist.push_back(this->filter.vec);
    this->histTime.push_back(this->filter.tUpd);
}

void Track_GNN::Update(std::vector<double> meas)
{
    this->filter.Update();
    this->FillHistory();
}

    
    // function this = Reset(this)
    //   this.trackID = 0;
    //   this.branchID = 0;
    //   this.filter = Reset(this.filter);
    //   this.hist = Reset(this.hist);
    //   this.histTime = Reset(this.histTime);
    //   this.nLost = 0;
    //   this.isConfirmed = false;
    // end
void Track_GNN::Reset()
{
    this->trackID = 0;
    this->branchID = 0;
    this->hist.clear();
    this->histTime.clear();
    this->hist.clear();
    this->nLost = 0;
    this->isConfirmed = false;
}

    // function this = Init(this,t, meas)
    //   %Initiate new track
    //   this = Reset(this);
      
    //   %init tracking filter
    //   if nargin == 3
    //     this.filter = Init(this.filter,t, meas);
    //   elseif nargin == 4
    //     this.filter = Init(this.filter, meas, p2cVec, p2cMat);
    //   end
      
    //   %add to hist
    //   this = FillHistory(this);
    // end

void Track_GNN::Init(double t, std::vector<double>meas){
    this->Reset();
    this->filter.Init(t, meas);
    FillHistory();
}