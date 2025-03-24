#pragma once

#include <tice.h>
#include <ti/screen.h>
#include <keypadc.h>
#include <string.h>
#include <math.h>

/* Needed for prompt_value */
#include <graphx.h>

/* Prompt the user */
void* prompt(const char *prompt) {
    char* input = (char*) malloc(20);
    os_NewLine();
    os_GetStringInput(prompt, input, sizeof(input));
    return input;
}

/* Blocking key input */
uint8_t input_wait_key(void) {
    while (1) {
        kb_Scan();
        for (uint8_t i = 1; i < 8; i++) {
            if (kb_Data[i]) return kb_Data[i];
        }
        delay(10);
    }
}

/* Prompt user for a float using the enter line */
float prompt_value(const char *prompt) {
    /* Interrupt gfx rendering */
    gfx_End();

    char buf[20] = {0};
    os_GetStringInput(prompt, buf, sizeof(buf));

    if (strlen(buf) == 0) return NAN;

    char *endptr;
    float val = strtof(buf, &endptr);
    if (*endptr != '\0') return NAN;

    /* Init gfx again */
    gfx_Begin();
    return val;
}
