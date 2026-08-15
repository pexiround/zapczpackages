char words[512];
char secret[24];
char guessed[32];
int nguessed;
int wrong;

void pick_word(int n) {
    if (n == 0) strcpy(secret, "KERNEL");
    else if (n == 1) strcpy(secret, "COMPILER");
    else if (n == 2) strcpy(secret, "FILESYSTEM");
    else if (n == 3) strcpy(secret, "BOOTLOADER");
    else if (n == 4) strcpy(secret, "INTERRUPT");
    else if (n == 5) strcpy(secret, "PACKET");
    else if (n == 6) strcpy(secret, "REGISTER");
    else if (n == 7) strcpy(secret, "POINTER");
    else if (n == 8) strcpy(secret, "ZAPCZOS");
    else strcpy(secret, "RAYCASTER");
}

int already(int c) {
    for (int i = 0; i < nguessed; i++) if (guessed[i] == c) return 1;
    return 0;
}

int show_word() {
    int missing = 0;
    print("  ");
    int n = strlen(secret);
    for (int i = 0; i < n; i++) {
        if (already(secret[i])) { put_char(secret[i]); }
        else { put_char(95); missing = missing + 1; }
        put_char(32);
    }
    put_char(10);
    return missing;
}

void draw_gallows() {
    print("  wrong: ");
    for (int i = 0; i < wrong; i++) put_char(88);
    for (int i = wrong; i < 6; i++) put_char(46);
    put_char(10);
}

int in_word(int c) {
    int n = strlen(secret);
    for (int i = 0; i < n; i++) if (secret[i] == c) return 1;
    return 0;
}

int main() {
    srand(ticks());
    pick_word(rand() % 10);
    nguessed = 0;
    wrong = 0;

    print_line("Hangman -- guess the computing word.");
    print_line("Type one letter and press enter. esc quits.");
    print_line("");

    for (;;) {
        int missing = show_word();
        draw_gallows();
        if (missing == 0) { print_line("  You got it!"); beep(880, 80); beep(1180, 120); break; }
        if (wrong >= 6) { print("  Out of guesses. It was "); print_line(secret); beep(160, 300); break; }

        print("letter> ");
        int k = get_key();
        if (k == 27) { print_line("bye"); break; }
        if (k >= 97 && k <= 122) k = k - 32;
        if (k < 65 || k > 90) { print_line(""); continue; }
        put_char(k); put_char(10);

        if (already(k)) { print_line("  already tried that"); continue; }
        if (nguessed < 31) { guessed[nguessed] = (char)k; nguessed = nguessed + 1; }
        if (in_word(k)) { beep(900, 40); }
        else { wrong = wrong + 1; beep(300, 80); }
    }
    sound_off();
    return 0;
}
