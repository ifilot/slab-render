# Saucepan

## Purpose
Saucepan is a (hopefully) intuitive GUI to automatically produce molecular structures
in Blender derived from VASP, ADF, or Gaussian type calculations.

**Saucepan has been tested for Blender 3.3 LTS**

## Usage
Select a type of file from the dropdown menu. Next, select a root folder. Saucepan will
aim to automatically grab all files in all folders. When you are satisfied with the list,
you can either select a single file and produce an image for that file or produce images
for all the files in the queue.

**Important: The program assumes that each geometry file resides in a separate directory.**

## Supported files
* VASP POSCAR/CONTCAR
* ADF logfile
* Gaussian .log/.LOG files

## Compilation

### Linux

```bash
sudo apt update
sudo apt install -y \
    build-essential \
    cmake \
    ninja-build \
    qtbase5-dev qttools5-dev \
    libboost-filesystem-dev \
    libboost-regex-dev \
    libboost-iostreams-dev \
    libeigen3-dev \
    libglm-dev
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j
```

### MinGW

```bash
pacman -Syu
pacman -S --needed \
    mingw-w64-x86_64-toolchain \
    mingw-w64-x86_64-cmake \
    mingw-w64-x86_64-ninja \
    mingw-w64-x86_64-qt6 \
    mingw-w64-x86_64-boost \
    mingw-w64-x86_64-eigen3 \
    mingw-w64-x86_64-glm
cd build
cmake .. -G Ninja
ninja
```