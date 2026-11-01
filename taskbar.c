/* Taskbar Studio for ZapczOS -- live taskbar editor (port of the Windows app)
   mouse: click or drag the sliders, click < > on the choices, click presets, slots and Save
   keys: up/down pick a setting, left/right change it ([ and ] move by 10)
   1-6 presets, O original, D default, F1-F4 load slot, F5-F8 save slot, Enter save
   changes are saved as you make them */

int rid[15] = {0,1,3,4,2,5,14,6,7,8,9,10,11,12,13};
int lo[15] = {36,15,0,50,4,0,0,0,0,0,0,0,0,0,0};
int hi[15] = {84,100,1,110,36,100,1,255,255,255,1,255,255,255,2};
/* presets, 15 values each in taskbar id order:
   height opacity top icon gap align bgon r g b accon r g b indicator */
int PRE[90] = {
 52,35,0,100,14,0,1,255,255,255,0,76,194,255,2,
 52,100,0,100,14,0,1,22,22,26,1,76,194,255,1,
 56,70,0,100,16,0,1,10,6,25,1,255,0,200,1,
 60,55,0,100,22,50,1,30,30,40,1,120,200,255,2,
 52,85,0,100,14,0,1,245,245,247,1,0,100,220,1,
 40,100,0,90,10,0,0,245,246,249,0,76,194,255,0
};
int DEF[15] = {52,100,0,100,14,0,0,245,246,249,0,76,194,255,1};
int v[15];
int org[15];
int slot[60];
int used[4];
char msg[24];
char nb[12];
char vb[20];
int sel = 0;
int dirty = 0;


int APPS[4] = {2, 8, 4, 10};

char *numstr(int n) {
    char t[12];
    int i = 0;
    int j;
    if (n == 0) { nb[0] = '0'; nb[1] = 0; return nb; }
    while (n > 0) { t[i] = '0' + n % 10; n = n / 10; i++; }
    for (j = 0; j < i; j++) nb[j] = t[i - 1 - j];
    nb[i] = 0;
    return nb;
}

char *unit(int id) {
    if (id == 0 || id == 4) return " px";
    if (id == 1 || id == 3 || id == 5) return "%";
    return "";
}

int clampv(int id, int x) {
    if (x < lo[id]) x = lo[id];
    if (x > hi[id]) x = hi[id];
    return x;
}

char *label(int r) {
    if (r == 0) return "Height";
    if (r == 1) return "Opacity";
    if (r == 2) return "Icon size";
    if (r == 3) return "Spacing";
    if (r == 4) return "Position";
    if (r == 5) return "Icon position";
    if (r == 6) return "Indicator";
    if (r == 7) return "Background";
    if (r == 8) return "   Red";
    if (r == 9) return "   Green";
    if (r == 10) return "   Blue";
    if (r == 11) return "Accent";
    if (r == 12) return "   Red";
    if (r == 13) return "   Green";
    return "   Blue";
}

char *enumtxt(int id, int x) {
    if (id == 2) { if (x) return "Top"; return "Bottom"; }
    if (id == 14) { if (x == 0) return "None"; if (x == 1) return "Bar"; return "Dot"; }
    if (x) return "Custom";
    return "Default";
}

void sync_in() {
    int i;
    for (i = 0; i < 15; i++) v[i] = tb_get(i);
}

void apply() {
    int i;
    for (i = 0; i < 15; i++) tb_set(i, v[i]);
}

void setmsg(char *s) { strcpy(msg, s); }

/* the original and the four slots live in the OS's own settings (tb_get/tb_set ids 100+),
   so this app never creates a file of its own */
void fsave() {
    int i;
    int k;
    int m = 128;
    for (k = 0; k < 4; k++) { if (used[k]) m = m + (1 << k); }
    tb_set(100, m);
    for (i = 0; i < 15; i++) tb_set(101 + i, org[i]);
    for (i = 0; i < 60; i++) tb_set(116 + i, slot[i]);
    tb_save();
}

int fload() {
    int i;
    int k;
    int m = tb_get(100);
    if (!(m & 128)) return 0;
    for (k = 0; k < 4; k++) used[k] = (m >> k) & 1;
    for (i = 0; i < 15; i++) org[i] = tb_get(101 + i);
    for (i = 0; i < 60; i++) slot[i] = tb_get(116 + i);
    return 1;
}

