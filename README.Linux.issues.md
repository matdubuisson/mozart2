# Issues with Linux

We assume the Linux user is on a debian/ubuntu based linux OS.

## Missing dependencies cmake installation

You will probably hit the same errors (or worst other ones but the idea should remain the same) because your system has miss some libraries.

```
CMake Error at Source/Modules/CMakeBuildUtilities.cmake:138 (message):
  CMAKE_USE_SYSTEM_CURL is ON but a curl is not found!
Call Stack (most recent call first):
  CMakeLists.txt:475 (include)

....

CMake Error at Source/Modules/CMakeBuildUtilities.cmake:248 (message):
  CMAKE_USE_SYSTEM_LIBARCHIVE is ON but LibArchive is not found!
Call Stack (most recent call first):
  CMakeLists.txt:475 (include)
```

You could download binaries or build these missing libraries however for simplicity we will rely on apt to install them as described below.

```
$ sudo apt install curl libcurl4-openssl-dev
$ sudo apt install libarchive-dev
```

## Missing dependencies llvm installation

You could hit the below error causing by some missing modules because binutils-dev is not install.

```
FAILED: tools/gold/CMakeFiles/LLVMgold.dir/gold-plugin.cpp.o 
/usr/bin/g++ -D_GLIBCXX_USE_CXX11_ABI=1 -D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS -I/home/matdubuisson/builds/llvm-project-22.1.0.src/llvm/build/tools/gold -I/home/matdubuisson/builds/llvm-project-22.1.0.src/llvm/tools/gold -I/home/matdubuisson/builds/llvm-project-22.1.0.src/llvm/build/include -I/home/matdubuisson/builds/llvm-project-22.1.0.src/llvm/include -fPIC -fno-semantic-interposition -fvisibility-inlines-hidden -Werror=date-time -Wall -Wextra -Wno-unused-parameter -Wwrite-strings -Wcast-qual -Wno-missing-field-initializers -pedantic -Wno-long-long -Wimplicit-fallthrough -Wno-maybe-uninitialized -Wno-nonnull -Wno-class-memaccess -Wno-dangling-reference -Wno-redundant-move -Wno-pessimizing-move -Wno-array-bounds -Wno-stringop-overread -Wno-noexcept-type -Wdelete-non-virtual-dtor -Wsuggest-override -Wno-comment -Wno-misleading-indentation -Wctad-maybe-unsupported -fdiagnostics-color -ffunction-sections -fdata-sections -O3 -DNDEBUG -std=c++17 -fPIC -fno-exceptions -funwind-tables -MD -MT tools/gold/CMakeFiles/LLVMgold.dir/gold-plugin.cpp.o -MF tools/gold/CMakeFiles/LLVMgold.dir/gold-plugin.cpp.o.d -o tools/gold/CMakeFiles/LLVMgold.dir/gold-plugin.cpp.o -c /home/matdubuisson/builds/llvm-project-22.1.0.src/llvm/tools/gold/gold-plugin.cpp
/home/matdubuisson/builds/llvm-project-22.1.0.src/llvm/tools/gold/gold-plugin.cpp:39:10: fatal error: plugin-api.h: No such file or directory
   39 | #include <plugin-api.h>
      |          ^~~~~~~~~~~~~~
compilation terminated.
[3762/4451] Building CXX object tools/llvm-readobj/CMakeFiles/llvm-readobj.dir/ELFDumper.cpp.o
ninja: build stopped: subcommand failed.
```

Install the required packages to fix it.

```
$ sudo apt install -y binutils binutils-dev
```