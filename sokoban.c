int W;
int H;
int MW;
int MH;
int grid[240];
int px;
int py;
int moves;
int level;
char lvlbuf[1024];

int TILE_FLOOR;
int TILE_WALL;
int TILE_GOAL;
int TILE_BOX;
int TILE_BOXG;

char builtin1[240];
char builtin2[240];

void set_defaults() {
    TILE_FLOOR = 0; TILE_WALL = 1; TILE_GOAL = 2; TILE_BOX = 3; TILE_BOXG = 4;
}

int at(int x, int y) {
    if (x < 0 || y < 0 || x >= MW || y >= MH) return TILE_WALL;
    return grid[y * MW + x];
}

void put(int x, int y, int v) {
    if (x < 0 || y < 0 || x >= MW || y >= MH) return;
    grid[y * MW + x] = v;
}

int parse_level(char* s) {
    MW = 0; MH = 0;
    int x = 0; int y = 0; int i = 0;
    while (s[i] != 0) {
        char c = s[i];
        if (c == 10) { if (x > MW) MW = x; x = 0; y = y + 1; i = i + 1; continue; }
        x = x + 1;
        i = i + 1;
    }
    if (x > MW) MW = x;
    if (x > 0) y = y + 1;
    MH = y;
    if (MW > 20) MW = 20;
    if (MH > 12) MH = 12;

    for (int k = 0; k < MW * MH; k++) grid[k] = TILE_FLOOR;

    x = 0; y = 0; i = 0;
    while (s[i] != 0 && y < MH) {
        char c = s[i];
        if (c == 10) { x = 0; y = y + 1; i = i + 1; continue; }
        if (x < MW) {
            if (c == 35) put(x, y, TILE_WALL);
            else if (c == 46) put(x, y, TILE_GOAL);
            else if (c == 36) put(x, y, TILE_BOX);
            else if (c == 42) put(x, y, TILE_BOXG);
            else if (c == 64) { put(x, y, TILE_FLOOR); px = x; py = y; }
            else if (c == 43) { put(x, y, TILE_GOAL); px = x; py = y; }
            else put(x, y, TILE_FLOOR);
        }
        x = x + 1;
        i = i + 1;
    }
    return 1;
}

int won() {
    for (int i = 0; i < MW * MH; i++) if (grid[i] == TILE_BOX) return 0;
    return 1;
}

void draw() {
    int cell = 14;
    canvas_clear(20, 22, 30);
    for (int y = 0; y < MH; y++) {
        for (int x = 0; x < MW; x++) {
            int t = at(x, y);
            int cx = x * cell;
            int cy = y * cell + 8;
            if (t == TILE_WALL) canvas_rect(cx, cy, cell - 1, cell - 1, 110, 116, 132);
            else if (t == TILE_GOAL) canvas_rect(cx + 4, cy + 4, cell - 9, cell - 9, 230, 190, 70);
            else if (t == TILE_BOX) canvas_rect(cx + 1, cy + 1, cell - 3, cell - 3, 190, 120, 60);
            else if (t == TILE_BOXG) canvas_rect(cx + 1, cy + 1, cell - 3, cell - 3, 110, 210, 110);
        }
    }
    canvas_rect(px * cell + 3, py * cell + 11, cell - 7, cell - 7, 90, 170, 250);
    canvas_present();
}

void try_move(int dx, int dy) {
    int nx = px + dx;
    int ny = py + dy;
    int t = at(nx, ny);
    if (t == TILE_WALL) return;
    if (t == TILE_BOX || t == TILE_BOXG) {
        int bx = nx + dx;
        int by = ny + dy;
        int bt = at(bx, by);
        if (bt == TILE_WALL || bt == TILE_BOX || bt == TILE_BOXG) return;
        if (bt == TILE_GOAL) put(bx, by, TILE_BOXG); else put(bx, by, TILE_BOX);
        if (t == TILE_BOXG) put(nx, ny, TILE_GOAL); else put(nx, ny, TILE_FLOOR);
        beep(700, 8);
    }
    px = nx; py = ny;
    moves = moves + 1;
}

void load_level(int n) {
    if (n == 1) parse_level(builtin1);
    else parse_level(builtin2);
    moves = 0;
}

void make_builtins() {
    strcpy(builtin1, "#######\n#     #\n# .$@ #\n#     #\n#######\n");
    strcpy(builtin2, "########\n#  .   #\n# $$@  #\n#  .   #\n#      #\n########\n");
}

int main() {
    canvas_init(240, 150);
    W = canvas_width();
    H = canvas_height();
    set_defaults();
    make_builtins();

    print_line("Sokoban -- push the crates onto the gold squares.");
    print_line("arrows move, r restarts, esc quits");
    print_line("Tip: a file named SOKO.TXT on disk is loaded as a custom level.");

    if (file_exists("SOKO.TXT")) {
        int n = file_read("SOKO.TXT", lvlbuf, 1024);
        if (n > 0) { parse_level(lvlbuf); moves = 0; level = 0; print_line("Loaded SOKO.TXT"); }
        else { level = 1; load_level(1); }
    } else {
        level = 1;
        load_level(1);
    }

    draw();
    for (;;) {
        int k = get_key();
        if (k == 27) break;
        if (k == 200) try_move(0, -1);
        else if (k == 201) try_move(0, 1);
        else if (k == 202) try_move(-1, 0);
        else if (k == 203) try_move(1, 0);
        else if (k == 114) { if (level) load_level(level); }
        else continue;
        draw();
        if (won()) {
            print("Solved in ");
            print_int(moves);
            print_line(" moves.");
            beep(880, 60); beep(1180, 90);
            if (level == 1) { level = 2; load_level(2); draw(); print_line("Level 2."); }
            else break;
        }
    }
    sound_off();
    print_line("done");
    return 0;
}
