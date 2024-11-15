class Filter
{
public:
    MatrixXd vec;
    MatrixXd cov;
    double tUpd;
    void Predict();
    void Update();
};