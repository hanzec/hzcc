FROM ubuntu:latest

ARG DEBIAN_FRONTEND=noninteractive
ENV TZ=Etc/UTC

###############################################################################
## LLVM                                                                      ##
###############################################################################
# Install Dependencies
RUN apt-get -qq update
RUN apt-get -qq install -y clang llvm

## Install lcov_cobertura for coverage
RUN apt-get -qq update
RUN apt-get -qq install -y python3 python3-pip
RUN pip3 install lcov_cobertura

## Install llvm-lit for testing
RUN apt-get -qq update
RUN apt-get -qq install -y python3 python3-pip
RUN pip3 install lit

###############################################################################
## CMake 3.18.0                                                              ##
###############################################################################
RUN apt-get -qq update
RUN apt-get -qq install -y git wget
RUN mkdir -p /tmp && cd /tmp
RUN wget -q "https://github.com/Kitware/CMake/releases/download/v3.20.0/cmake-3.20.0-linux-x86_64.sh" -P /tmp
RUN chmod +x /tmp/cmake-3.20.0-linux-x86_64.sh
RUN /tmp/cmake-3.20.0-linux-x86_64.sh --skip-license --prefix=/usr/local

###############################################################################
## Ninja Build                                                               ##
###############################################################################
RUN apt-get -qq update
RUN apt-get -qq install -y ninja-build

###############################################################################
## VCPKG Deps                                                                ##
###############################################################################
RUN apt-get -qq update
RUN apt-get -qq install -y curl zip unzip tar pkg-config cmake # install vcpkg dependencies


###############################################################################
## Documentation                                                             ##
###############################################################################
## Install LaTeX
RUN apt-get -qq update
RUN apt-get -qq install -y texlive texlive-latex-extra

###############################################################################
## JAVA Deps                                                                ##
###############################################################################
RUN apt-get -qq update
RUN apt-get -qq install -y default-jdk # install vcpkg dependencies
