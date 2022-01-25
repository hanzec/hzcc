# mycc

My awesome compiler implementation with C++ 17

## Dependency

Currently, most of the project dependencies are managed by [VCPKG](https://github.com/microsoft/vcpkg) which is a
cross-platform package manager for C++. VCPKG will be downloaded and configure automatically after cmake scripts is
configured. The documentation required of LATEX compiler.

### Install VCPKG Dependencies(Build in pyrite Machine could skip this step)

On Debian and Ubuntu derivatives:

```bash
sudo apt-get install curl zip unzip tar pkg-config cmake
```

On recent Red Hat and Fedora derivatives:

```bash
sudo dnf install curl zip unzip tar pkg-config cmake
```

On older Red Hat and Fedora derivatives:

```bash
sudo yum install curl zip unzip tar pkg-config cmake
```

On SUSE Linux and derivatives:

```bash
sudo zypper install curl zip unzip tar pkg-config cmake
```

On Alpine:

```bash
apk add build-base cmake ninja zip unzip curl git pkg-config cmake
```

### Install VCPKG

**No Need, will be installed automatically**

### Install LATEX (Build in pyrite Machine could skip this step)

On Debian and Ubuntu derivatives:

```bash
sudo apt-get install -y texlive-base texlive-latex-recommended texlive-latex-extra
```

### External Dependency Managed by VCPKG (Will automatically compile and linking by CMake)

- **Glog**: for logging.
- **Gtest**: for unit testing (this will be only used in test target) .
- **Gflags**: for parsing command line flags.

## Build

The project is build by cmake and required to use out-of-source build. Both Ninja and Makefile are supported and tested.

### Available build targets

- **mycc**: the compiler target
- **mycc_test**: testing target for **mycc** target.
- **documentation**: documentation target which will compile the documentation from latex to PDF

### Release build

In release build, we will use `-O3` optimization level.

```bash
cmake . -B./build/ && cd ./build && make mycc documentation -j$(nproc)
```

### Debug Build

In debug mode, the compiler will be built with debug symbols and assertions enabled with '-g -O0' and ASAN enabled

```bash
 cmake . -B./build/ -DCMAKE_BUILD_TYPE=Debug && cd ./build && make mycc documentation -j$(nproc)
 ```

## Usage

### supported arguments

- **-help**: print usage information and exit.
- **-version**: print version information and exit.