void adjust(int r, int d) {
    int id = rid[r];
    v[id] = clampv(id, v[id] + d);
    if (id >= 7 && id <= 9) { v[6] = 1; }
    if (id >= 11 && id <= 13) { v[10] = 1; }
    apply();
}

void setval(int id, int nv) {
    nv = clampv(id, nv);
    if (nv == v[id]) return;
    v[id] = nv;
    if (id >= 7 && id <= 9) { v[6] = 1; }
    if (id >= 11 && id <= 13) { v[10] = 1; }
    apply();
    dirty = 1;
}

/* choices (position, indicator, colour modes) step and wrap around */
void bump(int r, int d) {
    int id = rid[r];
    int nv = v[id] + d;
    if (nv > hi[id]) nv = lo[id];
    if (nv < lo[id]) nv = hi[id];
    v[id] = nv;
    apply();
    dirty = 1;
}

int is_enum(int id) {
    if (id == 2 || id == 6 || id == 14 || id == 10) return 1;
    return 0;
}

int hit_row(int mx, int my) {
    if (mx < 18 || mx > 476 || my < 56 || my >= 56 + 15 * 21) return -1;
    return (my - 56) / 21;
}

void use_preset(int p) {
    int i;
    for (i = 0; i < 15; i++) v[i] = PRE[p * 15 + i];
    apply();
}

void T(char *s, int x, int y, int r, int g, int b) { canvas_text(x, y, s, r, g, b, 1); }

void panel(int x, int y, int w, int h) { canvas_rrect(x, y, w, h, 10, 24, 27, 38); }

void slider(int x, int y, int w, int val, int lo2, int hi2, int c) {
    int r = (c >> 16) & 255;
    int g = (c >> 8) & 255;
    int b = c & 255;
    int f = (val - lo2) * w / (hi2 - lo2);
    canvas_rrect(x, y + 5, w, 6, 3, 46, 52, 70);
    if (f > 0) canvas_rrect(x, y + 5, f + 3, 6, 3, r, g, b);
    canvas_rrect(x + f - 3, y + 1, 14, 14, 7, 235, 238, 246);
}

void preview() {
    int i;
    int bh = v[0] * 73 / 100;
    int px = 16;
    int py = 394;
    int pw = 748;
    int ph = 114;
    int by = py + ph - bh;
    int op = v[1] * 255 / 100;
    int br = 245;
    int bg = 246;
    int bb = 249;
    int tc = 28;
    int ar = 76;
    int ag = 194;
    int ab = 255;
    int sz;
    int tile;
    int gap;
    int bx;
    int total;
    int room;
    int ty;
    int rx;
    if (v[2]) by = py;
    if (v[6]) {
        br = v[7]; bg = v[8]; bb = v[9];
        if ((br * 30 + bg * 59 + bb * 11) / 100 < 141) tc = 240;
    }
    if (v[10]) { ar = v[11]; ag = v[12]; ab = v[13]; }
    /* the real wallpaper */
    canvas_wallpaper(px, py, pw, ph, v[2]);
    /* taskbar */
    canvas_rect_a(px, by, pw, bh, br, bg, bb, op);
    sz = bh * 72 / 100;
    ty = by + (bh - sz) / 2;
    canvas_logo(px + 10, ty, sz, 154, 142, 242);
    tile = sz * v[3] / 100;
    if (tile < 6) tile = 6;
    gap = v[4] * 73 / 100;
    bx = px + 10 + sz + 14;
    total = 4 * tile + 3 * gap;
    room = pw - 96 - (bx - px) - total;
    if (room > 0) bx = bx + room * v[5] / 100;
    ty = by + (bh - tile) / 2;
    for (i = 0; i < 4; i++) {
        canvas_icon(APPS[i], bx, ty, tile);
        if (v[14] == 1) {
            if (i == 0) canvas_rrect(bx + tile / 6, by + bh - 4, tile * 2 / 3, 3, 1, ar, ag, ab);
            else canvas_rrect(bx + tile / 2 - 3, by + bh - 4, 6, 3, 1, 150, 150, 165);
        }
        if (v[14] == 2) {
            if (i == 0) canvas_rrect(bx + tile / 2 - 3, by + bh - 6, 6, 6, 3, ar, ag, ab);
            else canvas_rrect(bx + tile / 2 - 2, by + bh - 5, 4, 4, 2, 150, 150, 165);
        }
        if (i == 0) {
            canvas_rect(bx - 2, ty - 2, tile + 4, 2, ar, ag, ab);
        }
        bx = bx + tile + gap;
    }
    rx = px + pw - 12 - canvas_text_w("12:34", 1);
    canvas_text(rx, by + (bh - 16) / 2, "12:34", tc, tc, tc, 1);
}

