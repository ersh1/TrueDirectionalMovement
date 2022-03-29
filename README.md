## Requirements
* [CMake](https://cmake.org/)
	* Add this to your `PATH`
* [The Elder Scrolls V: Skyrim Special Edition](https://store.steampowered.com/app/489830)
	* Add the environment variable `CompiledPluginsPath` to point to the folder where you want the .dll to be copied after building
* [Vcpkg](https://github.com/microsoft/vcpkg)
	* Add the environment variable `VCPKG_ROOT` with the value as the path to the folder containing vcpkg
* [Visual Studio Community 2022](https://visualstudio.microsoft.com/)
	* Desktop development with C++
* [CommonLibSSE](https://github.com/powerof3/CommonLibSSE)
	* You need to build from the powerof3/dev branch
	* Add the environment variable `CommonLibSSEPath` with the value as the path to the folder containing CommonLibSSE

## User Requirements
* [Address Library for SKSE](https://www.nexusmods.com/skyrimspecialedition/mods/32444)

## Building
```
git clone https://github.com/ersh1/TrueDirectionalMovement/
cd TrueDirectionalMovement
git submodule init
git submodule update
```
### SSE
```
cmake --preset se
cmake --build build --config Release
```
### AE
```
cmake --preset ae
cmake --build build --config Release
```
