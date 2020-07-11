# ukui-greeter

![Check build](https://github.com/ukui/ukui-greeter/workflows/Check%20build/badge.svg?branch=master)

Lightdm greeter for UKUI

## Getting started

Clone this repository and update all submodule.

```
git clone https://github.com/ukui/ukui-greeter.git
git submodule init
git submodule update
```

Install necessary dependencies:

* `Ubuntu/Debian`: `qttools5-dev-tools`, `pkg-config`, `qtbase5-dev`, `libqt5x11extras5-dev`, `libqt5svg5-dev`, `liblightdm-qt5-3-dev`, `libx11-dev`, `libxtst-dev`, `libxrandr-dev`, `qttools5-dev-tools`, `libimlib2-dev`

Use `qmake` generate _Makefile_, then build the project.

```
mkdir -p build && cd build
qmake ../
make -j$(nproc)
```
