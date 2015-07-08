#===============================================================================
#===  USER DATA  ===============================================================
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
ADD package.json /root/user_data/
RUN cd /root/user_data && npm install

ADD gen_files /root/user_data/gen_files
ADD app.js /root/user_data/

#===  MISX  ====================================================================
CMD /bin/bash -c '\
  DB_IP=$($HOME/thilenius/core/tools/etcd_parser -k /databases/mongodb -f ip);\
  DB_PORT=$($HOME/thilenius/core/tools/etcd_parser -k /databases/mongodb\
      -f port);\
  LB_IP=$($HOME/thilenius/core/tools/etcd_parser -k /proxies/http/nginx -f ip);\
  LB_PORT=$($HOME/thilenius/core/tools/etcd_parser -k /proxies/http/nginx\
      -f port);\
  (\
    $HOME/thilenius/core/tools/register_etcd\
        services/http/user_data/$SERVICE_ID\
        $COREOS_PUBLIC_IPV4 $DOCKER_PORT &> /dev/null &\
  ) && (\
    cd /root/user_data && \
        DB_IP="$DB_IP" \
        DB_PORT="$DB_PORT" \
        LB_IP="$LB_IP" \
        LB_PORT="$LB_PORT" node app.js \
  )'
