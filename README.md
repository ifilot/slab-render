# SlabRender

## Purpose
SlabRender is a (hopefully) intuitive GUI to automatically produce molecular
structures in Blender derived from VASP, ADF, MKMCXX3 (mks) and Gaussian type
calculations.

> [!NOTE]
> SlabRender has been tested and verified with **Blender 3.3 LTS** and **Blender 4.5 LTS**.

> [!TIP]
> If you are using Windows, you can download a pre-built installer from the  
> 👉 **[Releases](../../releases)** page.

## Usage
Select a type of file from the dropdown menu. Next, select a root folder.
SlabRender will aim to automatically grab all files in all folders. When you are
satisfied with the list, you can either select a single file and produce an
image for that file or produce images for all the files in the queue.

> [!WARNING]
> **Each geometry file must reside in its own directory.**  
> Placing multiple geometry files in the same folder may cause rendered images
> to be overwritten.

## Supported files
* VASP POSCAR/CONTCAR
* ADF logfile
* Gaussian .log/.LOG files
* [MKMCXX3](https://www.mkmcxx.nl)'s `.mks` files

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
    mingw-w64-x86_64-qt5 \
    mingw-w64-x86_64-boost \
    mingw-w64-x86_64-eigen3 \
    mingw-w64-x86_64-glm
cd build
cmake .. -G Ninja
ninja
```