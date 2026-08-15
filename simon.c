int seq[64];
int len;
int W;
int H;
int tone[4];

void draw_pads(int lit) {
    canvas_clear(18, 20, 28);
    int pw = W / 2 - 6;
    int ph = H / 2 - 6;
    for (int i = 0; i < 4; i++) {
        int x = (i % 2) * (pw + 8) + 4;
        int y = (i / 2) * (ph + 8) + 4;
        int r = 60; int g = 60; int b = 70;
        if (i == 0) { r = 200; g = 60; b = 60; }
        if (i == 1) { r = 60; g = 190; b = 90; }
        if (i == 2) { r = 70; g = 120; b = 220; }
        if (i == 3) { r = 220; g = 190; b = 60; }
        if (lit == i) { r = 255; g = 255; b = 255; }
        else { r = r / 2; g = g / 2; b = b / 2; }
        canvas_rect(x, y, pw, ph, r, g, b);
    }
    canvas_present();
}

void play_pad(int i, int ms) {
    draw_pads(i);
    beep(tone[i], ms);
    draw_pads(-1);
    sleep(60);
}

int key_to_pad(int k) {
    if (k == 202) return 0;
    if (k == 203) return 1;
    if (k == 200) return 2;
    if (k == 201) return 3;
    if (k == 113) return 0;
    if (k == 119) return 1;
    if (k == 97) return 2;
    if (k == 115) return 3;
    return -1;
}

int main() {
    canvas_init(180, 150);
    W = canvas_width();
    H = canvas_height();
    tone[0] = 330; tone[1] = 415; tone[2] = 494; tone[3] = 660;
    srand(ticks());
    len = 0;

    print_line("Simon -- repeat the sequence.");
    print_line("pads: left=red right=green up=blue down=yellow");
    print_line("      (or q w a s).  esc quits");
    draw_pads(-1);
    sleep(600);

    for (;;) {
        if (len < 64) { seq[len] = rand() % 4; len = len + 1; }

        print("Round ");
        print_int(len);
        put_char(10);
        sleep(400);
        for (int i = 0; i < len; i++) play_pad(seq[i], 260);

        for (int i = 0; i < len; i++) {
            int pad = -1;
            while (pad < 0) {
                int k = get_key();
                if (k == 27) { print_line("bye"); sound_off(); return 0; }
                pad = key_to_pad(k);
            }
            play_pad(pad, 140);
            if (pad != seq[i]) {
                beep(140, 400);
                print("Wrong. You reached round ");
                print_int(len);
                put_char(10);
                sound_off();
                return 0;
            }
        }
        beep(1000, 60);
        sleep(300);
    }
}
