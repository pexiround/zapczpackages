int arr[1000];

int fib(int n) { if (n < 2) return n; return fib(n - 1) + fib(n - 2); }

int count_primes(int limit) {
    int c = 0;
    for (int n = 2; n < limit; n++) {
        int is = 1;
        for (int d = 2; d * d <= n; d++) if (n % d == 0) { is = 0; d = n; }
        if (is) c = c + 1;
    }
    return c;
}

void fill(int n) {
    int seed = 12345;
    for (int i = 0; i < n; i++) {
        seed = (seed * 1103515245 + 12345) & 1073741823;
        arr[i] = seed % 10000;
    }
}

void sort_it(int n) {
    for (int i = 1; i < n; i++) {
        int key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > key) { arr[j + 1] = arr[j]; j = j - 1; }
        arr[j + 1] = key;
    }
}

int report(char* label, int t0, int t1, int weight) {
    int dt = t1 - t0;
    if (dt < 1) dt = 1;
    print("  ");
    print(label);
    print("  ");
    print_int(dt * 10);
    print_line(" ms");
    return weight / dt;
}

int main() {
    print_line("ZapczOS system benchmark");
    print_line("Everything below runs on machine code produced by");
    print_line("ZapCC, the C compiler built into this OS.");
    print_line("");

    int total = 0;

    print_line("running...");
    int t0 = ticks();
    int f = fib(21);
    int t1 = ticks();
    total = total + report("recursion (fib 21)  ", t0, t1, 40000);

    t0 = ticks();
    int p = count_primes(20000);
    t1 = ticks();
    total = total + report("primes under 20000  ", t0, t1, 60000);

    t0 = ticks();
    fill(1000);
    sort_it(1000);
    t1 = ticks();
    total = total + report("insertion sort 1000 ", t0, t1, 50000);

    t0 = ticks();
    int acc = 0;
    for (int i = 1; i < 200000; i++) acc = acc + (i % 7) * (i % 11);
    t1 = ticks();
    total = total + report("integer mix 200k    ", t0, t1, 30000);

    print_line("");
    print("fib(21)      = "); print_int(f); put_char(10);
    print("primes       = "); print_int(p); put_char(10);
    print("sorted[0]    = "); print_int(arr[0]); put_char(10);
    print("sorted[999]  = "); print_int(arr[999]); put_char(10);
    print("checksum     = "); print_int(acc); put_char(10);
    print_line("");
    print("ZAPSCORE     = ");
    print_int(total);
    put_char(10);
    print_line("");
    print_line("(higher is better; it is a rough integer-throughput score)");
    return 0;
}
