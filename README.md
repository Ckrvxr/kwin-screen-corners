# KWin Screen Corners

A native KWin screen-corner effect for KDE Plasma 6 on Wayland.

It uses OpenGL to draw black, antialiased rounded masks over the four corners of each display. It supports multiple displays, display scaling, and hot-plugging.

## Features

- KDE Plasma 6 / KWin Wayland
- Configurable corner radius
- Independent corner switches
- Antialiasing
- Multi-display and HiDPI support

## Screenshots

### Real-world effect

<img width="3024" height="4032" alt="Real-world effect" src="https://github.com/user-attachments/assets/8b2b7e5d-e69c-43cb-b9ad-dc2ac0e0ed70" />

### Configuration UI

<img width="1910" height="702" alt="Configuration UI" src="https://github.com/user-attachments/assets/cf197ef4-a699-422d-bbee-13c5dbddfc74" />

## Install

```bash
sudo apt install ./kwin-screen-corners_*.deb
```

Then open:

```text
System Settings → Window Management → Desktop Effects → KWin Screen Corners
```

Enable and configure the effect there.

## Build and Package

```bash
cmake --preset clang
cmake --build build-clang --parallel 2
dpkg-buildpackage -us -uc -b
```

## License

[GNU Affero General Public License v3.0 or later](LICENSE)
