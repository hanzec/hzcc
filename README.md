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

### Special Notes for building in Pyrite

**Pyrite's user home folder's IO is really slow**. I suggest you can create a folder under `/tmp` which have 700
permission and use it as the build root. This will significantly improve the build speed (almost 10x).

### Release build

In release build, we will use `-O3` optimization level.

```bash
cmake . -B./build/ && cd ./build && make mycc documentation -j$(nproc)
```

The build will be done in `./build` directory. The document will be generated to `./build/Documentation/developers.pdf`.
And the executable will be generated to `./build/Source/mycc`.

### Debug Build

In debug mode, the compiler will be built with debug symbols and assertions enabled with '-g -O0' and ASAN enabled

```bash
 cmake . -B./build/ -DCMAKE_BUILD_TYPE=Debug && cd ./build && make mycc documentation -j$(nproc)
 ```

The build will be done in `./build` directory. The document will be generated to `./build/Documentation/developers.pdf`.
And the executable will be generated to `./build/Source/mycc`.

## Testing

There will be two part of testing which split into two different targets:

- **mycc_test**: unit testing
- **mycc_e2e_test**: end to end testing

`mycc_test` will be possible located in all sub-folders in `Source/` directory. And `mycc_e2e_test` will be located
in `Test/`

### Unit Testing

Unit testing is done by `gtest` with following command in git root directory:

```bash
 cmake . -B./build/ -DCMAKE_BUILD_TYPE=Debug && cd ./build && make mycc_test -j$(nproc) && ./Source/mycc_test
 ```

### End to End Testing

End to end testing is done by `llvm-lit` which is a LLVM testing tool and [FileCheck tools](utils/FileCHeck/README.md).
The test compares the output of the compiler with the expected output rules. The rules files support string matching and
regex matching.

You could install `llvm-lit` via python pip:

```bash
pip install lit
```

Then you could run the test by following command in git root directory:

```bash
 cmake . -B./build/ -DCMAKE_BUILD_TYPE=Debug && cd ./build && make mycc_e2e_test -j$(nproc)
 ```

## Usage

### VLOG Level

- **0**: only internal warning and error will be printed
- **1**: used for print current status
- **2**: print debug information
- **3**: print all information including pass output/input

### supported arguments

- **-help**: print usage information and exit.
- **-version**: print version information and exit.
