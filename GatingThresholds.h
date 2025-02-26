#ifndef GATINGTHRESHOLDS_H
#define GATINGTHRESHOLDS_H

#include <vector>
#include "parser.h"
#include <iostream>

double GatingThresholds(double Pg, int M){
    std::vector<std::vector<double>> GT;
    get_vector_of_vector_double(GT, "../GatingThresholds.csv");
    int ind = Pg*1000;
    return GT[ind%100][M-1];        
}

#endif 