#pragma once

#include "constants.h"

typedef enum {
    MODE_NORMAL,
    MODE_ACIDIC,
    MODE_BASIC
} IceMode;

typedef enum {
    STATUS_READY,
    STATUS_INCOMPLETE,
    STATUS_ERROR
} IceStatus;

#include <math.h>
#include <stdbool.h>
#include "constants.h"

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
