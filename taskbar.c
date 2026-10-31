/* Taskbar Studio for ZapczOS -- live taskbar editor (port of the Windows app)
   up/down pick a setting, left/right change it ([ and ] move by 10)
   1-6 presets, O original, D default, F1-F4 load slot, F5-F8 save slot
   enter saves to disk, esc saves and quits */

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
char fb[96];
char msg[24];
char nb[12];
char vb[20];
int sel = 0;


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

void fsave() {
    int i;
    int k;
    int m = 0;
    for (k = 0; k < 4; k++) { if (used[k]) m = m + (1 << k); }
    fb[0] = 'T';
    fb[1] = 'S';
    fb[2] = m;
    for (i = 0; i < 15; i++) fb[3 + i] = org[i];
    for (i = 0; i < 60; i++) fb[18 + i] = slot[i];
    file_write("TBSTUDIO.DAT", fb, 78);
}

int fload() {
    int n = file_read("TBSTUDIO.DAT", fb, 96);
    int i;
    int k;
    if (n < 78) return 0;
    if (fb[0] != 'T' || fb[1] != 'S') return 0;
    for (k = 0; k < 4; k++) used[k] = (fb[2] >> k) & 1;
    for (i = 0; i < 15; i++) org[i] = fb[3 + i] & 255;
    for (i = 0; i < 60; i++) slot[i] = fb[18 + i] & 255;
    return 1;
}

void adjust(int r, int d) {
    int id = rid[r];
    v[id] = clampv(id, v[id] + d);
    if (id >= 7 && id <= 9) { v[6] = 1; }
    if (id >= 11 && id <= 13) { v[10] = 1; }
    apply();
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
    T("F1-F4 load", 512, 284, 200, 206, 220);
    T("F5-F8 save", 512, 304, 200, 206, 220);
    for (k = 0; k < 4; k++) {
        if (used[k]) canvas_rrect(654 + k * 26, 286, 20, 12, 5, 120, 220, 140);
        else canvas_rrect(654 + k * 26, 286, 20, 12, 5, 50, 56, 74);
    }
    T("Enter  save", 512, 340, 110, 118, 140);
    T("Esc  save and quit", 512, 358, 110, 118, 140);
    T("Preview", 16, 376, 76, 194, 255);
    preview();
}

int main() {
    int k;
    int i;
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
        k = get_key();
        if (k == 27) break;
        if (k == 200 && sel > 0) sel--;
        if (k == 201 && sel < 14) sel++;
        if (k == 202) adjust(sel, -1);
        if (k == 203) adjust(sel, 1);
        if (k == '[') adjust(sel, -10);
        if (k == ']') adjust(sel, 10);
        if (k >= '1' && k <= '6') { use_preset(k - '1'); setmsg("Preset applied"); }
        if (k == 'o' || k == 'O') {
            for (i = 0; i < 15; i++) v[i] = org[i];
            apply();
            setmsg("Original restored");
        }
        if (k == 'd' || k == 'D') {
            for (i = 0; i < 15; i++) v[i] = DEF[i];
            apply();
            setmsg("Defaults");
        }
        if (k >= 205 && k <= 208) {
            int s = k - 205;
            if (used[s]) {
                for (i = 0; i < 15; i++) v[i] = slot[s * 15 + i];
                apply();
                setmsg("Slot loaded");
            } else setmsg("Slot empty");
        }
        if (k >= 209 && k <= 212) {
            int s = k - 209;
            for (i = 0; i < 15; i++) slot[s * 15 + i] = v[i];
            used[s] = 1;
            fsave();
            setmsg("Slot saved");
        }
        if (k == 10) {
            tb_save();
            fsave();
            setmsg("Saved");
        }
        draw();
        canvas_present();
    }
    tb_save();
    return 0;
}
