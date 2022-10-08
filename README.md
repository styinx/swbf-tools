# swbf-tools

[![Build](https://github.com/styinx/swbf-tools/actions/workflows/build.yml/badge.svg)](https://github.com/styinx/swbf-tools/actions/workflows/build.yml)

## Development

### Checkout repository

`git clone --recursive https://github.com/styinx/swbf-tools`

### Create CMake files

Replace <argument> parameters i.e:
- `<VS Version>` = Visual Studio 17 2022
- `<build type>` = Debug
- `<vcpkg-path>` = C:/Path/to/vcpkg/

```
mkdri build
cd build
cmake .. -G "<VS Version>" -A Win32 -DCMAKE_BUILD_TYPE=<build type> -DDIR_VCPKG_ROOT:PATH="<vcpkg-path>" -DVCPKG_MANIFEST_INSTALL:PATH="<vcpkg-path>/installed" -DVCPKG_TARGET_TRIPLET:STRING="x64-windows-static" -DCMAKE_TOOLCHAIN_FILE:PATH="<vcpkg-path>/scripts/buildsystems/vcpkg.cmake" -Wno-dev
cmake --build . 
```

### Run from CMD

```
".\SWBF-Tools Launcher.exe" --dir "<swbf directory>" --dll "<dll path>" --exe "<swbf executable path>" <swbf arguments (like /win /resolution 600 800, ...)>*
```
