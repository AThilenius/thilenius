#!/bin/bash
source ${HOME}/thilenius/core/shell/*

docker build -t athilenius/dev_env ~/thilenius/tools/dev_env ||\
        EROR 'Failed to build Docker Image'

INFO 'athilenius/dev_end built successfully.'
