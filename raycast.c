int MAPW;
int MAPH;
int map[384];
int W;
int H;

int px;
int py;
int ang;
int health;
int ammo;
int keys_found;
int total_keys;

int sintab[256];

int plot_col_r;
int plot_col_g;
int plot_col_b;

/* sprites: pickups and pillars drawn as billboards */
int spx[24];
int spy[24];
int spkind[24];
int spalive[24];
int spcount;

int zbuf[400];

void build_sin() {
    for (int i = 0; i < 128; i++) {
        int v = (4 * i * (128 - i) * 1024) / 16384;
        sintab[i] = v;
        sintab[i + 128] = -v;
    }
}

int isin(int a) { return sintab[a & 255]; }
int icos(int a) { return sintab[(a + 64) & 255]; }

void put(int x, int y, int v) { if (x >= 0 && y >= 0 && x < MAPW && y < MAPH) map[y * MAPW + x] = v; }

int cell_at(int gx, int gy) {
    if (gx < 0 || gy < 0 || gx >= MAPW || gy >= MAPH) return 1;
    return map[gy * MAPW + gx];
}

void add_sprite(int cx, int cy, int kind) {
    if (spcount >= 24) return;
    spx[spcount] = cx * 1024 + 512;
    spy[spcount] = cy * 1024 + 512;
    spkind[spcount] = kind;
    spalive[spcount] = 1;
    spcount = spcount + 1;
}

void build_map() {
    MAPW = 24;
    MAPH = 16;
    for (int y = 0; y < MAPH; y++)
        for (int x = 0; x < MAPW; x++)
            put(x, y, (x == 0 || y == 0 || x == MAPW - 1 || y == MAPH - 1) ? 1 : 0);

    /* rooms and corridors, wall types 1..4 pick different textures */
    for (int x = 4; x < 12; x++) { put(x, 4, 2); }
    for (int y = 4; y < 9; y++)  { put(4, y, 2); }
    put(8, 4, 0); put(4, 6, 0);

    for (int x = 14; x < 21; x++) { put(x, 3, 3); put(x, 10, 3); }
    for (int y = 3; y < 11; y++)  { put(14, y, 3); put(20, y, 3); }
    put(17, 3, 0); put(14, 7, 0);

    for (int x = 6; x < 11; x++) put(x, 12, 4);
    for (int y = 12; y < 15; y++) put(6, y, 4);
    put(9, 12, 0);

    put(12, 6, 2); put(12, 7, 2);
    put(2, 11, 3); put(3, 11, 3);

    spcount = 0;
    add_sprite(9, 6, 0);
    add_sprite(17, 6, 1);
    add_sprite(8, 13, 1);
    add_sprite(21, 13, 0);
    add_sprite(2, 2, 2);
    add_sprite(19, 2, 2);
    add_sprite(11, 9, 2);
    total_keys = 2;
}

/* Wall texture: procedural brick / panel patterns, picked by wall type.
   tx is the horizontal position across the wall (0..63), ty the vertical. */
void wall_texel(int kind, int tx, int ty, int side, int shade) {
    int r = 170; int g = 170; int b = 180;

    if (kind == 1) {
        /* grey brick with mortar lines */
        int row = (ty / 8) & 1;
        int bx = (tx + row * 16) % 32;
        int mortar = (ty % 8 == 0) || (bx == 0);
        if (mortar) { r = 90; g = 90; b = 100; }
        else { r = 150; g = 140; b = 135; }
    } else if (kind == 2) {
        /* red brick */
        int row = (ty / 6) & 1;
        int bx = (tx + row * 12) % 24;
        int mortar = (ty % 6 == 0) || (bx == 0);
        if (mortar) { r = 110; g = 80; b = 70; }
        else { r = 190; g = 85; b = 65; }
    } else if (kind == 3) {
        /* green metal panels with rivets */
        int panel = ((tx / 16) + (ty / 16)) & 1;
        r = panel ? 70 : 90;
        g = panel ? 160 : 185;
        b = panel ? 100 : 120;
        int rx = tx % 16;
        int ry = ty % 16;
        if ((rx == 3 || rx == 12) && (ry == 3 || ry == 12)) { r = 210; g = 230; b = 210; }
    } else {
        /* blue stone with vertical seams */
        int seam = (tx % 21 == 0);
        r = seam ? 60 : 95;
        g = seam ? 80 : 120;
        b = seam ? 150 : 210;
        if ((ty % 13) == 0) { r = r - 20; g = g - 20; b = b - 20; }
    }

    if (side) { r = (r * 65) / 100; g = (g * 65) / 100; b = (b * 65) / 100; }
    r = (r * shade) / 256; g = (g * shade) / 256; b = (b * shade) / 256;
    if (r < 0) r = 0; if (g < 0) g = 0; if (b < 0) b = 0;
    plot_col_r = r; plot_col_g = g; plot_col_b = b;
}

