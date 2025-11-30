#include "../include/norm.h"
#include <iostream>
#include <ostream>
#include <vector>
#include <cmath>
using namespace std;

float norm(
    int triDense,
    int num_nodes,
    float k
) {
    float norm_density;
    float maxTriDense;
    maxTriDense = ((num_nodes - 1) * (num_nodes - 2)) / 6; 
    float num = hOFx(k, triDense, maxTriDense) - hOFx(k, 0, maxTriDense); 
    float denum = hOFx(k, maxTriDense, maxTriDense) - hOFx(k, 0, maxTriDense); 
    float norm_density = num / denum;

    return norm_density;
}

float hOFx (int k, int x, int M) {
    float hOFx = 1 / (1 + exp( -k * (x - (M/2))));
    return hOFx;
}


