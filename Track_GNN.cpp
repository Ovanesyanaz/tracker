#include "Track_GNN.h"

Track_GNN::Track_GNN()
{
    this->score = -1;
}

void Track_GNN::Predict(double t)
{
    this->filter.Predict(t);
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

void Track_GNN::Update(double t, std::vector<double> meas)
{
    this->filter.Update(t, meas);
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

void Track_GNN::Init(double t, std::vector<double>meas){
    this->Reset();
    this->filter.Init(t, meas);
    FillHistory();
}