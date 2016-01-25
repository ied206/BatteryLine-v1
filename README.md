# BatteryLine
BatteryLine shows system's battery status as line in screen.

Written in C, for Win32 platform.

# Usage
Compile and launch BatteryLine.exe.

# Compile
This program is built under this environment:
- CodeBlocks
- TDM-GCC-64 (based on MinGW-w64)

You can compile BatteryLine yourself using MinGW-w64.

## Using Code::Blocks
1. Open BatteryLine-dist.cbp using CodeBlocks
2. Open 'Projects' tab in Management panel (usually at left of screen)
3. Right-click project 'BatteryLine' and select 'Build Options'
4. Choose your favorite compiler, usually 'GNU GCC Compiler', then press OK button
5. Set Build targer to 'Release', then build project.
6. Binary will be generated in bin\Release\BatteryLine.exe.

## Using MinGW make
1. Add MinGW-w64 toolchain to PATH. Make sure 'gcc, g++, ar, windres, mingw32-make' can be called in console.
2. Build project with mingw32-make
3. To compile binary for release, type:
> mingw32-make

## Using Microsoft Visual Studio
This code can be compiled under C99 compliance compiler.

In theory, Visual C++ 2010 and later can compile this project.

If you success at compiling in MSVC and have a will to provide README or VC project template, please issue an pull request.

# License
This project is licensed under MIT License.

