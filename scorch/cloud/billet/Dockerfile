#===============================================================================
#===  BILLET CLIENT  ===========================================================
#===============================================================================
FROM ubuntu:14.04

#===  UPDATE apt-get  ==========================================================
RUN apt-get update -q

#===  PYTHON 2.7  ==============================================================
RUN apt-get install -y python python-pip python-dev

#===  CLANG  ===================================================================
RUN apt-get install -y libc++-dev libc++abi-dev clang-3.6 lldb-3.6
RUN ln -s /usr/bin/clang-3.6 /usr/bin/clang
RUN ln -s /usr/bin/clang++-3.6 /usr/bin/clang++
RUN apt-get install -y clang-format-3.6
RUN ln /usr/bin/clang-format-3.6 /usr/bin/clang-format

#===  GIT  =====================================================================
RUN apt-get install -y git

#===  MAKE / CMAKE  ============================================================
RUN apt-get install -y make
RUN apt-get install -y cmake

#===  WGET  ====================================================================
RUN apt-get install -y wget

#===  UNZIP  ===================================================================
RUN apt-get install -y unzip
