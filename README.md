# KWin Screen Corners

A native KWin screen-corner effect for KDE Plasma 6 on Wayland.

It uses OpenGL to draw black, antialiased rounded masks over the four corners of each display. It supports multiple displays, display scaling, and hot-plugging.

## Features

- KDE Plasma 6 / KWin Wayland
- Configurable corner radius
- Independent corner switches
- Antialiasing
- Multi-display and HiDPI support

## Pictures

### Real-world effect

<img width="2320" height="1481" alt="Real-world effect" src="https://github.com/user-attachments/assets/93db5d64-84c8-4a0e-ad17-461e952d1825" />

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
