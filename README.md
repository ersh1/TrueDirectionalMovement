## Requirements
* [CMake](https://cmake.org/)
	* Add this to your `PATH`
* [The Elder Scrolls V: Skyrim Special Edition](https://store.steampowered.com/app/489830)
	* Add the environment variable `CompiledPluginsPath` to point to the folder where you want the .dll to be copied after building
* [Vcpkg](https://github.com/microsoft/vcpkg)
	* Add the environment variable `VCPKG_ROOT` with the value as the path to the folder containing vcpkg
* [Visual Studio Community 2022](https://visualstudio.microsoft.com/)
	* Desktop development with C++
* [CommonLibSSE](https://github.com/ersh1/CommonLibSSE/tree/dev)
	* You need to build from the ersh1/dev branch, or ersh1/dev-ae branch if you're building for post-AE versions of Skyrim.
	* Add the environment variable `CommonLibSSEPath_SE` and/or `CommonLibSSEPath_AE` with the value as the path to the folder containing the correct CommonLibSSE version

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
