#include <keypadc.h>

#include "statusbar.h"
#include "icebox.h"
#include "input.h"
#include "constants.h"

/* Draw a message at the bottom line */
void draw_error_line(const char *msg) {
    gfx_SetColor(255);
    gfx_FillRectangle(0, ENTER_LINE_Y, LCD_WIDTH, 10);
    gfx_SetTextFGColor(0);
    gfx_PrintStringXY(msg, 2, ENTER_LINE_Y);
}

/* Draw the main ICE table GUI */
void gui_draw_table(void) {
    gfx_FillScreen(255);
    gfx_SetTextFGColor(0);
    gfx_SetTextBGColor(255);
    gfx_SetTextTransparentColor(255);

    for (int col = 0; col < NUM_SPECIES; col++) {
        int x = TABLE_X + col * CELL_WIDTH;
        gfx_PrintStringXY(species[col], x + CELL_WIDTH / 2 - 4, TABLE_Y - 15);
    }

    for (int row = 0; row < NUM_ROWS; row++) {
        int y = TABLE_Y + row * CELL_HEIGHT;
        gfx_PrintStringXY(rows[row], TABLE_X - 45, y + 5);

        for (int col = 0; col < NUM_SPECIES; col++) {
            int x = TABLE_X + col * CELL_WIDTH;

            if (row == selected_row && col == selected_col) {
                gfx_SetColor(200);
                gfx_FillRectangle(x, y, CELL_WIDTH, CELL_HEIGHT);
                gfx_SetTextFGColor(0);
            } else {
                gfx_SetColor(0);
                gfx_Rectangle(x, y, CELL_WIDTH, CELL_HEIGHT);
            }

            char buf[12];
            if (row == 0) {
                sprintf(buf, "%i", (int)data[row][col]);
            } else if (isnan(data[row][col])) {
                strcpy(buf, " ");
            } else {
                sprintf(buf, "%.3f", data[row][col]);
            }
            gfx_PrintStringXY(buf, x + 2, y + 5);
        }
    }
}

void gui_draw_vars(IceMode mode) {
    
    int y = TABLE_Y + NUM_ROWS * CELL_HEIGHT + 5;
    int x = TABLE_X + CELL_WIDTH / 2;

    // Draw K
    gfx_PrintStringXY("K =", TABLE_X - 45, y);
    if (selected_row == ROW_K) {
        gfx_SetColor(200);
        gfx_FillRectangle(x - 2, y - 2, 70, 12);
        gfx_SetTextFGColor(0);
    }
    char buf[16];
    if (isnan(K_value)) {
        strcpy(buf, " ");
    } else {
        sprintf(buf, "%.5f", K_value);
    }
    gfx_PrintStringXY(buf, x, y);
    
    if (mode != MODE_ACIDIC && mode != MODE_BASIC) return;

    y += 12;
    gfx_PrintStringXY("pH =", TABLE_X - 45, y);
    if (selected_row == ROW_PH) {
        gfx_SetColor(200);
        gfx_FillRectangle(x - 2, y - 2, 70, 12);
        gfx_SetTextFGColor(0);
    }
    if (isnan(PH_value)) {
        strcpy(buf, " ");
    } else {
        sprintf(buf, "%.2f", PH_value);
    }
    gfx_PrintStringXY(buf, x, y);

    y += 12;
    gfx_PrintStringXY("Dissoc %% =", TABLE_X - 45, y);
    if (selected_row == ROW_DISS) {
        gfx_SetColor(200);
        gfx_FillRectangle(x - 2, y - 2, 70, 12);
        gfx_SetTextFGColor(0);
    }
    if (isnan(Dissoc_value)) {
        strcpy(buf, " ");
    } else {
        sprintf(buf, "%.1f%%", Dissoc_value);
    }
    gfx_PrintStringXY(buf, x, y);
    
}

void gui_draw(IceMode mode) {
    gui_draw_table();
    gui_draw_vars(mode);
}

int main(void) {
    gfx_Begin();

    IceMode mode = MODE_NORMAL;
    IceStatus status = STATUS_INCOMPLETE;

    while (1) {
        // General draw
        gui_draw(mode);
        draw_status_bar(mode, status);

        // Logic
        status = is_icebox_solvable(data, mode)
            ? STATUS_READY
            : STATUS_INCOMPLETE;

        input_wait_key();
        kb_Scan();

        // Exit
        if (kb_IsDown(kb_KeyClear)) break;

        // Arrow keys
        if (kb_IsDown(kb_KeyDown) && selected_row < NUM_SELECTABLE_ROWS - 1) selected_row++;
        if (kb_IsDown(kb_KeyUp)   && selected_row > 0)                       selected_row--;
        if (kb_IsDown(kb_KeyRight) && selected_col < NUM_SPECIES - 1)        selected_col++;
        if (kb_IsDown(kb_KeyLeft)  && selected_col > 0)                      selected_col--;

        // Prompt input
        if (kb_IsDown(kb_KeyEnter)) {
            float val = NAN;
        
            if (selected_row == ROW_K) {
                val = prompt_value("Enter K:");
                if (!isnan(val)) {
                    K_value = val;
                } else {
                    gfx_Begin();
                    gui_draw(mode);
                    draw_error_line("Invalid input. Press any key.");
                    input_wait_key();
                }
            } else if (selected_row == ROW_PH) {
                val = prompt_value("Enter pH:");
                if (!isnan(val)) {
                    PH_value = val;
                } else {
                    gfx_Begin();
                    gui_draw(mode);
                    draw_error_line("Invalid input. Press any key.");
                    input_wait_key();
                }
            } else if (selected_row == ROW_DISS) {
                val = prompt_value("Enter % dissociation:");
                if (!isnan(val)) {
                    Dissoc_value = val;
                } else {
                    gfx_Begin();
                    gui_draw(mode);
                    draw_error_line("Invalid input. Press any key.");
                    input_wait_key();
                }
            } else {
                char prompt_msg[40];
                sprintf(prompt_msg, "%s's %s:", species[selected_col], rows[selected_row]);
                val = prompt_value(prompt_msg);
        
                if (!isnan(val)) {
                    data[selected_row][selected_col] = val;
                } else {
                    gfx_Begin();
                    gui_draw(mode);
                    draw_error_line("Invalid input. Press any key.");
                    input_wait_key();
                }
            }
        }

        // Solve
        if (kb_IsDown(kb_KeyAdd)) {
            const char *msg = icebox_solve_all(data, mode);
            gui_draw(mode);
            draw_error_line(msg);
            input_wait_key();
        }

        // Clear field
        if (kb_IsDown(kb_KeyDel)) {
            if (selected_row == ROW_K) K_value = NAN;
            else if (selected_row == ROW_PH) PH_value = NAN;
            else if (selected_row == ROW_DISS) Dissoc_value = NAN;
            else data[selected_row][selected_col] = NAN;
        }

        // Mode
        if (kb_IsDown(kb_KeyAlpha)) {
            mode = (mode + 1) % 3;
            switch (mode) {
                case MODE_NORMAL:
                    species[0] = "A  ";
                    species[1] = "B  ";
                    species[2] = "C  ";
                    break;
                case MODE_ACIDIC:
                    species[0] = "HA";
                    species[1] = "H+ ";
                    species[2] = "OH-";
                    break;
                case MODE_BASIC:
                    species[0] = "B  ";
                    species[1] = "BH+";
                    species[2] = "OH-";
                    break;
            }
        }


        delay(200);
    }

    gfx_End();
    return 0;
}
