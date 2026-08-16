int board[64];
int side;
int best_from;
int best_to;
int best_promo;
int nodes;
char line[64];

int val[7];
int pst_pawn[64];
int pst_knight[64];
int pst_king[64];

int KDIR[8];
int NDIR[8];
int BDIR[4];
int RDIR[4];

int mvf[480];
int mvt[480];
int mvp[480];
int mv_n;
int mv_base;

void setup_tables() {
    val[0] = 0; val[1] = 100; val[2] = 320; val[3] = 330; val[4] = 500; val[5] = 900; val[6] = 20000;

    KDIR[0] = -9; KDIR[1] = -8; KDIR[2] = -7; KDIR[3] = -1;
    KDIR[4] = 1;  KDIR[5] = 7;  KDIR[6] = 8;  KDIR[7] = 9;
    NDIR[0] = -17; NDIR[1] = -15; NDIR[2] = -10; NDIR[3] = -6;
    NDIR[4] = 6;   NDIR[5] = 10;  NDIR[6] = 15;  NDIR[7] = 17;
    BDIR[0] = -9; BDIR[1] = -7; BDIR[2] = 7; BDIR[3] = 9;
    RDIR[0] = -8; RDIR[1] = -1; RDIR[2] = 1; RDIR[3] = 8;

    for (int i = 0; i < 64; i++) {
        int r = i / 8;
        int f = i % 8;
        int centre = 0;
        int df = f; if (df > 3) df = 7 - f;
        int dr = r; if (dr > 3) dr = 7 - r;
        centre = (df + dr) * 4;
        pst_knight[i] = centre - 12;
        pst_king[i] = -centre;
        pst_pawn[i] = (6 - r) * 6 + centre / 2;
    }
}

void set_start() {
    static int back[8];
    back[0] = 4; back[1] = 2; back[2] = 3; back[3] = 5;
    back[4] = 6; back[5] = 3; back[6] = 2; back[7] = 4;
    for (int i = 0; i < 64; i++) board[i] = 0;
    for (int f = 0; f < 8; f++) {
        board[f] = -back[f];
        board[8 + f] = -1;
        board[48 + f] = 1;
        board[56 + f] = back[f];
    }
    side = 1;
}

int piece_char(int p) {
    int a = p; if (a < 0) a = -a;
    if (a == 1) return 80;
    if (a == 2) return 78;
    if (a == 3) return 66;
    if (a == 4) return 82;
    if (a == 5) return 81;
    if (a == 6) return 75;
    return 46;
}

void show() {
    print_line("");
    print_line("    a b c d e f g h");
    for (int r = 0; r < 8; r++) {
        print("  ");
        print_int(8 - r);
        put_char(32);
        for (int f = 0; f < 8; f++) {
            int p = board[r * 8 + f];
            int c = piece_char(p);
            if (p < 0 && c != 46) c = c + 32;
            put_char(c);
            put_char(32);
        }
        print_int(8 - r);
        put_char(10);
    }
    print_line("    a b c d e f g h");
    print_line("");
}

int on_board(int sq) { return sq >= 0 && sq < 64; }

int step_ok(int from, int to) {
    if (!on_board(to)) return 0;
    int ff = from % 8;
    int tf = to % 8;
    int d = ff - tf; if (d < 0) d = -d;
    if (d > 2) return 0;
    return 1;
}

void add_move(int from, int to, int promo) {
    if (mv_n >= 95) return;
    int i = mv_base + mv_n;
    mvf[i] = from;
    mvt[i] = to;
    mvp[i] = promo;
    mv_n = mv_n + 1;
}

void gen_slide(int sq, int me, int* dirs, int ndir) {
    for (int d = 0; d < ndir; d++) {
        int cur = sq;
        for (int k = 0; k < 8; k++) {
            int nxt = cur + dirs[d];
            if (!step_ok(cur, nxt)) break;
            int t = board[nxt];
            if (t * me > 0) break;
            add_move(sq, nxt, 0);
            if (t != 0) break;
            cur = nxt;
        }
    }
}

void gen_moves(int me, int ply) {
    mv_base = ply * 96;
    mv_n = 0;
    for (int sq = 0; sq < 64; sq++) {
        int p = board[sq];
        if (p * me <= 0) continue;
        int a = p; if (a < 0) a = -a;
        if (a == 1) {
            int fwd = -8;
            int start_rank = 6;
            if (me < 0) { fwd = 8; start_rank = 1; }
            int one = sq + fwd;
            if (on_board(one) && board[one] == 0) {
                int r = one / 8;
                if (r == 0 || r == 7) add_move(sq, one, 5);
                else {
                    add_move(sq, one, 0);
                    int two = sq + fwd * 2;
                    if (sq / 8 == start_rank && on_board(two) && board[two] == 0) add_move(sq, two, 0);
                }
            }
            int cap1 = sq + fwd - 1;
            int cap2 = sq + fwd + 1;
            if (step_ok(sq, cap1) && board[cap1] * me < 0) {
                int r = cap1 / 8;
                add_move(sq, cap1, (r == 0 || r == 7) ? 5 : 0);
            }
            if (step_ok(sq, cap2) && board[cap2] * me < 0) {
                int r = cap2 / 8;
                add_move(sq, cap2, (r == 0 || r == 7) ? 5 : 0);
            }
        } else if (a == 2) {
            for (int d = 0; d < 8; d++) {
                int nxt = sq + NDIR[d];
                if (!on_board(nxt)) continue;
                int ff = sq % 8; int tf = nxt % 8;
                int df = ff - tf; if (df < 0) df = -df;
                if (df > 2) continue;
                if (board[nxt] * me > 0) continue;
                add_move(sq, nxt, 0);
            }
        } else if (a == 3) {
            gen_slide(sq, me, BDIR, 4);
        } else if (a == 4) {
            gen_slide(sq, me, RDIR, 4);
        } else if (a == 5) {
            gen_slide(sq, me, BDIR, 4);
            gen_slide(sq, me, RDIR, 4);
        } else {
            for (int d = 0; d < 8; d++) {
                int nxt = sq + KDIR[d];
                if (!step_ok(sq, nxt)) continue;
                if (board[nxt] * me > 0) continue;
                add_move(sq, nxt, 0);
            }
        }
    }
}

