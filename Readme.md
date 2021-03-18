# Simple OpenGL Setup for Visual Studio Code

```
$ git clone --recursive --shallow-submodules git@github.com:lucatironi/cpp-gl-skeletal-animation.git
$ cd cpp-gl-skeletal-animation
```
## MacOSX/Linux
```
$ cmake -DCMAKE_BUILD_TYPE:STRING=Debug -B./build -G "Unix Makefiles"
$ make -C ./build
```
## Visual Studio Code
Menu `Tasks > Run Task` and select `cmake`. Then `Tasks > Run Build Task` and select `make`.