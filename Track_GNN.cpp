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

void Track_GNN::Update(MatrixXd meas)
{
    this->filter.Update();
    this->FillHistory();
}
