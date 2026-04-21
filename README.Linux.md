# Full building of Mozart2

This readme describes how to build the mozart code but also how to re-generate the pre-generated files with llvm.

## Install build tools

```
$ sudo apt update
$ sudo apt install make ninja-build
```

## Install gcc/g++ 15

```
$ sudo apt update
$ sudo apt install build-essential
$ sudo apt install -y gcc-15 g++-15
```

### Manage multiple gcc/g++ versions

```
$ sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-14 14
$ sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-14 14
$ sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-15 15
$ sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-15 15
```

```
$ sudo update-alternatives --config gcc
There are 2 choices for the alternative gcc (providing /usr/bin/gcc).

  Selection    Path             Priority   Status
------------------------------------------------------------
* 0            /usr/bin/gcc-15   15        auto mode
  1            /usr/bin/gcc-14   14        manual mode
  2            /usr/bin/gcc-15   15        manual mode

Press <enter> to keep the current choice[*], or type selection number: 0
$ sudo update-alternatives --config g++
There are 2 choices for the alternative g++ (providing /usr/bin/g++).

  Selection    Path             Priority   Status
------------------------------------------------------------
* 0            /usr/bin/g++-15   15        auto mode
  1            /usr/bin/g++-14   14        manual mode
  2            /usr/bin/g++-15   15        manual mode

Press <enter> to keep the current choice[*], or type selection number: 0
```

```
$ gcc -v
Using built-in specs.
COLLECT_GCC=gcc
COLLECT_LTO_WRAPPER=/usr/libexec/gcc/x86_64-linux-gnu/15/lto-wrapper
OFFLOAD_TARGET_NAMES=nvptx-none:amdgcn-amdhsa
OFFLOAD_TARGET_DEFAULT=1
Target: x86_64-linux-gnu
Configured with: ../src/configure -v --with-pkgversion='Ubuntu 15-20250404-0ubuntu1' --with-bugurl=file:///usr/share/doc/gcc-15/README.Bugs --enable-languages=c,ada,c++,go,d,fortran,objc,obj-c++,m2,rust,cobol,algol68 --prefix=/usr --with-gcc-major-version-only --program-suffix=-15 --program-prefix=x86_64-linux-gnu- --enable-shared --enable-linker-build-id --libexecdir=/usr/libexec --without-included-gettext --enable-threads=posix --libdir=/usr/lib --enable-nls --enable-bootstrap --enable-clocale=gnu --enable-libstdcxx-debug --enable-libstdcxx-time=yes --with-default-libstdcxx-abi=new --enable-libstdcxx-backtrace --enable-gnu-unique-object --disable-vtable-verify --enable-plugin --enable-default-pie --with-system-zlib --enable-libphobos-checking=release --with-target-system-zlib=auto --enable-objc-gc=auto --enable-multiarch --disable-werror --enable-cet --with-arch-32=i686 --with-abi=m64 --with-multilib-list=m32,m64,mx32 --enable-multilib --with-tune=generic --enable-offload-targets=nvptx-none=/build/gcc-15-n9L2J0/gcc-15-15-20250404/debian/tmp-nvptx/usr,amdgcn-amdhsa=/build/gcc-15-n9L2J0/gcc-15-15-20250404/debian/tmp-gcn/usr --enable-offload-defaulted --without-cuda-driver --enable-checking=release --build=x86_64-linux-gnu --host=x86_64-linux-gnu --target=x86_64-linux-gnu --with-build-config=bootstrap-lto-lean --enable-link-serialization=2
Thread model: posix
Supported LTO compression algorithms: zlib zstd
gcc version 15.0.1 20250404 (experimental) [master r15-9193-g08e803aa9be] (Ubuntu 15-20250404-0ubuntu1) 
$ g++ -v
Using built-in specs.
COLLECT_GCC=g++
COLLECT_LTO_WRAPPER=/usr/libexec/gcc/x86_64-linux-gnu/15/lto-wrapper
OFFLOAD_TARGET_NAMES=nvptx-none:amdgcn-amdhsa
OFFLOAD_TARGET_DEFAULT=1
Target: x86_64-linux-gnu
Configured with: ../src/configure -v --with-pkgversion='Ubuntu 15-20250404-0ubuntu1' --with-bugurl=file:///usr/share/doc/gcc-15/README.Bugs --enable-languages=c,ada,c++,go,d,fortran,objc,obj-c++,m2,rust,cobol,algol68 --prefix=/usr --with-gcc-major-version-only --program-suffix=-15 --program-prefix=x86_64-linux-gnu- --enable-shared --enable-linker-build-id --libexecdir=/usr/libexec --without-included-gettext --enable-threads=posix --libdir=/usr/lib --enable-nls --enable-bootstrap --enable-clocale=gnu --enable-libstdcxx-debug --enable-libstdcxx-time=yes --with-default-libstdcxx-abi=new --enable-libstdcxx-backtrace --enable-gnu-unique-object --disable-vtable-verify --enable-plugin --enable-default-pie --with-system-zlib --enable-libphobos-checking=release --with-target-system-zlib=auto --enable-objc-gc=auto --enable-multiarch --disable-werror --enable-cet --with-arch-32=i686 --with-abi=m64 --with-multilib-list=m32,m64,mx32 --enable-multilib --with-tune=generic --enable-offload-targets=nvptx-none=/build/gcc-15-n9L2J0/gcc-15-15-20250404/debian/tmp-nvptx/usr,amdgcn-amdhsa=/build/gcc-15-n9L2J0/gcc-15-15-20250404/debian/tmp-gcn/usr --enable-offload-defaulted --without-cuda-driver --enable-checking=release --build=x86_64-linux-gnu --host=x86_64-linux-gnu --target=x86_64-linux-gnu --with-build-config=bootstrap-lto-lean --enable-link-serialization=2
Thread model: posix
Supported LTO compression algorithms: zlib zstd
gcc version 15.0.1 20250404 (experimental) [master r15-9193-g08e803aa9be] (Ubuntu 15-20250404-0ubuntu1)
```

