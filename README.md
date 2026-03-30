# Gomoku

A command-line Gomoku (Five in a Row) game written in C++17.

## Rules

- 15×15 board; stones are placed on intersections.
- Black (`X`) moves first, then White (`O`), alternating.
- First player to form five or more consecutive stones in a row, column, or
  diagonal wins.

## Build

```bash
cmake -B build
cmake --build build
```

The binary is placed at `build/gomoku`.

## Usage

```bash
./build/gomoku
```

Use the main menu to choose a mode:

| Key | Mode |
|-----|------|
| 1   | Single Player vs AI *(not yet implemented)* |
| 2   | Local Two Player |
| 3   | Network Multiplayer *(not yet implemented)* |
| 4   | Replay |
| 5   | Settings |
| 0   | Quit |

### Local Two Player

Enter coordinates to place a stone. Accepted formats:

- `H8` — column letter (A–O) followed by row number (1–15)
- `8 8` — row then column, both 1-indexed

Type `undo` to take back the last move (must be enabled in Settings).

### Replay

Enter the path to a `.gom` file. Use `n` / `p` to step forward / backward,
and `q` to return to the main menu.

### Settings

Toggle the undo feature and the per-move time limit. Settings are persisted
to `gomoku.cfg` in the working directory.

## Save Format (`.gom`)

Games are saved as human-readable text files:

```
# Gomoku Game Record
# Format version 1

[metadata]
date = 2026-03-31
duration_sec = 312
total_moves = 23
winner = black
undo_enabled = false
time_limit_enabled = false
time_limit_seconds = 60

[moves]
# num  player  row  col
1      black   7    7
2      white   7    8
```

- `row` and `col` are 0-indexed integers.
- `winner` is `black`, `white`, or `draw`.

## Code Style

Google C++ style enforced via `clang-format`:

```bash
clang-format --dry-run --Werror src/*.cpp include/*.h
```
