# -G "Visual Studio 17 2022" -A Win32 -DCMAKE_BUILD_TYPE=Debug -DDIR_VCPKG_ROOT:PATH="<vcpkg-path>" -DVCPKG_MANIFEST_INSTALL:PATH="<vcpkg-path>/installed" -DVCPKG_TARGET_TRIPLET:STRING="x64-windows-static" -DCMAKE_TOOLCHAIN_FILE:PATH="<vcpkg-path>/scripts/buildsystems/vcpkg.cmake" -Wno-dev
project(swbf-tools)

cmake_minimum_required(VERSION 3.21)

#
# Paths
#

# Local paths
set(DIR_ROOT "${PROJECT_SOURCE_DIR}")
set(DIR_INCLUDE "${DIR_ROOT}/include")
set(DIR_SOURCE "${DIR_ROOT}/source")
set(DIR_LIBRARY "${DIR_ROOT}/lib")
set(DIR_TEST "${DIR_ROOT}/test")

# VCPKG paths
set(DIR_VCPKG_INSTALL "${DIR_VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}")
set(DIR_VCPKG_INCLUDE "${DIR_VCPKG_INSTALL}/include")
set(DIR_VCPKG_LIBRARY "${DIR_VCPKG_INSTALL}/lib")
set(DIR_VCPKG_LIBRARY_DEBUG "${DIR_VCPKG_INSTALL}/debug/lib")

# Binary output paths
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${DIR_LIBRARY}/deb")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${DIR_LIBRARY}/deb")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${DIR_ROOT})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${DIR_LIBRARY}/rel")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${DIR_LIBRARY}/rel")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${DIR_ROOT})


#
# CXX
#

set(CMAKE_CXX_STANDARD 17)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang") # Clang
    #
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU") # GCC
    set(COMPILER_FLAGS "-Wall -Wextra -ansi -pedantic")
    set(COMPILER_FLAGS_DEBUG "-MTd")
    set(COMPILER_FLAGS_RELEASE "-O3 -DNDEBUG -MT")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC") # MSVC
    set(COMPILER_FLAGS "")
    set(COMPILER_FLAGS_DEBUG "/Zi /Ob0 /Od /RTC1 /MDd")
    set(COMPILER_FLAGS_RELEASE "/O2 /Ob2 /DNDEBUG /MD")
endif ()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${COMPILER_FLAGS} ${COMPILER_FLAGS_DEBUG}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${COMPILER_FLAGS} ${COMPILER_FLAGS_RELEASE}")


# Dependencies

add_subdirectory(DLLInject)
add_subdirectory(detours)
add_subdirectory(lua)
add_subdirectory(minhook)
set_target_properties(minhook PROPERTIES DEBUG_POSTFIX "")
set_target_properties(minhook PROPERTIES RELEASE_POSTFIX "")
set_target_properties(minhook PROPERTIES RELWITHDEBINFO_POSTFIX "")
set_target_properties(minhook PROPERTIES MINSIZEREL_POSTFIX "")
set_target_properties(minhook PROPERTIES PREFIX "lib")

# CMAKE

set(INCLUDE_DIRECTORIES
        ${DIR_INCLUDE}
        ${DIR_SOURCE}

        DLLInject/include/
        imgui/
        lua/
        minhook/include
        detours/src

        ${DIR_VCPKG_INCLUDE})

include_directories(${INCLUDE_DIRECTORIES})

if (CMAKE_BUILD_TYPE EQUAL "DEBUG")
    set(LIBRARY_DIRECTORIES
            ${DIR_VCPKG_LIBRARY}
            ${DIR_VCPKG_LIBRARY}/deb)
else()
    set(LIBRARY_DIRECTORIES
            ${DIR_VCPKG_LIBRARY}
            ${DIR_VCPKG_LIBRARY}/rel)
endif()

link_directories(${LIBRARY_DIRECTORIES} log.c/)


# Sources

set(SOURCES
        source/Launcher.cpp

        source/SplashScreen.cpp)


set(DLL_SOURCES
        imgui/imgui.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_tables.cpp
        imgui/imgui_widgets.cpp
        imgui/imgui_demo.cpp
        imgui/backends/imgui_impl_win32.cpp
        imgui/backends/imgui_impl_dx9.cpp

        lua/ldebug.c

        source/hook/Hook.cpp
        source/hook/IMGUIWindow.cpp
        source/hook/DX9Hook.cpp
        source/hook/LuaHook.cpp
        source/hook/menus/SWBFToolsMenu.cpp
)


# Binaries

# Launcher
add_executable(Launcher source/main.cpp ${SOURCES})
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set_target_properties(Launcher PROPERTIES OUTPUT_NAME "SWBF-Tools Launcher")
    set_target_properties(Launcher PROPERTIES LINK_FLAGS /SUBSYSTEM:WINDOWS)
    set_target_properties(Launcher PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif ()
target_link_libraries(Launcher PRIVATE dllinject shlwapi)

# Demo
add_executable(LuaSample source/demolua.cpp)
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set_target_properties(LuaSample PROPERTIES OUTPUT_NAME "DemoLua")
    set_target_properties(LuaSample PROPERTIES LINK_FLAGS /SUBSYSTEM:WINDOWS)
    set_target_properties(LuaSample PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif ()
target_link_libraries(LuaSample PRIVATE dllinject lua d3d9 d3dcompiler minhook imm32 dwmapi detours)

# DLL
add_library(swbf-tools SHARED source/hook/dll.cpp ${DLL_SOURCES})
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set_target_properties(swbf-tools PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif ()
target_link_libraries(swbf-tools lua d3d9 d3dcompiler minhook imm32 dwmapi detours)

