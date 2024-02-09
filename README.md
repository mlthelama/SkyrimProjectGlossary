# SkyrimProjectGlossary
here should be some description

## End User Dependencies
* [SKSE64](https://skse.silverlock.org/)
* [Address Library for SKSE Plugins](https://www.nexusmods.com/skyrimspecialedition/mods/32444)

## Build Dependencies
* [CommonLibSSE NG](https://github.com/mlthelama/CommonLibSSE/tree/ng)
  - currently a [fork from](https://github.com/alandtse/CommonLibVR/tree/ng) with minor changes
  - added as [submodule](extern/CommonLibSSE-NG)
* [spdlog](https://github.com/gabime/spdlog) 
  - for logging
* [simpleini](https://github.com/brofield/simpleini) 
  - reading/writing to ini files
* [vcpkg](https://github.com/microsoft/vcpkg) 
  - Add the environment variable `VCPKG_ROOT` with the value as the path to the folder containing vcpkg
* [cmake](https://cmake.org) 
  - installed dir needs to be added to the `PATH` environment variable
  - Version `3.25`
* [imgui](https://github.com/ocornut/imgui)
  - for displaying ui elements
* [clib-util](https://github.com/powerof3/CLibUtil)
  - added as vcpkg [port](cmake/ports/clib-util)
* [unordered-dense](https://github.com/martinus/unordered_dense)
  - added as vcpkg [port](cmake/ports/unordered-dense)
* [imgui-markdonw](https://github.com/juliettef/imgui_markdown)
  - added [submodule](extern/imgui_markdown)

## Building
```
git clone https://github.com/mlthelama/SkyrimProjectGlossary.git
cd SkyrimProjectGlossary

git submodule update --init --recursive
cmake --preset vs2022-windows
cmake --build --preset vs2022-windows --config Release
```