void draw() {
    int k;
    int y;
    int id;
    int val;
    int r;
    canvas_vgrad(0, 0, 780, 520, (15 << 16) + (17 << 8) + 25, (11 << 16) + (13 << 8) + 19);
    canvas_logo(18, 9, 28, 154, 142, 242);
    canvas_text(54, 14, "Taskbar Studio", 235, 238, 246, 1);
    canvas_text(54 + canvas_text_w("Taskbar Studio", 1) + 12, 14, "live editor for your ZapczOS taskbar", 110, 118, 140, 1);
    canvas_text(764 - canvas_text_w(msg, 1), 14, msg, 120, 220, 140, 1);
    panel(12, 50, 470, 332);
    panel(494, 50, 274, 332);
    for (k = 0; k < 15; k++) {
        y = 58 + k * 21;
        id = rid[k];
        val = v[id];
        if (k == sel) {
            canvas_rrect(18, y - 2, 458, 20, 6, 38, 70, 104);
            canvas_rrect(18, y + 1, 3, 14, 1, 76, 194, 255);
        }
        T(label(k), 30, y, 205, 211, 226);
        if (id == 2 || id == 6 || id == 14 || id == 10) {
            T("<", 150, y, 110, 118, 140);
            T(enumtxt(id, val), 168, y, 255, 214, 120);
            T(">", 150 + 8 * 11, y, 110, 118, 140);
            if (id == 6) {
                if (v[6]) canvas_rrect(392, y + 1, 52, 14, 5, v[7], v[8], v[9]);
                else canvas_rrect(392, y + 1, 52, 14, 5, 245, 246, 249);
            }
            if (id == 10) {
                if (v[10]) canvas_rrect(392, y + 1, 52, 14, 5, v[11], v[12], v[13]);
                else canvas_rrect(392, y + 1, 52, 14, 5, 76, 194, 255);
            }
        } else {
            int cr = 76;
            int cg = 194;
            int cb = 255;
            if (id == 7 || id == 11) { cr = 255; cg = 90; cb = 90; }
            if (id == 8 || id == 12) { cr = 90; cg = 220; cb = 110; }
            if (id == 9 || id == 13) { cr = 100; cg = 140; cb = 255; }
            slider(150, y, 200, val, lo[id], hi[id], (cr << 16) + (cg << 8) + cb);
            strcpy(vb, numstr(val));
            strcat(vb, unit(id));
            T(vb, 372, y, 205, 211, 226);
        }
    }
    T("Presets", 512, 60, 76, 194, 255);
    T("1  Glass", 512, 84, 200, 206, 220);
    T("2  Dark", 512, 104, 200, 206, 220);
    T("3  Neon", 512, 124, 200, 206, 220);
    T("4  Dock", 512, 144, 200, 206, 220);
    T("5  Light", 512, 164, 200, 206, 220);
    T("6  Slim", 512, 184, 200, 206, 220);
    T("O  Original", 512, 212, 255, 214, 120);
    T("D  Default", 512, 232, 255, 214, 120);
    T("Slots", 512, 262, 76, 194, 255);
    T("Load", 512, 284, 200, 206, 220);
    T("Save", 512, 308, 200, 206, 220);
    for (k = 0; k < 4; k++) {
        if (used[k]) canvas_rrect(590 + k * 34, 282, 28, 20, 6, 70, 150, 92);
        else canvas_rrect(590 + k * 34, 282, 28, 20, 6, 50, 56, 74);
        canvas_rrect(590 + k * 34, 306, 28, 20, 6, 50, 56, 74);
        T(numstr(k + 1), 600 + k * 34, 284, 235, 238, 246);
        T(numstr(k + 1), 600 + k * 34, 308, 235, 238, 246);
    }
    canvas_rrect(512, 336, 110, 26, 7, 38, 70, 104);
    T("Save now", 528, 341, 235, 238, 246);
    T("Esc  close", 640, 341, 110, 118, 140);
    T("Preview", 16, 376, 76, 194, 255);
    preview();
}

