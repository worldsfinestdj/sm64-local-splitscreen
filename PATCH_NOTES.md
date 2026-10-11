# Local Split-Screen Edition — Patch Notes

This edition is a local, two-player split-screen build based on Isaac0-dev's
`splitscreen` branch of SM64CoopDX. It is set up for playing together on one
Windows PC with two controllers.

## Added for this edition

- A two-player local split-screen setup, with the unused third and fourth
  viewports disabled.
- Independent character and palette editing for Player 1 and Player 2.
- Four named save slots, with load, rename, and delete controls.
- An offline local-play flow, so a host or online lobby is not required.
- Windows launcher instructions and a desktop shortcut in the local install.

## Gameplay fixes

- Cannon use now tracks the player who entered it, allowing either local
  player to aim and fire.
- Cannon reticles are placed at the center of the active split-screen viewport.
- Removed a Player 1-only restriction from slide handling so the same logic is
  available to both local players.

## Built and tested on

- Windows
- MSYS2 MinGW64
- Two local controllers

## Notes for players

This repository contains source code only. It does not include a Super Mario
64 ROM, extracted game assets, compiled game files, saves, controller settings,
or other personal files. Build it with a legally obtained compatible ROM that
you provide yourself.

See [CREDITS.md](CREDITS.md) for upstream projects and contributor credit.