## Install OpenJDK 21

```
$ sudo apt update
$ sudo apt install -y openjdk-21-jdk
```

```
$ sudo update-alternatives --config java
There are 3 choices for the alternative java (providing /usr/bin/java).

  Selection    Path                                            Priority   Status
------------------------------------------------------------
  0            /usr/lib/jvm/java-25-openjdk-amd64/bin/java      2511      auto mode
  1            /usr/lib/jvm/java-21-openjdk-amd64/bin/java      2111      manual mode
  2            /usr/lib/jvm/java-25-openjdk-amd64/bin/java      2511      manual mode
* 3            /usr/lib/jvm/java-8-openjdk-amd64/jre/bin/java   1081      manual mode

Press <enter> to keep the current choice[*], or type selection number: 1
$ sudo update-alternatives --config javac
There are 3 choices for the alternative javac (providing /usr/bin/javac).

  Selection    Path                                          Priority   Status
------------------------------------------------------------
  0            /usr/lib/jvm/java-25-openjdk-amd64/bin/javac   2511      auto mode
  1            /usr/lib/jvm/java-21-openjdk-amd64/bin/javac   2111      manual mode
  2            /usr/lib/jvm/java-25-openjdk-amd64/bin/javac   2511      manual mode
* 3            /usr/lib/jvm/java-8-openjdk-amd64/bin/javac    1081      manual mode

Press <enter> to keep the current choice[*], or type selection number: 1
update-alternatives: using /usr/lib/jvm/java-21-openjdk-amd64/bin/javac to provide /usr/bin/javac (javac) in manual mode
```

```
$ java -version
openjdk version "21.0.9" 2025-10-21
OpenJDK Runtime Environment (build 21.0.9+10-Ubuntu-125.04)
OpenJDK 64-Bit Server VM (build 21.0.9+10-Ubuntu-125.04, mixed mode, sharing)
$ javac -version
javac 21.0.9
```

## Install cmake-4.3.1 from sources

### Download the sources

```
$ cd
$ mkdir -p builds/
$ cd builds/
$ wget https://cmake.org/files/v4.3/cmake-4.3.1.tar.gz
$ tar -xvf cmake-4.3.1.tar.gz
$ cd cmake-4.3.1/
```

### Compile the sources

```
$ sed -i '/"lib64"/s/64//' Modules/GNUInstallDirs.cmake
$ ./bootstrap --prefix=/usr        \
            --system-libs        \
            --mandir=/share/man  \
            --no-system-jsoncpp  \
            --no-system-cppdap   \
            --no-system-librhash \
            --docdir=/share/doc/cmake-4.3.1
```

