
Minecraft-like C Program README
Welcome to our Minecraft-like C program project, a simple yet fascinating exploration of creating a voxel-based world similar to Minecraft using C and modern OpenGL. This project leverages an octree data structure for efficient block storage and rendering, providing an exciting way to understand game world construction and rendering techniques.

Features
Voxel-based world similar to Minecraft.
Octree data structure for efficient world representation and querying.
OpenGL for rendering the world with support for GLES and EGL for enhanced compatibility across devices.
X11 for window management on Linux systems.
Customizable world generation allowing for unique exploration experiences.
Dependencies
To build and run this project, you will need to install several dependencies, including libraries for OpenGL, GLES, EGL, and X11. Here's a quick guide on the dependencies required:

OpenGL: For rendering the game world.
GLES (OpenGL ES): For embedded systems support, if necessary.
EGL: As an interface between OpenGL, GLES, and the native windowing system.
X11: For window management on Linux.
CMake: For building the program from the provided CMakeLists.txt file.

Installing Dependencies
On Ubuntu/Debian-based systems
sudo apt-get update
sudo apt-get install libglu1-mesa-dev freeglut3-dev mesa-common-dev \
libx11-dev libegl1-mesa-dev libgles2-mesa-dev cmake

On Fedora/Red Hat-based systems
sudo dnf install mesa-libGLU-devel mesa-libGL-devel xorg-x11-server-devel \
mesa-libEGL-devel mesa-libGLES-devel cmake

On Arch Linux
sudo pacman -Syu mesa libglvnd xorg-server cmake
Building
This project uses CMake for building from source. Follow these steps to compile the program:

Clone the repository
git clone <repository-url>
cd <repository-name>

Create a build directory
mkdir build
cd build

Run CMake
cmake ..

Compile the project
make

After compilation, you can run the program from the build directory.

License
This project is licensed under the MIT License - see the LICENSE file for details.
