int sintab[256];
int W;
int H;

void build_sin() {
    for (int i = 0; i < 256; i++) {
        int x = i;
        if (x > 128) x = 256 - x;
        int v = (x * (128 - x));
        sintab[i] = (v * 1024) / 4096;
        if (i > 128) sintab[i] = -sintab[i];
    }
}

int isin(int a) {
    a = a & 255;
    return sintab[a];
}

int main() {
    canvas_init(200, 150);
    W = canvas_width();
    H = canvas_height();
    build_sin();

    print_line("Plasma -- press esc to stop, any other key to shift palette.");

    int t = 0;
    int shift = 0;
    for (;;) {
        for (int y = 0; y < H; y++) {
            for (int x = 0; x < W; x++) {
                int v = isin(x * 2 + t)
                      + isin(y * 3 - t)
                      + isin((x + y) + t * 2)
                      + isin((x - y) * 2 - t);
                v = (v / 64) + 128 + shift;
                int r = isin(v) / 8 + 128;
                int g = isin(v + 85) / 8 + 128;
                int b = isin(v + 170) / 8 + 128;
                if (r < 0) r = 0; if (r > 255) r = 255;
                if (g < 0) g = 0; if (g > 255) g = 255;
                if (b < 0) b = 0; if (b > 255) b = 255;
                plot(x, y, r, g, b);
            }
        }
        canvas_present();
        t = t + 3;
        int k = key_down();
        if (k == 27) break;
        if (k != 0) shift = shift + 24;
    }
    print_line("done");
    return 0;
}
