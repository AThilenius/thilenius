#!/bin/bash
docker kill thrift_test_dev 2> /dev/null
docker rm thrift_test_dev 2> /dev/null

docker build -t thrift_test:dev .

docker run -d -p 7070:80 --name thrift_test_dev --env-file=/etc/environment thrift_test:dev

CONTAINER_ID=$(docker ps -a | grep thrift_test:dev | cut -f1 -d" ")
sleep 4
docker logs $CONTAINER_ID
