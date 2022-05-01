# Billig-Sweeper
This is a just another simple minesweeper clone.

![](data/Screenshots/bsw1.png)

# Usage
```
billig-sweeper [-h]
```

## Keybindings

| Key | Actions          |
|-----|------------------|
|  F1 | Open Github link |
|  m  | Open menu        |
|  r  | Restart game     |
|  q  | Quit             |

# Installation

## Dependencies
- meson (build-time)
- GIMP (only for editing `graphics.xcf`)
- libSDL2
- libSDL2_image

### Flatpak (Comming Soon)
```
flatpak install xyz.stuerz.BilligSweeper
```

### Arch/Manjaro
```
sudo pacman -S base-devel meson sdl2 sdl2_image
```

### Debian/Ubuntu
```
sudo apt update
sudo apt install build-essential meson libsdl2-dev libsdl2-image-dev
```

### RHEL/Fedora
```
sudo dnf install gcc meson sdl2-devel sdl2_image-devel
```

## Building
```
git clone https://github.com/riscygeek/billig-sweeper
cd billig-sweeper
meson setup build
meson compile -C build
meson install -C build
```

Note:

This project must be installed to work,
if you just want to try it out,
you should add `--prefix=$PWD/tmp` to `meson setup build`.

# TODO
- Add a config file
    - background color
    - keybinds
- Add checks for valid values in the menu
- Move metainfo from flathub to upstream
- Fix icon location for Flatpaks
- Enable support for Wayland.
- Add a help menu (for keybinds)
