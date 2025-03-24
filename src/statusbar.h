#pragma once

#include <graphx.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <tice.h>

#include "icebox.h"

void draw_status_bar(IceMode mode, IceStatus status) {
    gfx_SetColor(200);
    gfx_FillRectangle(0, 0, LCD_WIDTH, 10);
    gfx_SetTextFGColor(0);
    gfx_SetTextBGColor(200);

    // Mode string
    const char *mode_str = "Normal";
    if (mode == MODE_ACIDIC) mode_str = "Acidic";
    else if (mode == MODE_BASIC) mode_str = "Basic";

    // Status string
    const char *status_str = "Incomplete";
    if (status == STATUS_READY) status_str = "Ready";
    else if (status == STATUS_ERROR) status_str = "Error";

    char left_buf[20];
    snprintf(left_buf, sizeof(left_buf), "Mode: %s", mode_str);
    gfx_PrintStringXY(left_buf, 2, 1);

    int status_x = LCD_WIDTH - (strlen(status_str) * 8) - 4;
    gfx_PrintStringXY(status_str, status_x, 1);
}
