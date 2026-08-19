char line[128];
char target[96];

void pick(int n) {
    if (n == 0) strcpy(target, "the kernel boots before anything else");
    else if (n == 1) strcpy(target, "a compiler turns source into machine code");
    else if (n == 2) strcpy(target, "every packet needs a checksum");
    else if (n == 3) strcpy(target, "memory is cheap but pointers are dangerous");
    else if (n == 4) strcpy(target, "zapcz systems archive");
    else if (n == 5) strcpy(target, "interrupts arrive when you least expect them");
    else if (n == 6) strcpy(target, "a filesystem is just a very careful list");
    else strcpy(target, "hello from an operating system i wrote myself");
}

int main() {
    srand(ticks());
    print_line("Typing test -- type the line exactly, then press enter.");
    print_line("Three rounds. esc quits.");
    print_line("");

    int total_chars = 0;
    int total_ticks = 0;
    int total_wrong = 0;

    for (int round = 0; round < 3; round++) {
        pick(rand() % 8);
        print("  ");
        print_line(target);
        print("> ");

        int t0 = ticks();
        int n = read_line(line, 128);
        int t1 = ticks();
        if (n == 0) { print_line("cancelled"); break; }

        int len = strlen(target);
        int wrong = 0;
        for (int i = 0; i < len; i++) {
            if (line[i] != target[i]) wrong = wrong + 1;
        }
        int typed = strlen(line);
        if (typed != len) wrong = wrong + abs(typed - len);

        int dt = t1 - t0;
        if (dt < 1) dt = 1;

        total_chars = total_chars + len;
        total_ticks = total_ticks + dt;
        total_wrong = total_wrong + wrong;

        print("  time ");
        print_int(dt / 100);
        print(" s, mistakes ");
        print_int(wrong);
        put_char(10);
        put_char(10);
    }

    if (total_ticks > 0 && total_chars > 0) {
        int seconds = total_ticks / 100;
        if (seconds < 1) seconds = 1;
        int wpm = (total_chars * 60) / (5 * seconds);
        int acc = 100 - (total_wrong * 100) / total_chars;
        if (acc < 0) acc = 0;
        print("RESULT: ");
        print_int(wpm);
        print(" wpm, ");
        print_int(acc);
        print_line("% accuracy");
        if (wpm > 40) beep(900, 60);
    }
    sound_off();
    return 0;
}
