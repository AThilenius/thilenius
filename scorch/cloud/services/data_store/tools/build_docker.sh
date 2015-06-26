#!/bin/bash
set -e
pushd ${PWD/\/Scorch\/*//Scorch}/Auth/tools &> /dev/null
source ${PWD/\/Scorch\/*//Scorch}/tools/shell/*

INFO "Building docker to athilenius/auth"
docker build -t athilenius/auth .
popd &> /dev/null
