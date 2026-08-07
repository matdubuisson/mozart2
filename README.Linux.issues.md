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

## Missing memory space in /tmp.

Be aware the /tmp/ directory is used in the examples however this is a independent limited file system and building everything inside may lead to run out of memory. Therefore after each build, delete the sources before going to the next build.

For example an error during the LLVM archived sources extraction looks like :
```
....
tar: llvm-project-llvmorg-22.1.0/utils/bazel/vulkan_sdk.bzl: Cannot write: No space left on device
tar: Exiting with failure status due to previous errors
```

Indeed the filesystem `/tmp` ran out of memory.

```bash
$ df -h
Filesystem      Size  Used Avail Use% Mounted on
tmpfs           754M  2.9M  751M   1% /run
/dev/nvme0n1p6  120G   55G   59G  49% /
tmpfs           3.7G   54M  3.7G   2% /dev/shm
efivarfs        384K   97K  283K  26% /sys/firmware/efi/efivars
none            1.0M     0  1.0M   0% /run/credentials/systemd-journald.service
none            1.0M     0  1.0M   0% /run/credentials/systemd-resolved.service
tmpfs           3.7G  3.7G     0 100% /tmp
/dev/nvme0n1p5  511M   11M  501M   3% /boot/efi
tmpfs           754M   11M  744M   2% /run/user/1000
```

A solution is to give temporay more space to `/tmp/` using the mount command.

```bash
$ sudo mount -o remount,size=8G /tmp
```

After reboot everything will come back to normal.
