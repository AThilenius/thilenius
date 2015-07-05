#===============================================================================
#===  AUTH  ====================================================================
#===============================================================================
FROM athilenius/base

#===  PYTHON / NODEJS  =========================================================
RUN apt-get install -y python python-pip python-dev
RUN /bin/bash -c -l 'apt-get install -qy nodejs'
RUN /bin/bash -c -l 'apt-get install -qy npm'
RUN /bin/bash -c -l 'npm update -g express'
RUN update-alternatives --install /usr/bin/node node /usr/bin/nodejs 10

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

#===  NODE PROJECT SETUP  ======================================================
# Install npm dependencies
ADD package.json /root/auth/
RUN cd /root/auth && npm install

ADD gen_files /root/auth/gen_files
ADD httpServer.js /root/auth/

#===  MISX  ====================================================================
CMD /bin/bash -c '\
  (\
    $HOME/thilenius/core/tools/register_etcd services/http/auth/$SERVICE_ID\
      $COREOS_PUBLIC_IPV4 $DOCKER_PORT &> /dev/null &\
  ) && (\
    cd /root/auth && node httpServer.js\
  )'
