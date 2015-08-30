#===============================================================================
#===  DEV_ENV LITE  ============================================================
#===============================================================================
FROM athilenius/base

#===  UPDATE apt-get  ==========================================================
RUN apt-get update -q

#===  PYTHON 2.7  ==============================================================
RUN apt-get install -y python python-pip python-dev

#===  CLANG 3.6  ===============================================================
RUN apt-get install -y libc++-dev libc++abi-dev clang-3.6 lldb-3.6
RUN ln -s /usr/bin/clang-3.6 /usr/bin/clang
RUN ln -s /usr/bin/clang++-3.6 /usr/bin/clang++

#===  GIT  =====================================================================
RUN apt-get install -y git

#===  MAKE / CMAKE  ============================================================
RUN apt-get install -y make
RUN apt-get install -y cmake

#===  FORMATTERS  ==============================================================
RUN apt-get install -y clang-format-3.6
RUN ln /usr/bin/clang-format-3.6 /usr/bin/clang-format

#===  VIM PLUGINS  =============================================================
ADD vimrc_plugins /root/env_files/
RUN mv /root/env_files/vimrc_plugins /root/.vimrc
RUN apt-get install -qy vim
RUN git clone https://github.com/gmarik/Vundle.vim.git ~/.vim/bundle/Vundle.vim
RUN vim +PluginInstall +qall

#===  YOU COMPLETE ME  =========================================================
RUN cd /root/.vim/bundle/YouCompleteMe && ./install.sh --clang-completer

#===  CTAGS  ===================================================================
RUN apt-get install -y exuberant-ctags

#===  WGET  ====================================================================
RUN apt-get install -y wget

#===  UNZIP  ===================================================================
RUN apt-get install -y unzip

#===  VIM RC  ==================================================================
ADD vimrc /root/env_files/
RUN cat /root/env_files/vimrc >> /root/.vimrc

#===  MISX FILES  ==============================================================
ADD bashrc /root/env_files/
RUN mv /root/env_files/bashrc /root/.bashrc

ADD clang-format /root/env_files/
RUN mv /root/env_files/clang-format /root/.clang-format

ADD ycm_extra_conf.py /root/env_files/
RUN mv /root/env_files/ycm_extra_conf.py /root/.ycm_extra_conf.py

#===  CMD  =====================================================================
CMD /bin/bash
