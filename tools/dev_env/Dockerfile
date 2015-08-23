#===============================================================================
#===  DEV_ENV  =================================================================
#===============================================================================
FROM athilenius/base

#===  UPDATE apt-get  ==========================================================
RUN apt-get update -q

#===  PYTHON 2.7  ==============================================================
RUN apt-get install -y python python-pip python-dev

#===  NODEJS / NPM  ============================================================
RUN apt-get install -qy nodejs
RUN apt-get install -qy nodejs-legacy
RUN apt-get install -qy npm

#===  CLANG  ===================================================================
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
RUN cd /root/.vim/bundle/YouCompleteMe && ./install.sh --clang-completer

#===  CTAGS  ===================================================================
RUN apt-get install -y exuberant-ctags

#===  WGET  ====================================================================
RUN apt-get install -y wget

#===  UNZIP  ===================================================================
RUN apt-get install -y unzip

#===  BOOST + BOOST PROCESS ====================================================
RUN mkdir /root/boost
ADD boost/CMakeLists_.txt /root/boost/
RUN cd /root/boost && \
    mv CMakeLists_.txt CMakeLists.txt && cmake . && \
    make || echo "\033[0;31mBoost failed to build, continuing anyway."
RUN mkdir /root/boost_process && \
    cd /root/boost_process && \
    wget http://www.highscore.de/boost/process.zip \
    unzip process.zip \
    cp -r boost/* /usr/include/boost
# Fix for:
# http://stackoverflow.com/questions/8926715/missing-exception-in-boost-process
RUN sed -i -e \
    's#<boost/filesystem/path.hpp>#<boost/filesystem/operations.hpp>#g' \
    /usr/include/boost/process/operations.hpp
RUN rm -rf /root/boost /root/boost_process

#===  LIB EVENT  ===============================================================
RUN apt-get install -y libevent-dev

#===  SCONS  ===================================================================
RUN apt-get install -y scons

#===  CURL-DEV  ================================================================
RUN apt-get install -y libcurl4-openssl-dev

#===  VIM RC  ==================================================================
ADD vimrc /root/env_files/
RUN cat /root/env_files/vimrc >> /root/.vimrc

#===  NPM MODULES  =============================================================
RUN npm install -g mocha

#===  MISX FILES  ==============================================================
ADD bashrc /root/env_files/
RUN mv /root/env_files/bashrc /root/.bashrc

ADD clang-format /root/env_files/
RUN mv /root/env_files/clang-format /root/.clang-format

#===  CMD  =====================================================================
CMD /bin/bash
