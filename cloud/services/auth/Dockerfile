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

#===  NODE PROJECT SETUP  ======================================================
# Install npm dependencies
ADD package.json /root/auth/
RUN cd /root/auth && npm install

ADD gen_files /root/auth/gen_files
ADD httpServer.js /root/auth/

#===  MISX  ====================================================================
CMD /bin/bash -c '\
  DB_IP=$($HOME/thilenius/core/tools/etcd_parser -k /databases/mongodb -f ip);\
  DB_PORT=$($HOME/thilenius/core/tools/etcd_parser -k /databases/mongodb\
      -f port);\
  (\
    $HOME/thilenius/core/tools/register_etcd services/http/auth/$SERVICE_ID\
        $COREOS_PUBLIC_IPV4 $DOCKER_PORT &> /dev/null &\
  ) && (\
    cd /root/auth && NODE_ENV=production DB_IP="$DB_IP" DB_PORT="$DB_PORT"\
        node httpServer.js\
  )'
