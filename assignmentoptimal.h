#include "helpfunctions.h"

std::pair<MatrixXd, double> assignmentoptimal(MatrixXd distMatrix);

MatrixXd buildassignmentvector(MatrixXd);

double computeassignmentcost(MatrixXd &, MatrixXd, int64_t);

MatrixXd step2(MatrixXd &, MatrixXd &, MatrixXd &,
               MatrixXd &, MatrixXd &, int64_t);

MatrixXd step3(MatrixXd &, MatrixXd &, MatrixXd &,
               MatrixXd &, MatrixXd &, int64_t);

MatrixXd step4(MatrixXd &, MatrixXd &, MatrixXd &,
               MatrixXd &, MatrixXd &, int64_t, int64_t, int64_t);

MatrixXd step5(MatrixXd &, MatrixXd &, MatrixXd &,
               MatrixXd &, MatrixXd &, int64_t);
