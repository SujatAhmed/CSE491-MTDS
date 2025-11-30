#include "../include/norm.h"
#include <cmath>
using namespace std;




float norm(int triCount, int num_nodes, float k) {
    float triDense = (float)triCount / (float)num_nodes;

    float maxTriDense = ((float)(num_nodes - 1) * (float)(num_nodes - 2)) / 6.0f;

    float num  = hOFx(k, triDense, maxTriDense) - hOFx(k, 0.0f, maxTriDense);
    float den  = hOFx(k, maxTriDense, maxTriDense) - hOFx(k, 0.0f, maxTriDense);

    float norm_density = num / den;

    return norm_density;
}

float hOFx(float k, float x, float M) {
    float h = 1.0f / (1.0f + expf(-k * (x - (M * 0.5f))));
    return h;
}
