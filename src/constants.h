#pragma once

#include <math.h>

#define is_known(x) (!isnan(x))

#define NUM_SPECIES         3
#define NUM_ROWS            3
#define CELL_WIDTH          60
#define CELL_HEIGHT         20
#define TABLE_X             50
#define TABLE_Y             40
#define ENTER_LINE_Y        230
#define ROW_K               3
#define NUM_SELECTABLE_ROWS 4

const char *species[NUM_SPECIES] = { "A", "B", "C" };
char *rows[NUM_ROWS] = { "Coeff", "Initial", "Final" };
float data[NUM_ROWS][NUM_SPECIES] = {
    {1, 1, 1},
    {NAN, NAN, NAN},
    {NAN, NAN, NAN}
};
float K_value = NAN;

int selected_row = 0;
int selected_col = 0;