See [here](./README.Linux.issues.md#missing-dependencies-cmake-installation) if you have some cmake errors about missing dependencies after these two steps.

By default make builds one thing at a time but you could speed up the compilation by allowing it to use multiple cores with `make -j$(nproc)`.

```
$ make
$ ./bin/cmake --version
cmake version 4.3.1

CMake suite maintained and supported by Kitware (kitware.com/cmake).
```

```
$ sudo make install
$ cmake --version
cmake version 4.3.1

CMake suite maintained and supported by Kitware (kitware.com/cmake).
```

## Install llvm 22.1.0 from sources

```
$ grep -rl '#!.*python$' | xargs sed -i '1s/python$/python3/'
$ sed 's/utility/tool/' -i llvm/utils/FileCheck/CMakeLists.txt
```

```
$ mkdir -v llvm/build/
$ cd llvm/build/
$ CC=gcc CXX=g++                               \
cmake -D CMAKE_INSTALL_PREFIX=/usr           \
      -D CMAKE_SKIP_INSTALL_RPATH=ON         \
      -D LLVM_ENABLE_FFI=ON                  \
      -D CMAKE_BUILD_TYPE=Release            \
      -D LLVM_BUILD_LLVM_DYLIB=ON            \
      -D LLVM_LINK_LLVM_DYLIB=ON             \
      -D LLVM_ENABLE_RTTI=ON                 \
      -D LLVM_TARGETS_TO_BUILD="host;AMDGPU" \
      -D LLVM_ENABLE_PROJECTS=clang          \
      -D LLVM_ENABLE_RUNTIMES=compiler-rt    \
      -D LLVM_BINUTILS_INCDIR=/usr/include   \
      -D LLVM_INCLUDE_BENCHMARKS=OFF         \
      -D CLANG_DEFAULT_PIE_ON_LINUX=ON       \
      -D CLANG_CONFIG_FILE_SYSTEM_DIR=/etc/clang \
      -W no-dev -G Ninja ..
$ ninja
```

See [here](./README.Linux.issues.md#missing-dependencies-llvm-installation) if you have some cmake errors about missing dependencies after these two steps.

Of course, we can specify several cores to ninja in order to speed up compilation with the following command `ninja -j($nproc)`.

Finally install it and check the version.

```
$ sudo ninja install
$ llvm-config --version
22.1.0
```

By the way, if you pay attention to the option `-D LLVM_ENABLE_PROJECTS=clang` it asked cmake to build clang as well.
Today llvm includes all its dependencies/subprojects inside its own source code to ease the building of it.

```
$ clang -v
clang version 22.1.0
Target: x86_64-unknown-linux-gnu
Thread model: posix
InstalledDir: /usr/bin
System configuration file directory: /etc/clang
Found candidate GCC installation: /usr/bin/../lib/gcc/x86_64-linux-gnu/14
Found candidate GCC installation: /usr/bin/../lib/gcc/x86_64-linux-gnu/15
Selected GCC installation: /usr/bin/../lib/gcc/x86_64-linux-gnu/15
Candidate multilib: .;@m64
Selected multilib: .;@m64
```

## Fully compile Mozart2

```
$ whereis llvm
llvm: /usr/include/llvm
```

```
$ mkdir build/
$ cd build/
```

## Sources

- [Manage multiple gcc/g++ versions](https://linuxconfig.org/how-to-switch-between-multiple-gcc-and-g-compiler-versions-on-ubuntu-20-04-lts-focal-fossa)
- [Manage multiple java/javac versions](https://batsov.com/articles/2021/12/10/working-with-multiple-versions-of-java-on-ubuntu/)
- [Install CMake 3.4.1](https://www.linuxfromscratch.org/blfs/view/systemd/general/cmake.html)
- [Install libcurl](https://stackoverflow.com/questions/34914944/could-not-find-curl-missing-curl-library-curl-include-dir-on-cmake)
- [Install libarchive](https://stackoverflow.com/questions/78343136/getting-libarchive-not-found-error-while-trying-to-publish-to-shinyapp)
- [File plugin-api.h not found when building llvm](https://github.com/llvm/llvm-project/issues/64216)
- [Install binutils](https://lindevs.com/install-binutils-on-ubuntu)