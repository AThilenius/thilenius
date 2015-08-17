#===============================================================================
#===  MongoDB  =================================================================
#===============================================================================
FROM athilenius/base

#===  MongoDB  =================================================================
RUN apt-key adv --keyserver hkp://keyserver.ubuntu.com:80 --recv 7F0CEB10
RUN echo 'deb http://downloads-distro.mongodb.org/repo/ubuntu-upstart dist 10gen' > /etc/apt/sources.list.d/mongodb.list
# Re-Update from custom list
RUN apt-get update
RUN apt-get install -y mongodb-org
RUN rm -rf /var/lib/apt/lists/*

#===  MISX  ====================================================================
CMD /bin/bash -c 'mkdir -p /data/mongodb && mongod --dbpath=/data/mongodb'
