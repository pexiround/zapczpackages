int W;
int H;
int cx;
int cy;
int zoom;
int maxit;

int pal_r[64];
int pal_g[64];
int pal_b[64];

void build_palette() {
    for (int i = 0; i < 64; i++) {
        int t = i * 4;
        pal_r[i] = (t * 3) % 256;
        pal_g[i] = (t * 5) % 256;
        pal_b[i] = 80 + (t * 2) % 176;
    }
    pal_r[63] = 0; pal_g[63] = 0; pal_b[63] = 0;
}

void render() {
    int half_w = W / 2;
    int half_h = H / 2;
    for (int py = 0; py < H; py++) {
        for (int px = 0; px < W; px++) {
            int c_re = cx + ((px - half_w) * zoom) / W;
            int c_im = cy + ((py - half_h) * zoom) / W;
            int zr = 0;
            int zi = 0;
            int it = 0;
            while (it < maxit) {
                int zr2 = (zr * zr) >> 12;
                int zi2 = (zi * zi) >> 12;
                if (zr2 + zi2 > 16384) break;
                int nzr = zr2 - zi2 + c_re;
                zi = ((zr * zi) >> 11) + c_im;
                zr = nzr;
                it = it + 1;
            }
            if (it >= maxit) plot(px, py, 0, 0, 0);
            else {
                int k = (it * 63) / maxit;
                plot(px, py, pal_r[k], pal_g[k], pal_b[k]);
            }
        }
        if ((py % 16) == 0) canvas_present();
    }
    canvas_present();
}

int main() {
    canvas_init(240, 180);
    W = canvas_width();
    H = canvas_height();
    build_palette();

    cx = -2048;
    cy = 0;
    zoom = 14336;
    maxit = 60;

    print_line("Mandelbrot explorer -- fixed point, no FPU.");
    print_line("arrows/WASD pan, +/- zoom, [ ] detail, r reset, esc quit");
    render();

    for (;;) {
        int k = get_key();
        if (k == 27) break;
        if (k == 119 || k == 87) k = 200;
        else if (k == 115 || k == 83) k = 201;
        else if (k == 97 || k == 65) k = 202;
        else if (k == 100 || k == 68) k = 203;
        int step = zoom / 8;
        if (k == 202) cx = cx - step;
        else if (k == 203) cx = cx + step;
        else if (k == 200) cy = cy - step;
        else if (k == 201) cy = cy + step;
        else if (k == 43) zoom = (zoom * 2) / 3;
        else if (k == 45) zoom = (zoom * 3) / 2;
        else if (k == 91) { maxit = maxit - 20; if (maxit < 20) maxit = 20; }
        else if (k == 93) { maxit = maxit + 20; if (maxit > 400) maxit = 400; }
        else if (k == 114) { cx = -2048; cy = 0; zoom = 14336; maxit = 60; }
        else continue;
        if (zoom < 8) zoom = 8;
        render();
    }
    print_line("done");
    return 0;
}
