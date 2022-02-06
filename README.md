# mycc

My awesome the c programming language compiler implementation with C++ 17

## Dependency

Currently, most project dependencies are managed by [VCPKG](https://github.com/microsoft/vcpkg), a cross-platform
package manager for C++. VCPKG will be downloaded and configured automatically after CMake scripts are configured. The
documentation is required of the LATEX compiler.

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

- **Glog**: for logging(FileCheck Tools only)
- **cli11**: for command line parsing(mycc only)
- **Gtest**: for unit testing (this will be only used in test target) .
- **Gflags**: for parsing command line flags.

## Build

The project is build by cmake and required to use out-of-source build. Both Ninja and Makefile are supported and tested.

### Available build targets

- **mycc**: the compiler target
- **mycc_test**: testing target for **mycc** target.
- **mycc_e2e_test**: end to end testing target for **mycc** target.
- **documentation**: documentation target which will compile the documentation from latex to PDF

### Available build options

- **-DENABLE_ASAN**: enable address sanitizer, default is disabled
- **-DENABLE_TSAN**: enable thread sanitizer, default is disabled
- **-DENABLE_MSAN**: enable memory sanitizer, default is disabled
- **-DENABLE_UBSAN**: enable undefined behavior sanitizer, default is disabled
- **-DENABLE_TESTING**: enable unit testing, default is disabled
- **-DENABLE_COVERAGE**: enable code coverage, default is disabled
- **-DENABLE_DOCUMENTATION**: enable documentation, default is enabled
- **-DENABLE_E2E_TESTING**: enable end to end testing, default is disabled

### Special Notes for building in Pyrite

**Pyrite's user home folder's IO is really slow**. I suggest you can create a folder under `/tmp` which have 700
permission and use it as the build root. This will significantly improve the build speed (almost 10x).

### Release build

In release build, we will use `-O3` optimization level. In order to compile, you need to run following command at the 
**root folder of git repository**:

```bash
cmake . -B./build/ && cd ./build && make mycc documentation -j$(nproc)
```

The build will be done in `./build` directory. The document will be generated to `./build/Documentation/developers.pdf`.
And the executable will be generated to `./build/Source/mycc`.

### Debug Build

In debug mode, the compiler will be built with debug symbols and assertions enabled with '-g -O0' and ASAN enabled. In
order to compile, you need to run following command at the **root folder of git repository**:

```bash
 cmake . -B./build/ -DCMAKE_BUILD_TYPE=Debug && cd ./build && make mycc documentation -j$(nproc)
 ```

The build will be done in `./build` directory. The document will be generated to `./build/Documentation/developers.pdf`.
And the executable will be generated to `./build/bin/` directory.

## Testing

There will be two part of testing which split into two different targets:

- **mycc_test**: unit testing
- **mycc_e2e_test**: end to end testing

Both of executable target `mycc_test` and `mycc_e2e_test` will be located in `./build/bin/` directory.

### Unit Testing

Unit testing is done by `gtest` with following command in git root directory:

```bash
 cmake . -B./build/ -DCMAKE_BUILD_TYPE=Debug && cd ./build && make mycc_test -j$(nproc) && ./Source/mycc_test
 ```

### Code Coverage

Code coverage is supported both llvm-cov and gcov, currently code coverage is only supported for `mycc_test` target. For
building with llvm, you need to install `llvm-cov`, for building with gcc, you need to install `gcovr`

For summary report to stdout, Run:

```bash
 cmake . -B./build/ -DCMAKE_BUILD_TYPE=Debug -DENABLE_TESTING -DENABLE_COVERAGE && cd ./build && make ccov-report-mycc_test -j$(nproc)
 ```

For exporting report to cobertura xml format, Run:

```bash
 cmake . -B./build/ -DCMAKE_BUILD_TYPE=Debug -DENABLE_TESTING -DENABLE_COVERAGE && cd ./build && make ccov-export-mycc_test -j$(nproc)
 ```

### End to End Testing

End-to-end testing is done by `llvm-lit`, LLVM testing tool and [FileCheck tools](utils/FileCHeck/README.md). The test
compares the output of the compiler with the expected output rules. The rules files support string matching and regex
matching.

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

- **-c**: specify input file name.
- **-h,--help**: print usage information and exit.
- **-v,--version**: print version information and exit.
- **--fno_color**: disable color output.
- **--fnice_error**: print error message in nice format.
- **-o,--output**: specify output file name.
- **-I,--include**: specify include path.(not support yet)
