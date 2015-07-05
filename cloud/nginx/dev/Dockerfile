#===============================================================================
#===  NGINX  ===================================================================
#===============================================================================
FROM athilenius/base


#===  NGINX  ===================================================================
RUN apt-get install -y nginx


#===  CONFD  ===================================================================
RUN \
  cd /usr/local/bin &&\
  curl -L https://github.com/kelseyhightower/confd/releases/download/v0.9.0/confd-0.9.0-linux-amd64 -o confd &&\
  chmod +x confd

# Setup directory, remove nginx defaults
RUN rm /etc/nginx/sites-enabled/default

# Add the template and confd config
ADD ./nginx.toml /etc/confd/conf.d/nginx.toml
ADD ./nginx.tmpl /etc/confd/templates/nginx.tmpl


#===  MISX  ====================================================================
# Startup Scripts
ADD ./confd-watch /usr/local/bin/confd-watch
RUN chmod +x /usr/local/bin/confd-watch

# Define default command.
CMD /bin/bash -c '\
  (\
    $HOME/thilenius/core/tools/register_etcd proxies/http/nginx/1\
        $COREOS_PUBLIC_IPV4 80 &> /dev/null &\
  ) && (\
    /usr/local/bin/confd-watch\
  )'
