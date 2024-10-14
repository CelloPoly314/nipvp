# nipvp
A plugin that turns the Co-op mode into a PvP mode for Naruto Shippuden: Ultimate Ninja Impact.
## Features
- PvP mode.
- Removes all other enimies.
- Balanced HP and Chakra.
- Replaces spawnpoints in certain maps.
- Works on both JP and USA version (`NPJH50435`/`ULUS10582`).
- Works on PSP, Vita and PPSSPP.
## Installation
- Download [nipvp.prx](https://github.com/CelloPoly314/nipvp/releases/download/0.3/nipvp.prx) and copy it into `seplugins/` folder in the root of your storage.

  - For PS Vita, it should be `ux0:pspemu/seplugins/` by default.

  
- Write this line into `seplugins/game.txt`:
  ```
  ms0:/seplugins/nipvp.prx 1
  ```
  - If you're using PSP Go internal storage, write `ef0:/seplugins/nipvp.prx 1` instead.

- For PPSSPP, Download [nipvp.zip](https://github.com/CelloPoly314/nipvp/releases/download/0.3/nipvp.zip) and extract the folder into `PPSSPP/PSP/PLUGINS/`

## Known Issues
- Awakening is not implemented because it would break the balance even more.
## Special Thanks
- ermaccer for his plugin template that make it possible for me to turn my cheat codes into an actual plugin.
