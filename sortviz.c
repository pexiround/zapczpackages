int arr[128];
int N;
int W;
int H;
int cmps;
int swaps;

void shuffle() {
    for (int i = 0; i < N; i++) arr[i] = i + 1;
    for (int i = N - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int t = arr[i]; arr[i] = arr[j]; arr[j] = t;
    }
    cmps = 0; swaps = 0;
}

void draw(int hi_a, int hi_b) {
    canvas_clear(14, 16, 24);
    int bw = W / N;
    if (bw < 1) bw = 1;
    for (int i = 0; i < N; i++) {
        int h = (arr[i] * (H - 12)) / N;
        int x = i * bw;
        int r = 90; int g = 160; int b = 240;
        if (i == hi_a) { r = 255; g = 90; b = 90; }
        else if (i == hi_b) { r = 250; g = 210; b = 80; }
        canvas_rect(x, H - h, bw - 1, h, r, g, b);
    }
    canvas_present();
}

void bubble() {
    for (int i = 0; i < N - 1; i++) {
        for (int j = 0; j < N - 1 - i; j++) {
            cmps = cmps + 1;
            if (arr[j] > arr[j + 1]) {
                int t = arr[j]; arr[j] = arr[j + 1]; arr[j + 1] = t;
                swaps = swaps + 1;
            }
            if ((cmps % 24) == 0) draw(j, j + 1);
        }
    }
    draw(-1, -1);
}

void insertion() {
    for (int i = 1; i < N; i++) {
        int key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > key) {
            cmps = cmps + 1;
            arr[j + 1] = arr[j];
            swaps = swaps + 1;
            j = j - 1;
            if ((swaps % 12) == 0) draw(j, i);
        }
        arr[j + 1] = key;
    }
    draw(-1, -1);
}

void selection() {
    for (int i = 0; i < N - 1; i++) {
        int m = i;
        for (int j = i + 1; j < N; j++) {
            cmps = cmps + 1;
            if (arr[j] < arr[m]) m = j;
        }
        if (m != i) {
            int t = arr[i]; arr[i] = arr[m]; arr[m] = t;
            swaps = swaps + 1;
        }
        draw(i, m);
    }
    draw(-1, -1);
}

void report(char* name) {
    print(name);
    print(": ");
    print_int(cmps);
    print(" comparisons, ");
    print_int(swaps);
    print_line(" swaps");
}

int main() {
    canvas_init(256, 160);
    W = canvas_width();
    H = canvas_height();
    N = 64;
    srand(ticks());

    print_line("Sorting visualizer");
    print_line("");

    shuffle(); draw(-1, -1); sleep(400);
    bubble();    report("bubble   ");
    sleep(600);

    shuffle(); draw(-1, -1); sleep(400);
    insertion(); report("insertion");
    sleep(600);

    shuffle(); draw(-1, -1); sleep(400);
    selection(); report("selection");

    print_line("");
    print_line("press esc");
    for (;;) { int k = get_key(); if (k == 27) break; }
    return 0;
}
