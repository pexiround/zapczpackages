int MAPW;
int MAPH;
int map[192];
int W;
int H;

int px;
int py;
int ang;

int sintab[256];

void build_sin() {
    for (int i = 0; i < 128; i++) {
        int v = (4 * i * (128 - i) * 1024) / 16384;
        sintab[i] = v;
        sintab[i + 128] = -v;
    }
}

int isin(int a) { return sintab[a & 255]; }
int icos(int a) { return sintab[(a + 64) & 255]; }

void build_map() {
    MAPW = 16;
    MAPH = 12;
    for (int y = 0; y < MAPH; y++) {
        for (int x = 0; x < MAPW; x++) {
            int wall = 0;
            if (x == 0 || y == 0 || x == MAPW - 1 || y == MAPH - 1) wall = 1;
            map[y * MAPW + x] = wall;
        }
    }
    map[3 * MAPW + 4] = 2; map[3 * MAPW + 5] = 2; map[3 * MAPW + 6] = 2;
    map[6 * MAPW + 9] = 3; map[7 * MAPW + 9] = 3; map[8 * MAPW + 9] = 3;
    map[5 * MAPW + 2] = 4; map[6 * MAPW + 2] = 4;
    map[9 * MAPW + 12] = 2; map[9 * MAPW + 13] = 2;
    map[2 * MAPW + 11] = 3; map[3 * MAPW + 11] = 3; map[4 * MAPW + 11] = 3;
}

int cell_at(int gx, int gy) {
    if (gx < 0 || gy < 0 || gx >= MAPW || gy >= MAPH) return 1;
    return map[gy * MAPW + gx];
}

int plot_col_r;
int plot_col_g;
int plot_col_b;

void wall_color(int kind, int side, int shade) {
    int r = 200; int g = 200; int b = 200;
    if (kind == 1) { r = 190; g = 190; b = 200; }
    else if (kind == 2) { r = 210; g = 110; b = 90; }
    else if (kind == 3) { r = 90; g = 190; b = 120; }
    else { r = 110; g = 140; b = 230; }
    if (side) { r = (r * 6) / 10; g = (g * 6) / 10; b = (b * 6) / 10; }
    r = (r * shade) / 256; g = (g * shade) / 256; b = (b * shade) / 256;
    if (r < 0) r = 0; if (g < 0) g = 0; if (b < 0) b = 0;
    plot_col_r = r; plot_col_g = g; plot_col_b = b;
}

void render() {
    int half = H / 2;
    canvas_rect(0, 0, W, half, 58, 64, 92);
    canvas_rect(0, half, W, H - half, 44, 40, 48);

    for (int col = 0; col < W; col++) {
        int off = ((col - W / 2) * 48) / W;
        int ra = (ang + off) & 255;
        int dx = icos(ra);
        int dy = isin(ra);

        int rx = px;
        int ry = py;
        int hit = 0;
        int side = 0;
        int dist = 0;
        int steps = 0;

        int prev_gx = rx >> 10;
        int prev_gy = ry >> 10;

        while (!hit && steps < 640) {
            rx = rx + (dx >> 4);
            ry = ry + (dy >> 4);
            steps = steps + 1;
            int gx = rx >> 10;
            int gy = ry >> 10;
            int c = cell_at(gx, gy);
            if (c) {
                hit = c;
                if (gx != prev_gx) side = 0; else side = 1;
            }
            prev_gx = gx;
            prev_gy = gy;
        }

        dist = steps * 64;
        int corr = icos(off & 255);
        if (corr < 64) corr = 64;
        dist = (dist * corr) >> 10;
        if (dist < 64) dist = 64;

        int lineh = (H * 2048) / dist;
        if (lineh > H) lineh = H;
        int y0 = half - lineh / 2;

        int shade = 288 - (dist / 64);
        if (shade > 256) shade = 256;
        if (shade < 70) shade = 70;

        wall_color(hit, side, shade);
        canvas_rect(col, y0, 1, lineh, plot_col_r, plot_col_g, plot_col_b);
    }
    canvas_present();
}

int can_walk(int nx, int ny) {
    return cell_at(nx >> 10, ny >> 10) == 0;
}

int main() {
    canvas_init(240, 150);
    W = canvas_width();
    H = canvas_height();
    build_sin();
    build_map();

    px = 4 * 1024 + 512;
    py = 6 * 1024 + 512;
    ang = 0;

    print_line("Raycaster -- a first-person maze on your own OS.");
    print_line("arrows move/turn, esc quits");

    render();
    for (;;) {
        int k = get_key();
        if (k == 27) break;
        int dx = icos(ang) >> 3;
        int dy = isin(ang) >> 3;
        if (k == 200) {
            if (can_walk(px + dx * 2, py)) px = px + dx;
            if (can_walk(px, py + dy * 2)) py = py + dy;
        } else if (k == 201) {
            if (can_walk(px - dx * 2, py)) px = px - dx;
            if (can_walk(px, py - dy * 2)) py = py - dy;
        } else if (k == 202) ang = (ang - 6) & 255;
        else if (k == 203) ang = (ang + 6) & 255;
        else continue;
        render();
    }
    print_line("done");
    return 0;
}
