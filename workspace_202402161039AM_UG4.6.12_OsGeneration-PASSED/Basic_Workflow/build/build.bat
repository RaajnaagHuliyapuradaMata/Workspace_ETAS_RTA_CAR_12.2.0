@echo off
if not exist CMakeBuild mkdir CMakeBuild
pushd CMakeBuild

@rem ensure CMake and MinGW are on the path
@rem set PATH=C:\Program Files\CMake\bin;C:\<replace_this>\i686-8.1.0-release-win32-dwarf-rt_v6-rev0\mingw32\bin;%PATH%

echo Running CMake
cmake .. -D CMAKE_C_COMPILER=gcc -D CMAKE_C++_COMPILER=g++ -D CMAKE_GENERATOR="MinGW Makefiles" || popd && exit /B
echo Running MinGW Makefiles
mingw32-make -j8 || popd && exit /B
echo - Build successful -
popd