void draw_minimap() {
    int cell = 3;
    int ox = W - MAPW * cell - 3;
    int oy = 3;
    canvas_rect(ox - 1, oy - 1, MAPW * cell + 2, MAPH * cell + 2, 20, 22, 30);
    for (int y = 0; y < MAPH; y++) {
        for (int x = 0; x < MAPW; x++) {
            int c = cell_at(x, y);
            if (!c) continue;
            int r = 110; int g = 116; int b = 130;
            if (c == 2) { r = 180; g = 90; b = 70; }
            if (c == 3) { r = 90; g = 175; b = 110; }
            if (c == 4) { r = 90; g = 115; b = 200; }
            canvas_rect(ox + x * cell, oy + y * cell, cell - 1, cell - 1, r, g, b);
        }
    }
    for (int i = 0; i < spcount; i++) {
        if (!spalive[i]) continue;
        if (spkind[i] == 2) continue;
        int mx = ox + (spx[i] >> 10) * cell;
        int my = oy + (spy[i] >> 10) * cell;
        canvas_rect(mx, my, cell - 1, cell - 1, 240, 210, 80);
    }
    canvas_rect(ox + (px >> 10) * cell, oy + (py >> 10) * cell, cell - 1, cell - 1, 90, 220, 255);
}

void draw_hud() {
    int bary = H - 9;
    canvas_rect(0, bary - 1, W, 10, 18, 20, 28);
    int hw = (health * (W / 3)) / 100;
    if (hw < 0) hw = 0;
    canvas_rect(3, bary + 1, W / 3, 6, 50, 30, 30);
    canvas_rect(3, bary + 1, hw, 6, 210, 70, 70);
    int kx = W / 2;
    for (int i = 0; i < total_keys; i++) {
        int lit = (i < keys_found);
        canvas_rect(kx + i * 9, bary + 1, 6, 6,
                    lit ? 250 : 60, lit ? 215 : 55, lit ? 80 : 40);
    }
    int aw = (ammo * (W / 4)) / 50;
    if (aw < 0) aw = 0;
    if (aw > W / 4) aw = W / 4;
    canvas_rect(W - W / 4 - 3, bary + 1, W / 4, 6, 30, 40, 55);
    canvas_rect(W - W / 4 - 3, bary + 1, aw, 6, 90, 170, 240);
}

