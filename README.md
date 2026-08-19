# Mozart-Oz v2

[![Join the chat at https://gitter.im/mozart/mozart2](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/mozart/mozart2?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

The Mozart Programming System is an open source implementation of Oz 3.
This repository contains the upcoming version 2 of the system.

The status of Mozart 2 is currently _alpha quality_. It is not ready for
production, but it can be used for experimenting, testing, and obviously, for
contributing.

<!-- ============================================================= -->

# Downloads

Binary packages for recent releases are available on [GitHub](https://github.com/mozart/mozart2/releases).

[SourceForge](http://sourceforge.net/projects/mozart-oz/files/) contains all the binary packages ever published, including previous releases and releases of Mozart 1.

<!-- ============================================================= -->

# Build Instructions (Linux only)

This main Readme is shamefully biased towards Linux. Side-along Readmes are
available for [Mac OS](README.MacOS.md), [Windows](README.Windows.md), and [OpenBSD](README.OpenBSD.md).

We found rather difficult to build and configure correctly CLANG/LLVM for all
supported system. Those tools are required to build pre-generated sources. In
reponse to this, we decided to include in this repository those sources. This
section describes a build with the pre-generated sources included. We do
however check the pre-generated sources at every commit, rebuilding them in a
Travis CI job. The files [.travis.yml(for Linux)](.travis.yml) and [appveyor(for
Windows)](appveyor.yml) can be very helpfull guides to build Mozart2 on your system. You can
find information about our [CI jobs](README.CI.md).

## Issues

In any case of unexpected issue during the whole build process take a look at [Linux issues](README.Linux.issues.md) if a solution is not already there. If not please report your found solution it might help others in the future or contact contributors.

<!-- ============================================================= -->

## Prerequisites

In order to build Mozart 2, you need the following tools on your computer:

*   git and Subversion to grab the source code
*   java 1.8.0 (openjdk-8)
*   gcc 15
*   cmake >= 4.2.x
*   Boost 1.88.0 (with development files). We recommend the use of Boost
1.65 as there is some issues with recent version of Boost with cmake.
*   Tcl/Tk 8.5 or 8.6 (with development files)
*   emacs

<!-- ============================================================= -->

### Boost

As mentioned, recent versions of Boost are not currently correctly supported by
cmake. However, if you wish to build Mozart2 with a recent version of Boost(>
1.65), you should include the option `-DCMAKE_CXX_COMPILER_ARCHITECTURE_ID=your
architecture id`(x64 for a 64 bits system) in your cmake command. You may also run the cmake command to
generate Makefile twice, as the second time the cache is used to find Boost.
Should cmake fail to find your Boost you can specify the localation with the
option `-DBOOST_ROOT`.

<!-- ============================================================= -->

## Clone the Mozart Repository

As the Mozart repository contains submodules, you should clone recursively:

    git clone --recursive https://github.com/mozart/mozart2

    You can also fetch the submodules separately using:

    git clone https://github.com/mozart/mozart2
    cd mozart2
    git submodule update --init

<!-- ============================================================= -->

## Build Mozart

### Installing build tools

<!-- ============================================================= -->

---

#### OpenJDK 8

The boot-compiler is written with an old Scala version running above the JVM of Java 8. In order to make it run, install openjdk-8 using apt or any other packages manager.

```bash
sudo apt update
sudo apt install -y openjdk-8-jdk # Should migrate to openjdk-21-jdk soon
```

Once it is done, ensure the system has taken the right version if several are co-existing.

```bash
sudo update-alternatives --config java
There are 2 choices for the alternative java (providing /usr/bin/java).

  Selection    Path                                            Priority   Status
------------------------------------------------------------
* 0            /usr/lib/jvm/java-25-openjdk-amd64/bin/java      2511      auto mode
  1            /usr/lib/jvm/java-25-openjdk-amd64/bin/java      2511      manual mode
  2            /usr/lib/jvm/java-8-openjdk-amd64/jre/bin/java   1081      manual mode

Press <enter> to keep the current choice[*], or type selection number: 2
update-alternatives: using /usr/lib/jvm/java-8-openjdk-amd64/jre/bin/java to provide /usr/bin/java (java) in manual mode

java -version
openjdk version "1.8.0_492"
OpenJDK Runtime Environment (build 1.8.0_492-8u492-ga~us2-0ubuntu1~26.04.1-b09)
OpenJDK 64-Bit Server VM (build 25.492-b09, mixed mode)
```

<!-- ============================================================= -->

---

#### Build-essential and gcc/g++ 15

The Mozart programming system has been updated to C++20 so install GNU C compiles version 15 supporting well this standard.

```bash
sudo apt update
sudo apt install -y build-essential gcc-15 g++-15
```

Maybe there are several gcc/g++ versions on the system so alternatives to it can be added as shown below.

```bash
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-14 14
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-14 14
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-15 15
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-15 15
```

Then select the right alternative that is gcc/g++ 15.

```bash
sudo update-alternatives --config gcc
There are 2 choices for the alternative gcc (providing /usr/bin/gcc).

  Selection    Path             Priority   Status
------------------------------------------------------------
* 0            /usr/bin/gcc-15   15        auto mode
  1            /usr/bin/gcc-14   14        manual mode
  2            /usr/bin/gcc-15   15        manual mode

Press <enter> to keep the current choice[*], or type selection number: 0
sudo update-alternatives --config g++
There are 2 choices for the alternative g++ (providing /usr/bin/g++).

  Selection    Path             Priority   Status
------------------------------------------------------------
* 0            /usr/bin/g++-15   15        auto mode
  1            /usr/bin/g++-14   14        manual mode
  2            /usr/bin/g++-15   15        manual mode

Press <enter> to keep the current choice[*], or type selection number: 0
```

Finally check the right version is taken by the system.

```bash
gcc -v
gcc (Ubuntu 15-20250404-0ubuntu1) 15.0.1 20250404 (experimental) [master r15-9193-g08e803aa9be]
Copyright (C) 2025 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
g++ -v
g++ (Ubuntu 15-20250404-0ubuntu1) 15.0.1 20250404 (experimental) [master r15-9193-g08e803aa9be]
Copyright (C) 2025 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
```

<!-- ============================================================= -->

---

#### CMake >= 4.2.x

First go to the right place to do that and download the appropriate version from [CMake releases](https://cmake.org/download/) according to your linux distribution. After extract the archive and go inside.

```bash
cd /tmp/
wget https://github.com/Kitware/CMake/releases/download/v4.4.2/cmake-4.4.2.tar.gz
tar -xvf cmake-4.4.2.tar.gz 
cd cmake-4.4.2/
```

Follow the instructions displayed by `cat README*` to compile CMake.

CMake build chain relies on make so it needs to be installed.

```bash
sudo apt update
sudo apt install -y make
```

Finally build CMake and install the project.

```bash
./bootstrap # Or with -- -DCMAKE_USE_OPENSSL=OFF to disable openssl
make
sudo make install
```

Note if you see the following error `
CMake Error at Utilities/cmcurl/CMakeLists.txt:1014 (message):
  Could not find OpenSSL. Install an OpenSSL development package or
  configure CMake with -DCMAKE_USE_OPENSSL=OFF to build without OpenSSL.` just run the first command as `./bootstrap -- -DCMAKE_USE_OPENSSL=OFF`.

Once it is installed check the proper version has been chosed by the system.

```bash
cmake --version
cmake version 4.4.2

CMake suite maintained and supported by Kitware (kitware.com/cmake).
```

<!-- ============================================================= -->

---

#### Boost 1.88

After this boost version, the sub-library libboost-system becomes header only (see [here](https://github.com/gnss-sdr/gnss-sdr/pull/990)) thus it makes the current CMakeLists files fail. It should be updated soon but for now we do not cross this version for newer ones.

Anyway from the [Boost releases](https://www.boost.org/releases/1.88.0/) download the right archive and extract it.

```bash
cd /tmp/
wget https://archives.boost.io/release/1.88.0/source/boost_1_88_0.tar.gz
tar -xvf boost_1_88_0.tar.gz
cd boost_1_88_0/
```

From the tutorial [here](https://www.boost.org/doc/user-guide/getting-started.html) compile the boost project using the recommended b2 method.

```bash
./bootstrap.sh --prefix=/usr/local/
./b2
sudo ./b2 install
```

Finally check the installation : if there are libboost files with the right version it means everything went fine.

```bash
ls /usr/local/lib/ | grep libboost
```

You may need to add the following environment variable at the end of the file `~/.bashrc` in order to help the system to find easily boost header files.

```bash
export BOOST_ROOT=/usr/local
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
export CPLUS_INCLUDE_PATH=/usr/local/include:$CPLUS_INCLUDE_PATH
```

Then reload it.

```bash
source ~/.bashrc
```

<!-- ============================================================= -->

---

#### CLang/LLVM 22.1.0

From the [LLVM 22.1.0 releases](https://github.com/llvm/llvm-project/releases/tag/llvmorg-22.1.0) download the archive containing the source code and extract it.

```bash
cd /tmp/
wget https://github.com/llvm/llvm-project/archive/refs/tags/llvmorg-22.1.0.tar.gz
tar -xvf llvmorg-22.1.0.tar.gz
cd llvm-project-llvmorg-22.1.0/
```

Install the ninja build tool that is used by LLVM to be built.

```bash
sudo apt update
sudo apt install -y ninja-build
sudo apt install -y binutils binutils-dev
```

Then build the project but before doing it take a loog at [`/tmp/` running out of memory](./README.Linux.issues.md#missing-memory-space-in-tmp) because it is a heavy build taking more than 3.5GB.

```bash
mkdir -v llvm/build/
cd llvm/build/
CC=gcc CXX=g++                               \
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
ninja -j$(nproc)
sudo ninja install
```

Check if everything is installed properly by listing the installed Clang and LLVM.

```bash
ls /usr/include/clang
ls /usr/include/llvm
```

---

#### TK, TCL and EMACS

Install the remaining tools.
```bash
sudo apt update
sudo apt install -y tcl-dev tk-dev
sudo apt install -y emacs
```

<!-- ============================================================= -->

### Compile Mozart2 with pre-generated files

Once all tools are well deployed it is finally possible to fully compile the Mozart programming system. First of all we need to parse the cmake files and specifying some important options as the paths to clang and llvm.

```bash
cmake -S . -B build/ -DMOZART_CACHED_BUILD=OFF -DClang_DIR=/usr/lib/cmake/clang/ -DLLVM_DIR=/usr/lib/cmake/llvm/ -DCMAKE_BUILD_TYPE=Release
```

If clang complains about an uncertainty of which gcc version to use try the following command. However adapt gcc instal directory to the right version path.

```bash
cmake -S . -B build/ \
  -DMOZART_CACHED_BUILD=OFF \
  -DClang_DIR=/usr/lib/cmake/clang/ \
  -DLLVM_DIR=/usr/lib/cmake/llvm/ \
  -DCMAKE_C_COMPILER=clang \
  -DCMAKE_CXX_COMPILER=clang++ \
  -DCMAKE_C_FLAGS="--gcc-install-dir=/usr/lib/gcc/x86_64-linux-gnu/15" \
  -DCMAKE_CXX_FLAGS="--gcc-install-dir=/usr/lib/gcc/x86_64-linux-gnu/15" \
  -DCMAKE_BUILD_TYPE=Release
```

Finally we compile the mozart virtual machine and generate all pre-generated C++ files. The Mozart virtual machine uses a **heavy meta-templating programming** where the code written by human tends to be as much as possible abstract and concrete implementations as specific operations applicated to each particular Mozart object are generated by the VM generator.

When the virtual machine is compiled and pre-generated files are created, the compiler is compiled at his turn. First the Scala boot-compiler compiles the OZ compiler then this new compiler using the C++ VM will recompile itself.

At the end of the process, just install the compiled Mozart system into your Linux OS.

```bash
cd build/
make -B gensources genboostsources VERBOSE=1 -j$(nproc)
make VERBOSE=1 -j$(nproc)
sude make install
```

Or alternatively thanks to new cmake version you can do the following commands.

```bash
cmake --build build/ --target gensources genboostsources --parallel $(nproc) --verbose
cmake --build build/ --parallel $(nproc) --verbose
sudo cmake --install build/
```

<!-- ============================================================= -->

---

#### CMake Options

Other cmake options can be given with the form `-DOPTION=Value`. The table below
lists the options you can add.

<table>
  <thead>
    <tr><th>Option</th><th>Value</th><th>Required if</th>
  </thead>
  <tbody>
    <tr>
      <td>CMAKE_BUILD_TYPE</td>
      <td>Debug or Release</td>
      <td>Always</td>
    </tr>
    <tr>
      <td>CMAKE_INSTALL_PREFIX</td>
      <td>Where `make install` should install</td>
      <td>-</td>
    </tr>
    <tr>
      <td>CMAKE_CXX_COMPILER</td>
      <td>Path to your C++ compiler</td>
      <td>Mac OS: must be forced to clang++</td>
    </tr>
    <tr>
      <td>CMAKE_MAKE_PROGRAM</td>
      <td>Path to your make program</td>
      <td>Windows: must be forced to MinGW make</td>
    </tr>
    <tr>
      <td>GTEST_SRC_DIR and GTEST_BUILD_DIR</td>
      <td>Paths to the source and build directories of GTest</td>
      <td>If not present, GTest will be downloaded and built automatically</td>
    </tr>
    <tr>
      <td>LLVM_SRC_DIR and LLVM_BUILD_DIR</td>
      <td>Paths to the source and build directories of LLVM</td>
      <td>If not present, LLVM will be downloaded and built automatically</td>
    </tr>
    <tr>
      <td>CLANG_SRC_DIR</td>
      <td>Paths to the source directory of CLANG</td>
      <td>Use this if cmake cannot find the CLANG sources</td>
    </tr>
    <tr>
      <td>EMACS</td>
      <td>Path to the Emacs executable</td>
      <td>Required on Windows (on Unix it can be found automatically, in principle)</td>
    </tr>
    <tr>
      <td>CPACK_GENERATOR</td>
      <td>Comma-separated list of generators for CPack</td>
      <td>Optional, see CPack documentation</td>
    </tr>
    <tr>
      <td>BOOST_ROOT</td>
      <td>Path to the boost root</td>
      <td>May be used if cmake fails to find boost</td>
    </tr>
    <tr>
      <td>CMAKE_CXX_COMPILER_ARCHITECTURE_ID</td>
      <td>Indicates in which architecture the system is compiled</td>
      <td>Required with recent version of Boost (due to some incompability with cmake)</td>
    </tr>
  </tbody>
</table>

There is a NixOS expression to install the Mozart2 binary:
`nix-env -i mozart-binary`

<!-- ============================================================= -->

### Setup vscode

Paste the below configuration json from the root of mozart2 repository into `.vscode/settings.json`.

```json
{
    "C_Cpp.default.compilerPath": "/usr/bin/g++-15",
    "C_Cpp.default.cppStandard": "c++20",
    "C_Cpp.default.includePath": [
        "${workspaceFolder}/**",
        "/usr/local/include",
        "/usr/include"
    ]
}
```

However you may adapt to your system if you use a different compiler, C++ standard (not recommended) or having installed the tools somewhere else.

<!-- ============================================================= -->

### Sources

- [Install OpenJDK-8](https://openjdk.org/install/)
- [Manage multiple java/javac versions](https://batsov.com/articles/2021/12/10/working-with-multiple-versions-of-java-on-ubuntu/)
- [Manage multiple gcc/g++ versions](https://linuxconfig.org/how-to-switch-between-multiple-gcc-and-g-compiler-versions-on-ubuntu-20-04-lts-focal-fossa)
- [CMake releases](https://cmake.org/download/)
- [Boost releases](https://www.boost.org/releases/1.88.0/)
- [Building boost](https://www.boost.org/doc/user-guide/getting-started.html)
- [CLang/LLVM 22.1.0 releases](https://github.com/llvm/llvm-project/releases/tag/llvmorg-22.1.0)
- [CLang/LLVM 22.1.x build instructions](https://www.linuxfromscratch.org/blfs/view/systemd/general/llvm.html)
