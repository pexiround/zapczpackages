int W;
int H;
int bricks[80];
int BW;
int BH;
int paddle_x;
int paddle_w;
int ball_x;
int ball_y;
int ball_dx;
int ball_dy;
int score;
int lives;
int alive_bricks;

void reset_ball() {
    ball_x = paddle_x + paddle_w / 2;
    ball_y = H - 24;
    ball_dx = 2;
    ball_dy = -2;
}

void build_level() {
    BW = 10;
    BH = 6;
    alive_bricks = 0;
    for (int i = 0; i < BW * BH; i++) { bricks[i] = 1; alive_bricks = alive_bricks + 1; }
}

int bc_r;
int bc_g;
int bc_b;

void brick_color(int row) {
    if (row == 0) { bc_r = 230; bc_g = 80; bc_b = 80; }
    else if (row == 1) { bc_r = 230; bc_g = 150; bc_b = 60; }
    else if (row == 2) { bc_r = 220; bc_g = 210; bc_b = 70; }
    else if (row == 3) { bc_r = 100; bc_g = 200; bc_b = 100; }
    else if (row == 4) { bc_r = 80; bc_g = 160; bc_b = 230; }
    else { bc_r = 160; bc_g = 110; bc_b = 220; }
}

void draw() {
    canvas_clear(16, 18, 28);
    int bw = W / BW;
    int bh = 8;
    for (int r = 0; r < BH; r++) {
        for (int c = 0; c < BW; c++) {
            if (!bricks[r * BW + c]) continue;
            brick_color(r);
            canvas_rect(c * bw + 1, 10 + r * bh + 1, bw - 2, bh - 2, bc_r, bc_g, bc_b);
        }
    }
    canvas_rect(paddle_x, H - 10, paddle_w, 4, 220, 220, 240);
    canvas_rect(ball_x - 2, ball_y - 2, 4, 4, 255, 240, 160);
    canvas_present();
}

int main() {
    canvas_init(220, 170);
    W = canvas_width();
    H = canvas_height();
    paddle_w = 34;
    paddle_x = W / 2 - paddle_w / 2;
    score = 0;
    lives = 3;
    build_level();
    reset_ball();

    print_line("Breakout -- left/right or A/D to move, esc to quit");

    for (;;) {
        int k = key_down();
        if (k == 27) break;
        if (k == 97 || k == 65) k = 202;   /* a */
        if (k == 100 || k == 68) k = 203;  /* d */
        if (k == 202) paddle_x = paddle_x - 10;
        if (k == 203) paddle_x = paddle_x + 10;
        if (paddle_x < 0) paddle_x = 0;
        if (paddle_x > W - paddle_w) paddle_x = W - paddle_w;

        ball_x = ball_x + ball_dx;
        ball_y = ball_y + ball_dy;

        if (ball_x < 2) { ball_x = 2; ball_dx = -ball_dx; beep(500, 8); }
        if (ball_x > W - 3) { ball_x = W - 3; ball_dx = -ball_dx; beep(500, 8); }
        if (ball_y < 2) { ball_y = 2; ball_dy = -ball_dy; beep(500, 8); }

        if (ball_y >= H - 12 && ball_y <= H - 8) {
            if (ball_x >= paddle_x && ball_x <= paddle_x + paddle_w) {
                ball_dy = -abs(ball_dy);
                int off = ball_x - (paddle_x + paddle_w / 2);
                ball_dx = off / 6;
                if (ball_dx == 0) ball_dx = 1;
                if (ball_dx > 3) ball_dx = 3;
                if (ball_dx < -3) ball_dx = -3;
                beep(880, 10);
            }
        }

        if (ball_y > H) {
            lives = lives - 1;
            beep(180, 120);
            if (lives <= 0) {
                print("Game over. Final score ");
                print_int(score);
                put_char(10);
                break;
            }
            reset_ball();
        }

        int bw = W / BW;
        int bh = 8;
        if (ball_y >= 10 && ball_y < 10 + BH * bh) {
            int c = ball_x / bw;
            int r = (ball_y - 10) / bh;
            if (c >= 0 && c < BW && r >= 0 && r < BH) {
                if (bricks[r * BW + c]) {
                    bricks[r * BW + c] = 0;
                    alive_bricks = alive_bricks - 1;
                    score = score + (BH - r) * 10;
                    ball_dy = -ball_dy;
                    beep(1200 - r * 100, 12);
                    if (alive_bricks == 0) {
                        print("You cleared it. Score ");
                        print_int(score);
                        put_char(10);
                        build_level();
                        reset_ball();
                    }
                }
            }
        }

        draw();
        sleep(20);
    }
    sound_off();
    return 0;
}
