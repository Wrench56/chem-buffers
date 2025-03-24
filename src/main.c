#include "statusbar.h"
#include "input.h"

#define NUM_SPECIES  3
#define NUM_ROWS     3
#define CELL_WIDTH   60
#define CELL_HEIGHT  20
#define TABLE_X      50
#define TABLE_Y      40
#define ENTER_LINE_Y 230

const char *species[NUM_SPECIES] = { "A", "B", "C" };
const char *rows[NUM_ROWS] = { "Coeff", "Initial", "Final" };
float data[NUM_ROWS][NUM_SPECIES] = {
    {1, 1, 1},
    {0.0, 0.0, 0.0},
    {0.0, 0.0, 0.0}
};

int selected_row = 0;
int selected_col = 0;

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
            } else {
                sprintf(buf, "%.3f", data[row][col]);
            }
            gfx_PrintStringXY(buf, x + 2, y + 5);
        }
    }
}

int main(void) {
    gfx_Begin();

    IceMode mode = MODE_BASIC;
    IceStatus status = STATUS_INCOMPLETE;

    while (1) {
        // General draw
        gui_draw_table();
        draw_status_bar(mode, status);

        input_wait_key();
        kb_Scan();

        // Exit
        if (kb_IsDown(kb_KeyClear)) break;

        // Arrow keys
        if (kb_IsDown(kb_KeyDown) && selected_row < NUM_ROWS - 1)     selected_row++;
        if (kb_IsDown(kb_KeyUp)   && selected_row > 0)                selected_row--;
        if (kb_IsDown(kb_KeyRight) && selected_col < NUM_SPECIES - 1) selected_col++;
        if (kb_IsDown(kb_KeyLeft)  && selected_col > 0)               selected_col--;

        // Prompt input
        if (kb_IsDown(kb_KeyEnter)) {
            char prompt_msg[40];
            sprintf(prompt_msg, "%s's %s:", species[selected_col], rows[selected_row]);
            float val = prompt_value(prompt_msg);

            if (!isnan(val)) {
                data[selected_row][selected_col] = val;
            } else {
                draw_error_line("Invalid input. Press any key.");
                input_wait_key();
            }
        }

        // Clear field
        if (kb_IsDown(kb_KeyDel)) {
            if (selected_row == 0) {
                data[0][selected_col] = 0;
            } else {
                data[selected_row][selected_col] = 0.0f;
            }
        }
    }

    gfx_End();
    return 0;
}
