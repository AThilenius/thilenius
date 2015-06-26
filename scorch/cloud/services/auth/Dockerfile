#===============================================================================
#===  THRIFT_TEST  =============================================================
#===============================================================================
FROM ubuntu:14.04


#===  CURL / PYTHON / NODEJS  ==================================================
RUN apt-get update -qq
RUN apt-get install -y curl
RUN apt-get install -y python python-pip python-dev
RUN /bin/bash -c -l 'apt-get install -qy nodejs'
RUN /bin/bash -c -l 'apt-get install -qy npm'
RUN /bin/bash -c -l 'npm update -g express'
RUN update-alternatives --install /usr/bin/node node /usr/bin/nodejs 10


#===  NODE PROJECT SETUP  ======================================================
ADD node_modules /home/core/node/node_modules
ADD gen-nodejs /home/core/node/gen-nodejs
ADD httpServer.js /home/core/node/


#===  ETCD  ====================================================================
ADD register_etcd.sh /home/core/node/
RUN chmod +x /home/core/node/register_etcd.sh


#===  MISX  ====================================================================
WORKDIR /home/core/node
CMD /bin/bash -c '\
  (\
    /home/core/node/register_etcd.sh services/http/thrift/1 $COREOS_PUBLIC_IPV4\
      7070 &> /dev/null &\
  ) && (\
    node httpServer.js\
  )'
