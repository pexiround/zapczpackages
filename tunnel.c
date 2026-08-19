int W;
int H;
char dist_tab[7040];   /* 0..255 fits a byte; the 16 KB data cap is the binding limit */
char ang_tab[7040];
int sintab[256];

void build_sin() {
    for (int i = 0; i < 128; i++) {
        int v = (4 * i * (128 - i) * 1024) / 16384;
        sintab[i] = v;
        sintab[i + 128] = -v;
    }
}
int isin(int a) { return sintab[a & 255]; }

int isqrt_i(int v) {
    if (v <= 0) return 0;
    int x = v; int y = (x + 1) / 2;
    while (y < x) { x = y; y = (x + v / x) / 2; }
    return x;
}

int iatan(int y, int x) {
    /* 0..255 around the circle, good enough for a texture lookup */
    int ax = x; if (ax < 0) ax = -ax;
    int ay = y; if (ay < 0) ay = -ay;
    int a;
    if (ax + ay == 0) a = 0;
    else a = (32 * ay) / (ax + ay);
    if (x >= 0 && y >= 0) return a;
    if (x < 0 && y >= 0) return 64 - a;
    if (x < 0 && y < 0) return 128 + a;
    return 192 - a;
}

int main() {
    canvas_init(110, 64);
    W = canvas_width();
    H = canvas_height();
    build_sin();

    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            int dx = x - W / 2;
            int dy = y - H / 2;
            int d = isqrt_i(dx * dx + dy * dy);
            if (d < 1) d = 1;
            dist_tab[y * W + x] = (3000 / d) & 255;
            ang_tab[y * W + x] = iatan(dy, dx) & 255;
        }
    }

    print_line("Tunnel -- precomputed polar mapping. esc to stop.");

    int t = 0;
    for (;;) {
        for (int y = 0; y < H; y++) {
            for (int x = 0; x < W; x++) {
                int i = y * W + x;
                int u = (dist_tab[i] + t) & 255;
                int v = (ang_tab[i] + t / 2) & 255;
                int checker = ((u / 16) + (v / 16)) & 1;
                int shade = 255 - dist_tab[i];
                if (shade < 0) shade = 0;
                int r = checker ? (shade * 240) / 255 : (shade * 60) / 255;
                int g = checker ? (shade * 120) / 255 : (shade * 70) / 255;
                int b = checker ? (shade * 60) / 255 : (shade * 200) / 255;
                plot(x, y, r, g, b);
            }
        }
        canvas_present();
        t = t + 4;
        int k = key_down();
        if (k == 27) break;
    }
    print_line("done");
    return 0;
}
