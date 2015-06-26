#!/bin/bash
source ${HOME}/thilenius/core/shell/*

INFO 'Copying core to local'
cp -a ${HOME}/thilenius/core ~/thilenius/tools/docker_base/core ||\
        EROR 'Failed to copy thilenius core'

INFO 'Building Docker image'
docker build -t athilenius/base ~/thilenius/tools/docker_base ||\
        EROR 'Failed to build Docker Image'

INFO 'Cleaning up shared_node_modules link'
rm -rf ~/thilenius/tools/docker_base/core

INFO 'athilenius/docker_base built successfully.'
