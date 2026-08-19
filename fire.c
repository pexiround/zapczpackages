int W;
int H;
char heat[9600];   /* values are 0..63, a byte each keeps us inside the 16 KB data limit */
int pal_r[64];
int pal_g[64];
int pal_b[64];

void build_palette() {
    for (int i = 0; i < 64; i++) {
        int r = i * 8; if (r > 255) r = 255;
        int g = (i - 16) * 8; if (g < 0) g = 0; if (g > 255) g = 255;
        int b = (i - 44) * 12; if (b < 0) b = 0; if (b > 255) b = 255;
        pal_r[i] = r; pal_g[i] = g; pal_b[i] = b;
    }
}

int main() {
    canvas_init(120, 80);
    W = canvas_width();
    H = canvas_height();
    build_palette();
    srand(ticks());

    for (int i = 0; i < W * H; i++) heat[i] = 0;

    print_line("Fire -- classic demoscene effect. esc to stop.");

    for (;;) {
        for (int x = 0; x < W; x++) heat[(H - 1) * W + x] = 32 + rand() % 32;

        for (int y = 0; y < H - 1; y++) {
            for (int x = 1; x < W - 1; x++) {
                int below = (y + 1) * W + x;
                int sum = heat[below] + heat[below - 1] + heat[below + 1];
                if (y + 2 < H) sum = sum + heat[(y + 2) * W + x];
                else sum = sum + heat[below];
                int v = sum / 4;
                v = v - (rand() % 2);
                if (v < 0) v = 0;
                heat[y * W + x] = v;
            }
        }

        for (int y = 0; y < H; y++) {
            for (int x = 0; x < W; x++) {
                int v = heat[y * W + x];
                if (v > 63) v = 63;
                plot(x, y, pal_r[v], pal_g[v], pal_b[v]);
            }
        }
        canvas_present();

        int k = key_down();
        if (k == 27) break;
    }
    print_line("done");
    return 0;
}
