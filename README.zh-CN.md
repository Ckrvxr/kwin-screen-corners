# KWin Screen Corners

[English](README.md)

一个面向 KDE Plasma 6 Wayland 的原生 KWin 屏幕圆角特效插件。

插件使用 OpenGL 在每个显示器的四角绘制黑色抗锯齿圆角遮罩，支持多显示器、显示缩放和热插拔。

## 功能

- KDE Plasma 6 / KWin Wayland
- 可配置圆角半径
- 四角独立开关
- 抗锯齿
- 支持多显示器和 HiDPI 缩放

## 构建

```bash
cmake --preset clang
cmake --build build-clang --parallel 2
```

## 打包

```bash
dpkg-buildpackage -us -uc -b
```

生成的 Debian 软件包位于项目目录的上一级目录。

## 安装

```bash
sudo apt install ./kwin-screen-corners_*.deb
```

然后打开：

```text
系统设置 → 窗口管理 → 桌面特效 → KWin Screen Corners
```

在其中启用并配置特效。

## 效果预览

### 配置界面

![配置界面](docs/images/configuration-placeholder.png)

> 图片占位符：待补充配置界面截图。

### 实拍效果

![实拍效果](docs/images/effect-photo-placeholder.png)

> 图片占位符：待补充实际桌面效果照片。

## 许可证

[GNU Affero General Public License v3.0 或更高版本](LICENSE)
