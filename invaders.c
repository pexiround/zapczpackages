int W;
int H;
int ex[40];
int ey[40];
int ealive[40];
int ecount;
int edir;
int px;
int bx;
int by;
int bactive;
int score;
int lives;
int wave;

void spawn_wave() {
    ecount = 0;
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 8; c++) {
            ex[ecount] = 16 + c * 22;
            ey[ecount] = 12 + r * 16;
            ealive[ecount] = 1;
            ecount = ecount + 1;
        }
    }
    edir = 2;
}

void draw() {
    canvas_clear(8, 10, 18);
    for (int i = 0; i < ecount; i++) {
        if (!ealive[i]) continue;
        int row = ey[i] / 16;
        int r = 220; int g = 90; int b = 90;
        if (row == 1) { r = 220; g = 170; b = 70; }
        if (row == 2) { r = 110; g = 210; b = 120; }
        if (row >= 3) { r = 110; g = 160; b = 240; }
        canvas_rect(ex[i], ey[i], 12, 8, r, g, b);
        canvas_rect(ex[i] + 2, ey[i] - 2, 8, 2, r, g, b);
    }
    canvas_rect(px, H - 12, 18, 6, 120, 220, 255);
    canvas_rect(px + 7, H - 16, 4, 4, 200, 240, 255);
    if (bactive) canvas_rect(bx, by, 2, 6, 255, 240, 140);
    canvas_present();
}

int main() {
    canvas_init(220, 170);
    W = canvas_width();
    H = canvas_height();
    px = W / 2 - 9;
    score = 0; lives = 3; wave = 1;
    bactive = 0;
    spawn_wave();

    print_line("Invaders -- A/D or arrows to move, space to fire, esc quits");

    for (;;) {
        int k = key_down();
        if (k == 27) break;
        if (k == 97 || k == 65 || k == 202) px = px - 8;
        if (k == 100 || k == 68 || k == 203) px = px + 8;
        if (k == 32 && !bactive) {
            bactive = 1; bx = px + 8; by = H - 18;
            beep(1400, 8);
        }
        if (px < 0) px = 0;
        if (px > W - 18) px = W - 18;

        if (bactive) {
            by = by - 5;
            if (by < 0) bactive = 0;
            else {
                for (int i = 0; i < ecount; i++) {
                    if (!ealive[i]) continue;
                    if (bx >= ex[i] && bx <= ex[i] + 12 && by >= ey[i] && by <= ey[i] + 8) {
                        ealive[i] = 0;
                        bactive = 0;
                        score = score + 10;
                        beep(500, 12);
                        i = ecount;
                    }
                }
            }
        }

        int left = 0;
        int hit_edge = 0;
        for (int i = 0; i < ecount; i++) {
            if (!ealive[i]) continue;
            left = left + 1;
            if (ex[i] + edir < 0 || ex[i] + edir > W - 12) hit_edge = 1;
        }

        if (left == 0) {
            wave = wave + 1;
            score = score + 50;
            print("Wave "); print_int(wave); print_line(" incoming");
            beep(900, 60);
            spawn_wave();
        }

        if (hit_edge) {
            edir = -edir;
            for (int i = 0; i < ecount; i++) ey[i] = ey[i] + 6;
        } else {
            for (int i = 0; i < ecount; i++) ex[i] = ex[i] + edir;
        }

        for (int i = 0; i < ecount; i++) {
            if (!ealive[i]) continue;
            if (ey[i] + 8 >= H - 14) {
                lives = lives - 1;
                beep(160, 200);
                if (lives <= 0) {
                    print("Game over. Score "); print_int(score); put_char(10);
                    sound_off();
                    return 0;
                }
                spawn_wave();
                i = ecount;
            }
        }

        draw();
        sleep(30);
    }
    print("Final score "); print_int(score); put_char(10);
    sound_off();
    return 0;
}