int king_alive(int me) {
    for (int i = 0; i < 64; i++) if (board[i] == 6 * me) return 1;
    return 0;
}

int evaluate() {
    int sc = 0;
    for (int i = 0; i < 64; i++) {
        int p = board[i];
        if (p == 0) continue;
        int a = p; if (a < 0) a = -a;
        int v = val[a];
        int bonus = 0;
        if (a == 1) bonus = (p > 0) ? pst_pawn[i] : pst_pawn[63 - i];
        else if (a == 2) bonus = pst_knight[i];
        else if (a == 6) bonus = pst_king[i];
        if (p > 0) sc = sc + v + bonus;
        else sc = sc - v - bonus;
    }
    return sc;
}

int search(int depth, int me, int alpha, int beta, int root, int ply) {
    if (!king_alive(me)) return -900000 + (6 - depth) * 100;
    if (depth == 0) {
        nodes = nodes + 1;
        return evaluate() * me;
    }

    gen_moves(me, ply);
    int n = mv_n;
    int base = ply * 96;
    if (n == 0) return -800000;

    int best = -999999;
    for (int i = 0; i < n; i++) {
        int from = mvf[base + i];
        int to = mvt[base + i];
        int promo = mvp[base + i];
        int moved = board[from];
        int captured = board[to];
        board[to] = promo ? (promo * me) : moved;
        board[from] = 0;

        int sc = -search(depth - 1, -me, -beta, -alpha, 0, ply + 1);

        board[from] = moved;
        board[to] = captured;

        if (sc > best) {
            best = sc;
            if (root) { best_from = from; best_to = to; best_promo = promo; }
        }
        if (sc > alpha) alpha = sc;
        if (alpha >= beta) return best;
    }
    return best;
}

int sq_from_text(char* s, int i) {
    int f = s[i] - 97;
    int r = s[i + 1] - 49;
    if (f < 0 || f > 7 || r < 0 || r > 7) return -1;
    return (7 - r) * 8 + f;
}

int legal_for_human(int from, int to) {
    gen_moves(1, 0);
    for (int i = 0; i < mv_n; i++)
        if (mvf[i] == from && mvt[i] == to) return 1;
    return 0;
}

void print_sq(int sq) {
    put_char(97 + (sq % 8));
    put_char(49 + (7 - sq / 8));
}

int main() {
    setup_tables();
    set_start();

    print_line("ZapczOS Chess");
    print_line("You are White (uppercase). Enter moves like e2e4.");
    print_line("Type 'new' to restart, 'quit' to leave.");
    show();

    for (;;) {
        print("your move> ");
        int n = read_line(line, 64);
        if (n == 0) continue;
        if (strcmp(line, "quit") == 0) { print_line("bye"); return 0; }
        if (strcmp(line, "new") == 0) { set_start(); show(); continue; }
        if (n < 4) { print_line("format is e2e4"); continue; }

        int from = sq_from_text(line, 0);
        int to = sq_from_text(line, 2);
        if (from < 0 || to < 0) { print_line("format is e2e4"); continue; }
        if (board[from] <= 0) { print_line("that is not one of your pieces"); continue; }
        if (!legal_for_human(from, to)) { print_line("illegal move"); continue; }

        int moved = board[from];
        int promo = 0;
        if (moved == 1 && to / 8 == 0) promo = 5;
        board[to] = promo ? promo : moved;
        board[from] = 0;
        show();

        if (!king_alive(-1)) { print_line("You win. Checkmate."); beep(900, 80); beep(1200, 160); return 0; }

        print_line("thinking...");
        nodes = 0;
        best_from = -1;
        int t0 = ticks();
        search(3, -1, -999999, 999999, 1, 0);
        int t1 = ticks();

        if (best_from < 0) { print_line("I have no moves. You win."); return 0; }

        int bm = board[best_from];
        int bpromo = best_promo;
        board[best_to] = bpromo ? -bpromo : bm;
        board[best_from] = 0;

        print("I play ");
        print_sq(best_from);
        print_sq(best_to);
        print("   (");
        print_int(nodes);
        print(" positions, ");
        print_int((t1 - t0) * 10);
        print_line(" ms)");
        beep(600, 40);
        show();

        if (!king_alive(1)) { print_line("I win. Better luck next time."); beep(200, 300); return 0; }
    }
}