void render() {
    int half = H / 2;

    /* gradient sky and floor instead of two flat blocks */
    for (int y = 0; y < half; y++) {
        int t = (y * 255) / (half > 0 ? half : 1);
        canvas_rect(0, y, W, 1, 30 + t / 5, 38 + t / 4, 70 + t / 3);
    }
    for (int y = half; y < H; y++) {
        int t = ((y - half) * 255) / (half > 0 ? half : 1);
        canvas_rect(0, y, W, 1, 74 - t / 6, 68 - t / 6, 62 - t / 7);
    }

    for (int col = 0; col < W; col++) {
        int off = ((col - W / 2) * 48) / W;
        int ra = (ang + off) & 255;
        int dx = icos(ra);
        int dy = isin(ra);

        int rx = px;
        int ry = py;
        int hit = 0;
        int side = 0;
        int steps = 0;
        int prev_gx = rx >> 10;

        while (!hit && steps < 900) {
            rx = rx + (dx >> 5);
            ry = ry + (dy >> 5);
            steps = steps + 1;
            int gx = rx >> 10;
            int gy = ry >> 10;
            int c = cell_at(gx, gy);
            if (c) {
                hit = c;
                if (gx != prev_gx) side = 0; else side = 1;
            }
            prev_gx = gx;
        }

        int dist = steps * 32;
        int corr = icos(off & 255);
        if (corr < 64) corr = 64;
        dist = (dist * corr) >> 10;
        if (dist < 48) dist = 48;
        if (col < 400) zbuf[col] = dist;

        int lineh = (H * 2048) / dist;
        int y0 = half - lineh / 2;
        int y1 = y0 + lineh;

        int shade = 300 - (dist / 48);
        if (shade > 256) shade = 256;
        if (shade < 55) shade = 55;

        /* where along the wall this ray landed, for the texture lookup */
        int tx;
        if (side) tx = (rx >> 4) & 63;
        else tx = (ry >> 4) & 63;

        int ys = y0; if (ys < 0) ys = 0;
        int ye = y1; if (ye > H - 10) ye = H - 10;

        for (int y = ys; y < ye; y++) {
            int ty = ((y - y0) * 64) / (lineh > 0 ? lineh : 1);
            wall_texel(hit, tx, ty & 63, side, shade);
            plot(col, y, plot_col_r, plot_col_g, plot_col_b);
        }
    }

    /* billboard sprites, depth-tested against the wall distances */
    for (int i = 0; i < spcount; i++) {
        if (!spalive[i]) continue;
        int rx = spx[i] - px;
        int ry = spy[i] - py;

        int ca = icos(-ang & 255);
        int sa = isin(-ang & 255);
        int tx2 = (rx * ca - ry * sa) >> 10;
        int ty2 = (rx * sa + ry * ca) >> 10;
        if (ty2 < 96) continue;

        int scol = W / 2 + (tx2 * W) / (ty2 * 48 / 32);
        int size = (H * 700) / ty2;
        if (size < 2) continue;
        if (size > H) size = H;

        int sy0 = half + (H * 260) / ty2 - size;
        int r = 240; int g = 200; int b = 70;
        if (spkind[i] == 1) { r = 90; g = 220; b = 130; }
        if (spkind[i] == 2) { r = 200; g = 200; b = 210; }

        for (int x = scol - size / 2; x < scol + size / 2; x++) {
            if (x < 0 || x >= W) continue;
            if (x < 400 && ty2 > zbuf[x]) continue;
            for (int y = sy0; y < sy0 + size; y++) {
                if (y < 0 || y >= H - 10) continue;
                int lx = x - (scol - size / 2);
                int ly = y - sy0;
                int hs = size / 2;
                if (hs < 1) hs = 1;
                int ddx = lx - hs;
                int ddy = ly - hs;
                if (spkind[i] == 2) {
                    if (lx < size / 4 || lx > (size * 3) / 4) continue;
                } else {
                    if (ddx * ddx + ddy * ddy > hs * hs) continue;
                }
                int sh = 256 - (ty2 / 24);
                if (sh < 70) sh = 70;
                if (sh > 256) sh = 256;
                plot(x, y, (r * sh) / 256, (g * sh) / 256, (b * sh) / 256);
            }
        }
    }

    draw_minimap();
    draw_hud();
    canvas_present();
}

int can_walk(int nx, int ny) { return cell_at(nx >> 10, ny >> 10) == 0; }

void pickup_check() {
    for (int i = 0; i < spcount; i++) {
        if (!spalive[i]) continue;
        if (spkind[i] == 2) continue;
        int dx = spx[i] - px; if (dx < 0) dx = -dx;
        int dy = spy[i] - py; if (dy < 0) dy = -dy;
        if (dx < 500 && dy < 500) {
            spalive[i] = 0;
            if (spkind[i] == 0) {
                keys_found = keys_found + 1;
                beep(880, 40); beep(1180, 70);
                print("Key found: ");
                print_int(keys_found);
                print("/");
                print_int(total_keys);
                put_char(10);
                if (keys_found >= total_keys) print_line("All keys collected. Find the exit at the top-left.");
            } else {
                ammo = ammo + 10;
                if (ammo > 50) ammo = 50;
                beep(700, 30);
            }
        }
    }
}

int main() {
    canvas_init(300, 190);
    W = canvas_width();
    H = canvas_height();
    build_sin();
    build_map();

    px = 6 * 1024 + 512;
    py = 8 * 1024 + 512;
    ang = 0;
    health = 100;
    ammo = 20;
    keys_found = 0;

    print_line("RAYCAST -- textured maze crawler");
    print_line("arrows or WASD to move and turn, Q/E to strafe, esc quits");
    print_line("Gold spheres are keys, green ones are ammo.");

    render();
    for (;;) {
        int k = get_key();
        if (k == 27) break;
        if (k == 119 || k == 87) k = 200;
        else if (k == 115 || k == 83) k = 201;
        else if (k == 97 || k == 65) k = 202;
        else if (k == 100 || k == 68) k = 203;

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
        else if (k == 113 || k == 81) {
            int sx2 = icos((ang - 64) & 255) >> 3;
            int sy2 = isin((ang - 64) & 255) >> 3;
            if (can_walk(px + sx2 * 2, py)) px = px + sx2;
            if (can_walk(px, py + sy2 * 2)) py = py + sy2;
        } else if (k == 101 || k == 69) {
            int sx2 = icos((ang + 64) & 255) >> 3;
            int sy2 = isin((ang + 64) & 255) >> 3;
            if (can_walk(px + sx2 * 2, py)) px = px + sx2;
            if (can_walk(px, py + sy2 * 2)) py = py + sy2;
        } else continue;

        pickup_check();
        render();
    }
    sound_off();
    print_line("done");
    return 0;
}
