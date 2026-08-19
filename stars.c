int W;
int H;
int sx[220];
int sy[220];
int sz[220];
int N;

int main() {
    canvas_init(200, 150);
    W = canvas_width();
    H = canvas_height();
    N = 200;
    srand(ticks());

    for (int i = 0; i < N; i++) {
        sx[i] = (rand() % 4000) - 2000;
        sy[i] = (rand() % 4000) - 2000;
        sz[i] = 1 + rand() % 1000;
    }

    print_line("Starfield -- flying through space. esc to stop.");
    int speed = 8;

    for (;;) {
        canvas_clear(4, 5, 10);
        for (int i = 0; i < N; i++) {
            sz[i] = sz[i] - speed;
            if (sz[i] < 1) {
                sx[i] = (rand() % 4000) - 2000;
                sy[i] = (rand() % 4000) - 2000;
                sz[i] = 1000;
            }
            int px = W / 2 + (sx[i] * 100) / sz[i];
            int py = H / 2 + (sy[i] * 100) / sz[i];
            if (px < 0 || py < 0 || px >= W || py >= H) continue;
            int b = 255 - (sz[i] / 4);
            if (b < 40) b = 40;
            if (b > 255) b = 255;
            plot(px, py, b, b, b);
            if (sz[i] < 400) {
                plot(px + 1, py, b, b, b);
                plot(px, py + 1, b, b, b);
            }
        }
        canvas_present();
        int k = key_down();
        if (k == 27) break;
        if (k == 200 || k == 119) { speed = speed + 2; if (speed > 40) speed = 40; }
        if (k == 201 || k == 115) { speed = speed - 2; if (speed < 2) speed = 2; }
    }
    print_line("done");
    return 0;
}
