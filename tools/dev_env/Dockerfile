#===============================================================================
#===  DEV_ENV  =================================================================
#===============================================================================
FROM athilenius/base

#===  PYTHON 2.7  ==============================================================
RUN apt-get install -y python python-pip python-dev

#===  NODEJS / NPM  ============================================================
RUN apt-get install -qy nodejs
RUN apt-get install -qy nodejs-legacy
RUN apt-get install -qy npm

#===  CLANG  ===================================================================
RUN apt-get install -y clang

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
ADD vimrc_plugins /root/vim_files/
ADD vim_setup.sh /root/vim_files/
RUN mv /root/vim_files/vimrc_plugins /root/.vimrc
RUN /bin/bash /root/vim_files/vim_setup.sh

#===  TERN FOR VIM  ============================================================
RUN cd /root/.vim/bundle/tern_for_vim && npm install

#===  YOU COMPLETE ME  =========================================================
RUN cd /root/.vim/bundle/YouCompleteMe && ./install.sh --clang-completer

#===  CTAGS  ===================================================================
RUN apt-get install -y exuberant-ctags

#===  DOCKER  ==================================================================
RUN apt-get install -y apt-transport-https ca-certificates lxc iptables
RUN curl -sSL https://get.docker.com/ | sh
ADD wrapdocker /usr/local/bin/
RUN chmod +x /usr/local/bin/wrapdocker

#===  THRIFT  ==================================================================
RUN apt-get install -qy libboost-dev
RUN apt-get install -qy libboost-test-dev
RUN apt-get install -qy libboost-program-options-dev
RUN apt-get install -qy libboost-system-dev
RUN apt-get install -qy libboost-filesystem-dev
RUN apt-get install -qy libevent-dev
RUN apt-get install -qy automake
RUN apt-get install -qy libtool
RUN apt-get install -qy flex
RUN apt-get install -qy bison
RUN apt-get install -qy pkg-config
RUN apt-get install -qy g++
RUN apt-get install -qy libssl-dev
RUN cd /tmp &&\
  curl http://archive.apache.org/dist/thrift/0.9.2/thrift-0.9.2.tar.gz | tar zx\
  &&\
  cd thrift-0.9.2/ &&\
  ./configure &&\
  make &&\
  make install

#===  VIM RC  ==================================================================
ADD vimrc /root/vim_files/
RUN cat /root/vim_files/vimrc >> /root/.vimrc

#===  MISX FILES  ==============================================================
ADD bashrc /root/misx_files/
RUN mv /root/misx_files/bashrc /root/.bashrc

#===  CMD  =====================================================================
VOLUME /var/lib/docker
CMD ["wrapdocker"]
