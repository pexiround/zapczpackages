char buf[8192];
char name[64];
int size;

int hexdig(int v) {
    v = v & 15;
    if (v < 10) return 48 + v;
    return 65 + (v - 10);
}

void put_hex2(int v) {
    put_char(hexdig(v >> 4));
    put_char(hexdig(v));
}

void put_hex4(int v) {
    put_hex2((v >> 8) & 255);
    put_hex2(v & 255);
}

void dump(int from, int lines) {
    for (int l = 0; l < lines; l++) {
        int off = from + l * 16;
        if (off >= size) break;
        put_hex4(off);
        print("  ");
        for (int i = 0; i < 16; i++) {
            if (off + i < size) put_hex2(buf[off + i] & 255);
            else print("  ");
            put_char(32);
            if (i == 7) put_char(32);
        }
        print(" |");
        for (int i = 0; i < 16; i++) {
            if (off + i >= size) break;
            int c = buf[off + i] & 255;
            if (c >= 32 && c < 127) put_char(c);
            else put_char(46);
        }
        print("|");
        put_char(10);
    }
}

int main() {
    print_line("Hex viewer -- reads any file off the disk.");
    print("filename> ");
    int n = read_line(name, 64);
    if (n == 0) { print_line("cancelled"); return 0; }

    if (!file_exists(name)) { print("no such file: "); print_line(name); return 1; }

    int fs = file_size(name);
    print("size: ");
    print_int(fs);
    print_line(" bytes");

    size = file_read(name, buf, 8192);
    if (size <= 0) { print_line("could not read it"); return 1; }
    if (fs > 8192) print_line("(showing the first 8192 bytes)");
    print_line("");

    int pos = 0;
    dump(pos, 16);
    print_line("");
    print_line("space = next page, b = back, esc = quit");

    for (;;) {
        int k = get_key();
        if (k == 27) break;
        if (k == 32) { pos = pos + 256; if (pos >= size) pos = pos - 256; }
        else if (k == 98) { pos = pos - 256; if (pos < 0) pos = 0; }
        else continue;
        print_line("");
        dump(pos, 16);
    }
    print_line("done");
    return 0;
}