void do_original() {
    int i;
    for (i = 0; i < 15; i++) v[i] = org[i];
    apply();
    setmsg("Original restored");
    dirty = 1;
}

void do_default() {
    int i;
    for (i = 0; i < 15; i++) v[i] = DEF[i];
    apply();
    setmsg("Defaults");
    dirty = 1;
}

void do_load(int s) {
    int i;
    if (s < 0 || s > 3) return;
    if (used[s]) {
        for (i = 0; i < 15; i++) v[i] = slot[s * 15 + i];
        apply();
        setmsg("Slot loaded");
        dirty = 1;
    } else setmsg("Slot empty");
}

void do_save(int s) {
    int i;
    if (s < 0 || s > 3) return;
    for (i = 0; i < 15; i++) slot[s * 15 + i] = v[i];
    used[s] = 1;
    fsave();
    setmsg("Slot saved");
}

int main() {
    int k;
    int i;
    int mx;
    int my;
    int mb;
    int pmb = 0;
    int drag = -1;
    int ch;
    int r;
    int id;
    canvas_init(780, 520);
    sync_in();
    if (!fload()) {
        for (i = 0; i < 15; i++) org[i] = v[i];
        fsave();
    }
    setmsg("Live");
    draw();
    canvas_present();
    while (1) {
        ch = 0;
        k = key_down();
        if (k) {
            ch = 1;
            if (k == 27) break;
            if (k == 200 && sel > 0) sel--;
            if (k == 201 && sel < 14) sel++;
            if (k == 202) { if (is_enum(rid[sel])) bump(sel, -1); else adjust(sel, -1); dirty = 1; }
            if (k == 203) { if (is_enum(rid[sel])) bump(sel, 1); else adjust(sel, 1); dirty = 1; }
            if (k == '[') { adjust(sel, -10); dirty = 1; }
            if (k == ']') { adjust(sel, 10); dirty = 1; }
            if (k >= '1' && k <= '6') { use_preset(k - '1'); setmsg("Preset applied"); dirty = 1; }
            if (k == 'o' || k == 'O') { do_original(); }
            if (k == 'd' || k == 'D') { do_default(); }
            if (k >= 205 && k <= 208) { do_load(k - 205); }
            if (k >= 209 && k <= 212) { do_save(k - 209); }
            if (k == 10) { fsave(); dirty = 0; setmsg("Saved"); }
        }
        mx = mouse_x();
        my = mouse_y();
        mb = mouse_btn();
        if (mx >= 0 && my >= 0 && drag < 0) {
            r = hit_row(mx, my);
            if (r >= 0 && r != sel) { sel = r; ch = 1; }
        }
        if (mb && !pmb && mx >= 0 && my >= 0) {
            ch = 1;
            r = hit_row(mx, my);
            if (r >= 0) {
                sel = r;
                id = rid[r];
                if (is_enum(id)) {
                    if (mx >= 142 && mx < 170) bump(r, -1);
                    else if (mx >= 170 && mx < 270) bump(r, 1);
                } else if (mx >= 140 && mx <= 362) {
                    drag = r;
                }
            }
            if (mx >= 508 && mx < 740) {
                if (my >= 82 && my < 202) { use_preset((my - 82) / 20); setmsg("Preset applied"); dirty = 1; }
                if (my >= 208 && my < 228) { do_original(); }
                if (my >= 228 && my < 248) { do_default(); }
            }
            if (mx >= 590 && mx < 726) {
                if (my >= 280 && my < 304) { do_load((mx - 590) / 34); }
                if (my >= 304 && my < 328) { do_save((mx - 590) / 34); }
            }
            if (mx >= 512 && mx < 622 && my >= 336 && my < 362) { fsave(); dirty = 0; setmsg("Saved"); }
        }
        if (mb && drag >= 0 && mx >= 0) {
            id = rid[drag];
            setval(id, lo[id] + (mx - 150) * (hi[id] - lo[id]) / 200);
            ch = 1;
        }
        if (!mb) drag = -1;
        pmb = mb;
        if (dirty && !mb) { fsave(); dirty = 0; }
        if (ch) draw();
        canvas_present();
        sleep(10);
    }
    fsave();
    return 0;
}
