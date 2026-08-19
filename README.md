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

Everything lives in the repository root:

| path         | what it is                                          |
|--------------|-----------------------------------------------------|
| `index.txt`  | the catalog the OS fetches first                     |
| `*.ZAP`      | compiled package binaries                            |
| `*.c`        | the C source for every package, so it's inspectable  |

The OS looks for `NAME.ZAP` in the root first, and falls back to
`packages/NAME.ZAP`, so either layout works.

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

Then drop the `.ZAP` in the repo root, add a line to `index.txt`, and push.
The `size` field must match the file exactly -- the OS checks it and refuses
the install if it doesn't, which catches a corrupted upload.

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
| raycast | Graphics | First-person maze raycaster, arrows or WASD |
| mandel | Graphics | Mandelbrot explorer, pan and zoom, no FPU |
| plasma | Graphics | Animated demoscene plasma effect |
| fire | Graphics | Classic demoscene fire effect |
| stars | Graphics | Starfield flythrough, W/S changes speed |
| sortviz | Graphics | Sorting algorithm visualizer with stats |
| maze | Graphics | Maze generator plus breadth-first solver |
| invaders | Games | Space shooter, A/D to move, space to fire |
| breakout | Games | Paddle, ball and bricks, arrows or A/D |
| sokoban | Games | Push crates onto goals, arrows or WASD |
| simon | Games | Memory game using the PC speaker |
| hangman | Games | Guess the computing word |
| typing | Tools | Typing speed test, words per minute |
| hexview | Tools | Hex dump any file on the disk |
| textadv | Tools | The Kernel Escape, a text adventure |
| rpncalc | Tools | Reverse polish calculator |
| sysbench | Tools | Benchmark ZapCC generated code |
