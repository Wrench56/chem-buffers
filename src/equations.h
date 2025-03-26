#include <math.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MIN3(a, b, c) (MIN(a, MIN(b, c)))

float calc_pKa(float K_value) {
    return -log10f(K_value);
}

float calc_henderson_hasselbach(float K_value, float A, float HA) {
    return calc_pKa(K_value) + log10f((A / HA));
}

float calc_pH(float H_plus) {
    return -log10f(H_plus);
}

float calc_pOH_from_pH(float pH) {
    return 14 - pH;
}
