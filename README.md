# KWin Screen Corners

[简体中文](README.zh-CN.md)

A native KWin screen-corner effect for KDE Plasma 6 on Wayland.

It uses OpenGL to draw black, antialiased rounded masks over the four corners of each display. It supports multiple displays, display scaling, and hot-plugging.

## Features

- KDE Plasma 6 / KWin Wayland
- Configurable corner radius
- Independent corner switches
- Antialiasing
- Multi-display and HiDPI support

## Build

```bash
cmake --preset clang
cmake --build build-clang --parallel 2
```

## Package

```bash
dpkg-buildpackage -us -uc -b
```

The Debian package is generated in the parent directory of the project.

## Install

```bash
sudo apt install ./kwin-screen-corners_*.deb
```

Then open:

```text
System Settings → Window Management → Desktop Effects → KWin Screen Corners
```

Enable and configure the effect there.

## Screenshots

### Configuration UI

![Configuration UI](docs/images/configuration-placeholder.png)

> Placeholder: add a screenshot of the configuration UI here.

### Real-world effect

![Real-world effect](docs/images/effect-photo-placeholder.png)

> Placeholder: add a photo of the effect on the desktop here.

## License

[GNU Affero General Public License v3.0 or later](LICENSE)
