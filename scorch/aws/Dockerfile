#===============================================================================
#===  SCORCH_FORGE  ============================================================
#===============================================================================
FROM ubuntu:14.04

#===  UPDATE apt-get  ==========================================================
RUN apt-get update -q

#===  CLANG  ===================================================================
RUN apt-get install -y libc++-dev libc++abi-dev clang-3.6 lldb-3.6
RUN ln -s /usr/bin/clang-3.6 /usr/bin/clang
RUN ln -s /usr/bin/clang++-3.6 /usr/bin/clang++
RUN apt-get install -y clang-format-3.6

#===  MAKE / CMAKE  ============================================================
RUN apt-get install -y make
RUN apt-get install -y cmake

#===  NGINX  ===================================================================
RUN apt-get install -y nginx

#===  CONFIGURE NGINX  =========================================================
ADD default /etc/nginx/sites-available/default
ADD certs/ /etc/ssl/
ADD www/data/ /www/data/
RUN chown -R www-data /www

#===  MongoDB  =================================================================
RUN apt-key adv --keyserver hkp://keyserver.ubuntu.com:80 --recv 7F0CEB10
RUN echo 'deb http://downloads-distro.mongodb.org/repo/ubuntu-upstart dist 10gen' > /etc/apt/sources.list.d/mongodb.list
# Re-Update from custom list
RUN apt-get update
RUN apt-get install -y mongodb-org
RUN rm -rf /var/lib/apt/lists/*
RUN mkdir -p /data/mongodb

#===  SERVER BINARIES  =========================================================
ADD packages/ /etc/scorch/packages/

#===  RUN SCRIPT  ==============================================================
ADD run_script /etc/scorch/
CMD sudo bash /etc/scorch/run_script
