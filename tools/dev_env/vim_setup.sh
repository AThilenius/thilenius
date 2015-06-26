#!/bin/bash
source ${HOME}/thilenius/core/shell/*

INFO 'Installing VIM'
apt-get install -y vim || EROR 'Failed to install VIM'

INFO 'Adding Plugins-only .vimrc'
cp /root/env_files/vimrc_plugins $HOME/.vimrc

INFO 'Installing Vundle'
git clone https://github.com/gmarik/Vundle.vim.git ~/.vim/bundle/Vundle.vim ||\
        EROR 'Failed to GIT clone Vundle'

INFO 'Installing Vundle Plugins [PluginInstall]'
vim +PluginInstall +qall || EROR 'Failed to install Vundle Plugins'

INFO 'VIM setup compleated successfully.'
