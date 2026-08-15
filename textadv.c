char cmd[128];
char world[2048];
int room;
int have_key;
int have_lamp;
int lamp_on;
int done;

void look() {
    print_line("");
    if (room == 0) {
        print_line("-- Boot Sector --");
        print_line("A cramped room of humming magnetic platters.");
        print_line("A dusty LAMP sits on a ledge. Exits: north.");
    } else if (room == 1) {
        print_line("-- Kernel Heap --");
        print_line("Blocks of memory drift past, some allocated,");
        print_line("some free. A brass KEY glints inside one.");
        print_line("Exits: south, east.");
    } else if (room == 2) {
        if (!lamp_on) {
            print_line("-- Somewhere Dark --");
            print_line("It is pitch black. You could easily lose a");
            print_line("pointer in here. Exits: unknown.");
        } else {
            print_line("-- Interrupt Vector Table --");
            print_line("256 gates line the walls, each humming with");
            print_line("its own handler. A locked DOOR marks gate 80h.");
            print_line("Exits: west, and the door if it were open.");
        }
    } else if (room == 3) {
        print_line("-- User Space --");
        print_line("Sunlight. Actual sunlight. You made it out of");
        print_line("ring zero. You win.");
    }
}

void help() {
    print_line("commands: look, north, south, east, west,");
    print_line("          take lamp, take key, light lamp,");
    print_line("          open door, inventory, quit");
}

void go(int dir) {
    if (room == 0) {
        if (dir == 0) { room = 1; look(); return; }
    } else if (room == 1) {
        if (dir == 1) { room = 0; look(); return; }
        if (dir == 2) { room = 2; look(); return; }
    } else if (room == 2) {
        if (dir == 3) { room = 1; look(); return; }
    }
    print_line("You cannot go that way.");
}

int starts(char* s, char* pre) {
    int i = 0;
    while (pre[i] != 0) {
        if (s[i] != pre[i]) return 0;
        i = i + 1;
    }
    return 1;
}

int main() {
    room = 0;
    have_key = 0;
    have_lamp = 0;
    lamp_on = 0;
    done = 0;

    print_line("THE KERNEL ESCAPE");
    print_line("A tiny text adventure running on ZapczOS.");
    print_line("Type 'help' for commands.");
    look();

    while (!done) {
        print("");
        print("> ");
        int n = read_line(cmd, 128);
        if (n == 0) continue;

        if (starts(cmd, "quit")) { print_line("Goodbye."); break; }
        else if (starts(cmd, "help")) help();
        else if (starts(cmd, "look")) look();
        else if (starts(cmd, "north") || starts(cmd, "n")) go(0);
        else if (starts(cmd, "south") || starts(cmd, "s")) go(1);
        else if (starts(cmd, "east") || starts(cmd, "e")) go(2);
        else if (starts(cmd, "west") || starts(cmd, "w")) go(3);
        else if (starts(cmd, "inv")) {
            print("You carry:");
            if (have_lamp) print(" a lamp");
            if (have_key) print(" a brass key");
            if (!have_lamp && !have_key) print(" nothing at all");
            put_char(10);
        }
        else if (starts(cmd, "take lamp")) {
            if (room == 0 && !have_lamp) { have_lamp = 1; print_line("Taken."); }
            else print_line("There is no lamp here.");
        }
        else if (starts(cmd, "take key")) {
            if (room == 1 && !have_key) { have_key = 1; print_line("Taken. It is warm."); }
            else print_line("There is no key here.");
        }
        else if (starts(cmd, "light")) {
            if (!have_lamp) print_line("You have no lamp.");
            else { lamp_on = 1; print_line("The lamp flickers on."); look(); }
        }
        else if (starts(cmd, "open door")) {
            if (room != 2) print_line("There is no door here.");
            else if (!lamp_on) print_line("You cannot find it in the dark.");
            else if (!have_key) print_line("It is locked. You need a key.");
            else {
                print_line("The key turns. The door swings open.");
                beep(700, 60); beep(950, 90);
                room = 3;
                look();
                done = 1;
            }
        }
        else print_line("I do not understand that.");
    }
    sound_off();
    return 0;
}
