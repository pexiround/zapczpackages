int MW;
int MH;
int cell[800];
int stk[800];
int dist[800];
int que[800];
int sp;
int W;
int H;
int CS;

int idx(int x, int y) { return y * MW + x; }
int inb(int x, int y) { return x >= 0 && y >= 0 && x < MW && y < MH; }

void draw_cell(int x, int y, int r, int g, int b) {
    canvas_rect(x * CS, y * CS, CS, CS, r, g, b);
}

void draw_all() {
    canvas_clear(12, 14, 20);
    for (int y = 0; y < MH; y++)
        for (int x = 0; x < MW; x++)
            if (cell[idx(x, y)] == 0) draw_cell(x, y, 96, 104, 124);
            else draw_cell(x, y, 22, 26, 36);
    canvas_present();
}

void carve() {
    for (int i = 0; i < MW * MH; i++) cell[i] = 0;
    int cx = 1;
    int cy = 1;
    cell[idx(cx, cy)] = 1;
    sp = 0;
    stk[sp] = idx(cx, cy); sp = 1;

    while (sp > 0) {
        cx = stk[sp - 1] % MW;
        cy = stk[sp - 1] / MW;
        int dirs[4];
        int nd = 0;
        if (inb(cx, cy - 2) && cell[idx(cx, cy - 2)] == 0) { dirs[nd] = 0; nd = nd + 1; }
        if (inb(cx, cy + 2) && cell[idx(cx, cy + 2)] == 0) { dirs[nd] = 1; nd = nd + 1; }
        if (inb(cx - 2, cy) && cell[idx(cx - 2, cy)] == 0) { dirs[nd] = 2; nd = nd + 1; }
        if (inb(cx + 2, cy) && cell[idx(cx + 2, cy)] == 0) { dirs[nd] = 3; nd = nd + 1; }

        if (nd == 0) { sp = sp - 1; continue; }
        int d = dirs[rand() % nd];
        int nx = cx;
        int ny = cy;
        if (d == 0) ny = cy - 2;
        else if (d == 1) ny = cy + 2;
        else if (d == 2) nx = cx - 2;
        else nx = cx + 2;

        cell[idx((cx + nx) / 2, (cy + ny) / 2)] = 1;
        cell[idx(nx, ny)] = 1;
        stk[sp] = idx(nx, ny); sp = sp + 1;

        if ((sp % 3) == 0) {
            draw_cell(nx, ny, 70, 200, 140);
            canvas_present();
        }
    }
    draw_all();
}

void solve() {
    for (int i = 0; i < MW * MH; i++) dist[i] = -1;
    int head = 0;
    int tail = 0;
    que[tail] = idx(1, 1); tail = tail + 1;
    dist[idx(1, 1)] = 0;
    int gx = MW - 2;
    int gy = MH - 2;

    while (head < tail) {
        int x = que[head] % MW;
        int y = que[head] / MW;
        head = head + 1;
        if (x == gx && y == gy) break;
        int d = dist[idx(x, y)];
        for (int k = 0; k < 4; k++) {
            int nx = x;
            int ny = y;
            if (k == 0) ny = y - 1;
            else if (k == 1) ny = y + 1;
            else if (k == 2) nx = x - 1;
            else nx = x + 1;
            if (!inb(nx, ny)) continue;
            if (cell[idx(nx, ny)] == 0) continue;
            if (dist[idx(nx, ny)] >= 0) continue;
            dist[idx(nx, ny)] = d + 1;
            que[tail] = idx(nx, ny); tail = tail + 1;
            int shade = 40 + (d * 3) % 160;
            draw_cell(nx, ny, 40, shade, 200 - shade / 2);
        }
        if ((head % 12) == 0) canvas_present();
    }
    canvas_present();

    if (dist[idx(gx, gy)] < 0) { print_line("no path"); return; }

    int x = gx;
    int y = gy;
    int steps = 0;
    while (!(x == 1 && y == 1)) {
        draw_cell(x, y, 250, 210, 70);
        int d = dist[idx(x, y)];
        int moved = 0;
        for (int k = 0; k < 4 && !moved; k++) {
            int nx = x;
            int ny = y;
            if (k == 0) ny = y - 1;
            else if (k == 1) ny = y + 1;
            else if (k == 2) nx = x - 1;
            else nx = x + 1;
            if (!inb(nx, ny)) continue;
            if (dist[idx(nx, ny)] == d - 1) { x = nx; y = ny; moved = 1; }
        }
        if (!moved) break;
        steps = steps + 1;
        if ((steps % 4) == 0) canvas_present();
    }
    draw_cell(1, 1, 90, 240, 120);
    draw_cell(gx, gy, 250, 90, 90);
    canvas_present();
    print("shortest path: ");
    print_int(dist[idx(gx, gy)]);
    print_line(" steps");
}

int main() {
    canvas_init(248, 186);
    W = canvas_width();
    H = canvas_height();
    CS = 8;
    MW = W / CS;
    MH = H / CS;
    if ((MW % 2) == 0) MW = MW - 1;
    if ((MH % 2) == 0) MH = MH - 1;
    srand(ticks());

    print_line("Maze -- recursive backtracker builds it,");
    print_line("breadth-first search solves it.");
    print_line("Green = start, red = goal, yellow = shortest path.");
    print_line("Press any key for a new maze, esc to quit.");

    for (;;) {
        carve();
        sleep(300);
        solve();
        int k = get_key();
        if (k == 27) break;
    }
    print_line("done");
    return 0;
}
