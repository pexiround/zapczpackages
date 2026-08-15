int stack[64];
int sp;
char line[128];
char tok[32];

void push(int v) { if (sp < 64) { stack[sp] = v; sp = sp + 1; } }
int pop() { if (sp > 0) { sp = sp - 1; return stack[sp]; } return 0; }

int is_digit(int c) { return c >= 48 && c <= 57; }

int parse_int(char* s) {
    int v = 0; int i = 0; int neg = 0;
    if (s[0] == 45) { neg = 1; i = 1; }
    while (s[i] != 0) {
        if (!is_digit(s[i])) return 0;
        v = v * 10 + (s[i] - 48);
        i = i + 1;
    }
    if (neg) return -v;
    return v;
}

int looks_numeric(char* s) {
    int i = 0;
    if (s[0] == 45 && s[1] != 0) i = 1;
    if (s[i] == 0) return 0;
    while (s[i] != 0) { if (!is_digit(s[i])) return 0; i = i + 1; }
    return 1;
}

void show() {
    print("  [");
    for (int i = 0; i < sp; i++) { print_int(stack[i]); if (i < sp - 1) put_char(32); }
    print("]");
    put_char(10);
}

int main() {
    sp = 0;
    print_line("ZapczOS RPN calculator");
    print_line("Enter numbers and operators separated by spaces.");
    print_line("Operators: + - * / % ^  |  commands: drop dup swap clear quit");
    print_line("Example:  3 4 + 2 *   gives 14");
    print_line("");

    for (;;) {
        print("rpn> ");
        int n = read_line(line, 128);
        if (n == 0) { if (sp > 0) show(); continue; }

        int i = 0;
        int bad = 0;
        while (line[i] != 0) {
            while (line[i] == 32) i = i + 1;
            if (line[i] == 0) break;
            int t = 0;
            while (line[i] != 0 && line[i] != 32 && t < 31) { tok[t] = line[i]; t = t + 1; i = i + 1; }
            tok[t] = 0;

            if (strcmp(tok, "quit") == 0) { print_line("bye"); return 0; }
            else if (strcmp(tok, "clear") == 0) sp = 0;
            else if (strcmp(tok, "drop") == 0) pop();
            else if (strcmp(tok, "dup") == 0) { int a = pop(); push(a); push(a); }
            else if (strcmp(tok, "swap") == 0) { int a = pop(); int b = pop(); push(a); push(b); }
            else if (looks_numeric(tok)) push(parse_int(tok));
            else if (strcmp(tok, "+") == 0) { int a = pop(); int b = pop(); push(b + a); }
            else if (strcmp(tok, "-") == 0) { int a = pop(); int b = pop(); push(b - a); }
            else if (strcmp(tok, "*") == 0) { int a = pop(); int b = pop(); push(b * a); }
            else if (strcmp(tok, "/") == 0) { int a = pop(); int b = pop(); if (a == 0) { print_line("  divide by zero"); bad = 1; } else push(b / a); }
            else if (strcmp(tok, "%") == 0) { int a = pop(); int b = pop(); if (a == 0) { print_line("  divide by zero"); bad = 1; } else push(b % a); }
            else if (strcmp(tok, "^") == 0) { int a = pop(); int b = pop(); push(pow(b, a)); }
            else { print("  unknown: "); print_line(tok); bad = 1; }
            if (bad) break;
        }
        if (!bad) show();
    }
}
