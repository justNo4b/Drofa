#ifndef KPNN_H
#define KPNN_H

#include <math.h>


// use net Pnet => 128x16x2 => opS /egS outputs
#define N_INPUTS   (64 * 2)
#define N_HIDDEN   (8)


// net stuff
double OUTPUT_WEIGHTS[N_HIDDEN] = {0};


double HIDDEN_WEIGHTS[N_INPUTS * N_HIDDEN] = {0};




double sigmoid(double x){
    return 1 / (1 + exp(-1 * x));
}

#endif