# Full building of Mozart2

This readme describes how to build the mozart code but also how to re-generate the pre-generated files with llvm.

## Install build tools

The cmake project will be compiled using make and LLVM/CLang will use ninja.

```bash
$ sudo apt update
$ sudo apt install make ninja-build
```

## Install gcc/g++ 15

```bash
$ sudo apt update
$ sudo apt install build-essential
$ sudo apt install -y gcc-15 g++-15
```

### Manage multiple gcc/g++ versions

#### Add new alternatives for gcc/g++

```bash
$ sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-14 14
$ sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-14 14
$ sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-15 15
$ sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-15 15
```

#### Select the latest alternative

```bash
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

#### Check the versions

```bash
$ gcc -v
gcc (Ubuntu 15-20250404-0ubuntu1) 15.0.1 20250404 (experimental) [master r15-9193-g08e803aa9be]
Copyright (C) 2025 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
$ g++ -v
g++ (Ubuntu 15-20250404-0ubuntu1) 15.0.1 20250404 (experimental) [master r15-9193-g08e803aa9be]
Copyright (C) 2025 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
```

## Install OpenJDK 21

The boot-compiler has been made in Scala using a version executed with Java 8. **It should be updated soon.**

```bash
$ sudo apt update
$ sudo apt install -y openjdk-8-jdk # Should migrate to openjdk-21-jdk soon
```

### Manage multiple openjdk versions

#### Select the latest java version

```bash
$ sudo update-alternatives --config java
There are 3 choices for the alternative java (providing /usr/bin/java).

  Selection    Path                                            Priority   Status
------------------------------------------------------------
  0            /usr/lib/jvm/java-25-openjdk-amd64/bin/java      2511      auto mode
  1            /usr/lib/jvm/java-21-openjdk-amd64/bin/java      2111      manual mode
  2            /usr/lib/jvm/java-25-openjdk-amd64/bin/java      2511      manual mode
* 3            /usr/lib/jvm/java-8-openjdk-amd64/jre/bin/java   1081      manual mode

Press <enter> to keep the current choice[*], or type selection number: 3
```

#### Check the java version

```bash
$ java -version
openjdk version "1.8.0_472"
OpenJDK Runtime Environment (build 1.8.0_472-8u472-ga-1~25.04-b08)
OpenJDK 64-Bit Server VM (build 25.472-b08, mixed mode)
```

## Install cmake-4.3.1 from sources

### Download the sources

```bash
$ cd
$ mkdir -p builds/
$ cd builds/
$ wget https://cmake.org/files/v4.3/cmake-4.3.1.tar.gz
$ tar -xvf cmake-4.3.1.tar.gz
$ cd cmake-4.3.1/
```

### Compile the sources

```bash
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

```bash
$ make
$ ./bin/cmake --version
cmake version 4.3.1

CMake suite maintained and supported by Kitware (kitware.com/cmake).
```

```bash
$ sudo make install
$ cmake --version
cmake version 4.3.1

CMake suite maintained and supported by Kitware (kitware.com/cmake).
```

## Install llvm 22.1.0 from sources

```bash
$ grep -rl '#!.*python$' | xargs sed -i '1s/python$/python3/'
$ sed 's/utility/tool/' -i llvm/utils/FileCheck/CMakeLists.txt
```

```bash
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

```bash
$ sudo ninja install
$ llvm-config --version
22.1.0
```

By the way, if you pay attention to the option `-D LLVM_ENABLE_PROJECTS=clang` it asked cmake to build clang as well.
Today llvm includes all its dependencies/subprojects inside its own source code to ease the building of it.

Check the installed CLang version.

```bash
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

```bash
$ whereis llvm
llvm: /usr/include/llvm
```

```bash
$ mkdir build/
$ cd build/
```

## Compile Mozart2 with pre-generated files

Here we describe how to make a full compilation of the Mozart project : all pre-generated sources used for templates and builtins are recompiled. You will need to do that if you develop new methods using templates or new OZ builtin modules.

```bash
$ cmake -S . -B build/ -DMOZART_CACHED_BUILD=OFF -DClang_DIR=/usr/lib/cmake/clang/ -DLLVM_DIR=/usr/lib/cmake/llvm/ -DCMAKE_BUILD_TYPE=Release
$ cd build/
$ make -B gensources genboostsources && make
```

An advice is to add all cores to compile faster and activate the verbose mode in order to see which commands fail to compile.

```bash
$ make -B gensources genboostsources VERBOSE=1 -j$(nproc) && make VERBOSE=1 -j$(nproc)
```

## Sources

- [Manage multiple gcc/g++ versions](https://linuxconfig.org/how-to-switch-between-multiple-gcc-and-g-compiler-versions-on-ubuntu-20-04-lts-focal-fossa)
- [Manage multiple java/javac versions](https://batsov.com/articles/2021/12/10/working-with-multiple-versions-of-java-on-ubuntu/)
- [Install CMake 3.4.1](https://www.linuxfromscratch.org/blfs/view/systemd/general/cmake.html)
- [Install libcurl](https://stackoverflow.com/questions/34914944/could-not-find-curl-missing-curl-library-curl-include-dir-on-cmake)
- [Install libarchive](https://stackoverflow.com/questions/78343136/getting-libarchive-not-found-error-while-trying-to-publish-to-shinyapp)
- [File plugin-api.h not found when building llvm](https://github.com/llvm/llvm-project/issues/64216)
- [Install binutils](https://lindevs.com/install-binutils-on-ubuntu)