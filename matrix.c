int W;
int H;
int col_y[120];
int col_speed[120];
int col_len[120];
int cols;
int CW;
int CH;

int main() {
    canvas_init(200, 140);
    W = canvas_width();
    H = canvas_height();
    CW = 4;
    CH = 6;
    cols = W / CW;
    if (cols > 120) cols = 120;
    srand(ticks());

    for (int i = 0; i < cols; i++) {
        col_y[i] = rand() % (H / CH);
        col_speed[i] = 1 + rand() % 3;
        col_len[i] = 5 + rand() % 12;
    }

    print_line("Matrix rain -- esc to stop.");
    canvas_clear(0, 0, 0);

    for (;;) {
        /* fade the whole screen slightly instead of clearing, so trails persist */
        for (int y = 0; y < H; y = y + 1) {
            for (int x = 0; x < W; x = x + 3) {
                canvas_rect(x, y, 3, 1, 0, 0, 0);
                y = y;
            }
        }

        for (int c = 0; c < cols; c++) {
            int head = col_y[c];
            for (int k = 0; k < col_len[c]; k++) {
                int row = head - k;
                if (row < 0) continue;
                if (row * CH >= H) continue;
                int g = 255 - (k * 220) / col_len[c];
                if (g < 0) g = 0;
                int r = (k == 0) ? 200 : g / 4;
                int b = (k == 0) ? 255 : g / 5;
                int gg = (k == 0) ? 255 : g;
                /* a blocky glyph: a few lit cells inside the character box */
                int seed = (c * 31 + row * 17 + k) & 7;
                canvas_rect(c * CW, row * CH, CW - 1, CH - 1, r / 3, gg / 3, b / 3);
                canvas_rect(c * CW, row * CH + (seed & 3), CW - 1, 2, r, gg, b);
            }
            col_y[c] = col_y[c] + col_speed[c];
            if ((col_y[c] - col_len[c]) * CH > H) {
                col_y[c] = 0;
                col_speed[c] = 1 + rand() % 3;
                col_len[c] = 5 + rand() % 12;
            }
        }
        canvas_present();
        int k2 = key_down();
        if (k2 == 27) break;
        sleep(40);
    }
    print_line("done");
    return 0;
}
