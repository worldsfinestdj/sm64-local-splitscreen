# SM64 Local Split-Screen Edition

A personal two-player, local split-screen edition of
[SM64CoopDX](https://github.com/coop-deluxe/sm64coopdx), built from
[Isaac0-dev's `splitscreen` branch](https://github.com/Isaac0-dev/sm64coopdx/tree/splitscreen).

It is designed for two people playing together on one Windows PC with two
controllers. No online lobby or host setup is needed for local play.

## Features

- Two-player horizontal split-screen.
- Separate character and palette editing for each local player.
- Four selectable save slots with load, rename, and delete controls.
- Local offline play flow.
- Cannon controls for either local player, with a reticle centered in each
  player's viewport.

See [PATCH_NOTES.md](PATCH_NOTES.md) for the complete change list and
[CREDITS.md](CREDITS.md) for acknowledgements.

## Build on Windows

1. Install MSYS2 and open **MSYS2 MinGW x64**.
2. Install the project dependencies: GCC, make, Python, SDL2, and GLEW.
3. Put your legally obtained compatible Super Mario 64 ROM in the project
   folder as `baserom.us.z64`.
4. Run `make -j4` from the project folder.
5. Start the built `sm64coopdx.exe`.

The ROM, generated files, saves, controller settings, and compiled executables
are intentionally excluded from this repository.

## Credits and legal note

This is an unofficial personal fork. It preserves credit for the upstream
projects and contributors; see [CREDITS.md](CREDITS.md) and [credits.txt](credits.txt).
It contains no Super Mario 64 ROM or extracted Nintendo assets. You must use a
legally obtained compatible ROM that you provide yourself.
