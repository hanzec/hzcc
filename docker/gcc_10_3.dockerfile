FROM gcc:10.3

ARG DEBIAN_FRONTEND=noninteractive
ENV TZ=Etc/UTC

###############################################################################
## LLVM Lit                                                                  ##
###############################################################################
RUN apt-get -qq update
RUN apt-get -qq install -y python3 python3-pip build-essential
RUN pip3 install lit

###############################################################################
## GCC                                                                       ##
###############################################################################
RUN apt-get -qq update
RUN apt-get -qq install -y python3 python3-pip build-essential
RUN pip3 install gcovr

###############################################################################
## CMake 3.16.3                                                              ##
###############################################################################
RUN apt-get -qq update
RUN apt-get -qq install -y git wget
RUN mkdir -p /tmp && cd /tmp
RUN wget -q "https://github.com/Kitware/CMake/releases/download/v3.16.3/cmake-3.16.3-linux-x86_64.sh" -P /tmp
RUN chmod +x /tmp/cmake-3.16.3-linux-x86_64.sh
RUN /tmp/cmake-3.16.3-linux-x86_64.sh --skip-license --prefix=/usr/local

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
RUN apt-get -qq install -y texlive-base texlive-latex-recommended texlive-latex-extra

## Install Doxygen
RUN apt-get -qq update
RUN apt-get -qq install -y graphviz doxygen-latex doxygen xapian-tools