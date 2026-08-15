# ZapczOS Packages

The official package repository for **ZapczOS V5**.

ZapczOS downloads this index over HTTPS using its own from-scratch TLS 1.2
stack, fetches the `.ZAP` binaries, writes them to its FAT32 partition and
runs them on its own runtime. The packages are compiled by **ZapCC**, the C
compiler built into the operating system.

## Using it from inside ZapczOS

```
zap update            fetch the package index from this repo
zap list              show everything available (* = installed)
zap info <name>       details for one package
zap install <name>    download it over HTTPS and save it to disk
zap remove <name>     delete it
zaprun NAME.ZAP       run an installed package
```

## Layout

| path            | what it is                                        |
|-----------------|---------------------------------------------------|
| `index.txt`     | the catalog the OS fetches first                  |
| `packages/*.ZAP`| compiled package binaries                         |
| `src/*.c`       | the C source for every package, so it's inspectable |

## index.txt format

One package per line, `#` starts a comment:

```
name|version|size|category|description
```

`name` must be 8 characters or fewer — it becomes an 8.3 filename on disk
(`raycast` -> `RAYCAST.ZAP`).

## Writing your own package

Start from `src/hello.c`, which lists every builtin the runtime provides.
Compile it inside ZapczOS:

```
zapcc MYAPP.C          produces MYAPP.ZAP
zaprun MYAPP.ZAP       run it
```

Then drop the `.ZAP` in `packages/`, add a line to `index.txt`, and push.

### Language notes

ZapCC supports structs, pointers, 1D arrays, recursion, and the usual
operators and control flow. It does **not** have:

* floating point — everything is integer, use fixed point
* 2D arrays — use `grid[y * WIDTH + x]`
* `switch` — use `if` / `else if`
* function pointers

Limits are 64 KB of code and 16 KB of data per package.

## Packages

| name | category | what it does |
|------|----------|--------------|
| hello | Template | Starter template listing every builtin |
| raycast | Graphics | First-person maze raycaster, fixed point |
| mandel | Graphics | Mandelbrot explorer, pan and zoom, no FPU |
| plasma | Graphics | Animated demoscene plasma effect |
| sortviz | Graphics | Sorting algorithm visualizer with stats |
| breakout | Games | Paddle, ball and bricks with sound |
| sokoban | Games | Push crates onto goals, loads SOKO.TXT |
| simon | Games | Memory game using the PC speaker |
| hangman | Games | Guess the computing word |
| hexview | Tools | Hex dump any file on the disk |
| textadv | Tools | The Kernel Escape, a text adventure |
| rpncalc | Tools | Reverse polish calculator |
| sysbench | Tools | Benchmark ZapCC generated code |
