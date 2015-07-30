#===============================================================================
#===  USER DATA  ===============================================================
#===============================================================================
FROM athilenius/npm_base

#===  NODE PROJECT SETUP  ======================================================
# Install npm dependencies
ADD package.json /root/user_data/
RUN cd /root/user_data && npm install

# Modules
ADD modules /root/user_data/modules

# Project Files
ADD gen_files /root/user_data/gen_files
ADD app.js /root/user_data/
ADD service_racks.js /root/user_data/

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
    while true; do\
        DB_IP="$DB_IP" \
        DB_PORT="$DB_PORT" \
        LB_IP="$LB_IP" \
        LB_PORT="$LB_PORT" \
        NODE_PATH=/root/user_data node app.js; \
   done\
  )'
