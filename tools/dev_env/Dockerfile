#===============================================================================
#===  DEV_ENV  =================================================================
#===============================================================================
FROM athilenius/base

#===  UPDATE apt-get  ==========================================================
RUN apt-get update -q

#===  PYTHON 2.7  ==============================================================
RUN apt-get install -y python python-pip python-dev pylint
RUN pip install yapf

#===  NODEJS / NPM  ============================================================
RUN curl -sL https://deb.nodesource.com/setup_5.x | sudo -E bash -
RUN apt-get install -y nodejs

#===  CLANG  ===================================================================
RUN apt-get install -y build-essential
RUN apt-get install -y libc++-dev libc++abi-dev clang-3.6 lldb-3.6
RUN ln -s /usr/bin/clang-3.6 /usr/bin/clang
RUN ln -s /usr/bin/clang++-3.6 /usr/bin/clang++

#===  GIT  =====================================================================
RUN apt-get install -y git

#===  MAKE / CMAKE  ============================================================
RUN apt-get install -y make
RUN apt-get install -y cmake

#===  MISX TOOLS  ==============================================================
RUN apt-get install -y tmux

#===  FORMATTERS  ==============================================================
RUN npm install -g js-beautify
RUN npm install -g jshint
RUN npm install -g jscs
RUN apt-get install -y clang-format-3.6
RUN ln /usr/bin/clang-format-3.6 /usr/bin/clang-format

#===  VIM PLUGINS  =============================================================
ADD vimrc_plugins /root/env_files/
RUN mv /root/env_files/vimrc_plugins /root/.vimrc
RUN apt-get install -qy vim
RUN git clone https://github.com/gmarik/Vundle.vim.git ~/.vim/bundle/Vundle.vim
RUN vim +PluginInstall +qall

#===  TERN FOR VIM  ============================================================
RUN cd /root/.vim/bundle/tern_for_vim && npm install

#===  YOU COMPLETE ME  =========================================================
RUN export CC=/usr/bin/clang-3.6 && export CXX=/usr/bin/clang++-3.6 && \
    export CXXFLAGS=--std=c++11 && \
    cd /root/.vim/bundle/YouCompleteMe && ./install.sh --clang-completer

#===  WGET  ====================================================================
RUN apt-get install -y wget

#===  CTAGS  ===================================================================
RUN apt-get install -y exuberant-ctags

#===  UNZIP  ===================================================================
RUN apt-get install -y unzip

#===  SCONS  ===================================================================
RUN apt-get install -y scons

#===  MongoDB  =================================================================
RUN apt-key adv --keyserver hkp://keyserver.ubuntu.com:80 --recv 7F0CEB10
RUN echo 'deb http://downloads-distro.mongodb.org/repo/ubuntu-upstart dist 10gen' > /etc/apt/sources.list.d/mongodb.list
# Re-Update from custom list
RUN apt-get update
RUN apt-get install -y mongodb-org
RUN rm -rf /var/lib/apt/lists/*

#===  NPM MODULES  =============================================================
RUN npm install -g mocha

#===  MISX APT-GET  ============================================================
RUN apt-get update -q
RUN apt-get install -y \
  libdouble-conversion-dev \
  libssl-dev \
  make \
  zip \
  git \
  autoconf \
  libtool \
  g++ \
  libboost-all-dev \
  flex \
  bison \
  libgoogle-glog-dev \
  scons \
  libkrb5-dev \
  libsnappy-dev \
  libsasl2-dev \
  libnuma-dev \
  libevent-dev \
  nginx \
  default-jdk \
  git

#===  MONO  ====================================================================
RUN apt-key adv --keyserver hkp://keyserver.ubuntu.com:80 --recv-keys 3FA7E0328081BFF6A14DA29AA6A19B38D3D831EF
RUN echo "deb http://download.mono-project.com/repo/debian wheezy main" | sudo tee /etc/apt/sources.list.d/mono-xamarin.list
RUN apt-get update -y
RUN apt-get install -y mono-devel

#===  SCALA / SBT / JAVA  ======================================================
RUN wget www.scala-lang.org/files/archive/scala-2.11.7.deb
RUN dpkg -i scala-2.11.7.deb

RUN echo "deb https://dl.bintray.com/sbt/debian /" | sudo tee -a /etc/apt/sources.list.d/sbt.list
RUN apt-key adv --keyserver hkp://keyserver.ubuntu.com:80 --recv 642AC823
RUN apt-get update
RUN apt-get install -y sbt

#===  NPM packages =============================================================
RUN npm install -g strongloop bower grunt-cli
# Manually install loopback-skd-lbxm
RUN cd ~; \
    git clone https://github.com/strongloop/loopback-sdk-xamarin.git; \
    cd loopback-sdk-xamarin; \
    npm install;

#===  VIM RC  ==================================================================
ADD vimrc /root/env_files/
RUN cat /root/env_files/vimrc >> /root/.vimrc
RUN mkdir --parent /root/.vim/swapfiles
RUN mkdir --parent /root/.vim/bakfiles

#===  MISX FILES  ==============================================================
ADD bashrc /root/env_files/
RUN mv /root/env_files/bashrc /root/.bashrc

ADD jsbeautifyrc /root/env_files/
RUN mv /root/env_files/jsbeautifyrc /root/.jsbeautifyrc

ADD clang-format /root/env_files/
RUN mv /root/env_files/clang-format /root/.clang-format

#===  CMD  =====================================================================
CMD /bin/bash
