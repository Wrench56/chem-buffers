#pragma once

#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

#include "constants.h"
#include "equations.h"

typedef enum {
    MODE_NORMAL,
    MODE_ACIDIC,
    MODE_BASIC,
    MODE_ICF
} IceMode;

typedef enum {
    STATUS_READY,
    STATUS_INCOMPLETE,
    STATUS_ERROR
} IceStatus;

extern float K_value;

bool is_icebox_solvable(float data[3][3], IceMode mode) {
    if (mode != MODE_NORMAL)
        return true;

    int num_initials = 0;
    int num_finals = 0;
    int num_known_both = 0;
    int num_unknowns = 0;

    for (int i = 0; i < NUM_SPECIES; i++) {
        bool has_initial = is_known(data[1][i]);
        bool has_final = is_known(data[2][i]);

        if (has_initial) num_initials++;
        if (has_final) num_finals++;
        if (!has_initial && !has_final) num_unknowns++;
        if (has_initial && has_final) num_known_both++;
    }

    bool has_K = is_known(K_value);

    // Rule 1: I for A, B, C with no K or finals : FAIL
    if (num_initials == NUM_SPECIES && num_finals == 0 && !has_K)
        return false;

    // Rule 2: I for A, B, C and K : OK
    if (num_initials == NUM_SPECIES && has_K)
        return true;

    // Rule 3: I for A, B, C and E for one species : OK
    if (num_initials == NUM_SPECIES && num_finals >= 1)
        return true;

    // Rule 4: I for one, E for other two : OK
    if (num_initials == 1 && num_finals == 2)
        return true;

    // Rule 5: E for all species only : OK
    if (num_initials == 0 && num_finals == NUM_SPECIES)
        return true;

    // Rule 6: I for A and K only : FAIL
    if (num_initials == 1 && num_finals == 0 && has_K)
        return false;

    // Rule 7: I for A, B and K : OK
    if (num_initials == 2 && has_K)
        return true;

    // Rule 8: I for A, B, no K, no E : FAIL
    if (num_initials == 2 && num_finals == 0 && !has_K)
        return false;

    // Rule 9: K and E for A, B only : OK
    if (num_initials == 0 && num_finals == 2 && has_K)
        return true;

    // Rule 10: I for A, B, C and E for one species : OK
    if (num_initials == NUM_SPECIES && num_finals >= 1)
        return true;

    // Rule 11: I for A, B and E for one product/reactant : OK
    if (num_initials == 2 && num_finals == 1)
        return true;

    // Rule 12: K and E for one species : FAIL
    if (num_initials == 0 && num_finals == 1 && has_K)
        return false;

    // Default: can't solve
    return false;
}

#include <stdbool.h>
#include <math.h>
#include <stdlib.h>

#define is_known(x) (!isnan(x))

extern float K_value;
extern float PH_value;
extern float Dissoc_value;

const char* solve_normal(float data[3][3]) {
    float *coeffs   = data[0];
    float *initials = data[1];
    float *finals   = data[2];

    // Case: All initials + K - solve for x using actual ICE expression
    if (is_known(initials[0]) && is_known(initials[1]) && is_known(initials[2]) &&
    !is_known(finals[0]) && !is_known(finals[1]) && !is_known(finals[2]) &&
    is_known(K_value)) {

        float a_i = initials[0], b_i = initials[1], c_i = initials[2];
        float a = coeffs[0], b = coeffs[1], c = coeffs[2];

        double x = pow((K_value * pow(a_i, a)) / (pow(b, b) * pow(c, c)), (1.0f / (b+c)));
        data[2][0] = a_i - a * (float) x;
        data[2][1] = b_i + b * (float) x;
        data[2][2] = c_i + c * (float) x;

        return "Solved using initials & K";
    }

    return "No solvable case found";
}


const char* solve_acidic(float data[3][3]) {
    float *coeffs   = data[0];
    float *initials = data[1];
    float *finals   = data[2];
    float a_i = initials[0], b_i = initials[1], c_i = initials[2];
    float a = coeffs[0], b = coeffs[1], c = coeffs[2];

    if (is_known(initials[0]) && is_known(initials[1]) && is_known(initials[2]) &&
    !is_known(finals[0]) && !is_known(finals[1]) && !is_known(finals[2]) &&
    is_known(K_value)) {

        double x = pow((K_value * pow(a_i, a)) / (pow(b, b) * pow(c, c)), (1.0f / (b+c)));
        data[2][0] = a_i - a * (float) x;
        data[2][1] = b_i + b * (float) x;
        data[2][2] = c_i + c * (float) x;

        PH_value = -log10f(data[2][1]);
        Dissoc_value = (data[2][1] / a_i) * 100;

        return "Solved using initials & K";
    }

    if (is_known(a_i) && is_known(Dissoc_value) && is_known(b_i) && is_known(c_i)) {
        double x = (Dissoc_value / 100) * initials[0];
        data[2][0] = a_i - a * (float) x;
        data[2][1] = b_i + b * (float) x;
        data[2][2] = c_i + c * (float) x;

        PH_value = -log10f(data[2][1]);
        K_value = (pow(data[2][1], b)*pow(data[2][2], c)) / pow(data[2][0], a);

        return "Solved using initials & \% Dissociation";
    }

    return "No solvable case found";
}

const char* solve_icf(float data[3][3]) {
    float *coeffs   = data[0];
    float *initials = data[1];
    float *finals   = data[2];
    float a_i = initials[0], b_i = initials[1], c_i = initials[2];
    float a = coeffs[0], b = coeffs[1], c = coeffs[2];

    if (is_known(initials[0]) && is_known(initials[1]) && is_known(initials[2]) && is_known(K_value)) {

        double x = MIN(initials[0]/a, initials[1]/b);
        data[2][0] = a_i - a * (float) x;
        data[2][1] = b_i - b * (float) x;
        data[2][2] = c_i + c * (float) x;

        // If equivalent
        if (fabs(data[2][0] - data[2][2]) < EPSILON) {
            PH_value = calc_pKa(K_value);
        } else {
            PH_value = -log10f(data[2][1]);
        }

        return "Solved using initials & K";
    }

    return "No solvable case found";
}

const char* icebox_solve_all(float data[3][3], IceMode mode) {
    switch (mode) {
        case MODE_NORMAL:
            return solve_normal(data);
        case MODE_ACIDIC:
            return solve_acidic(data);
        case MODE_ICF:
            return solve_icf(data);
        default:
            return "No solvable case found";
    }
}
