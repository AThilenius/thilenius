#===============================================================================
#===  OPEN_TO_LAN  =============================================================
#===============================================================================
FROM ubuntu:14.04

#===  UPDATE apt-get  ==========================================================
RUN apt-get update -y

#===  MongoDB  =================================================================
RUN apt-key adv --keyserver hkp://keyserver.ubuntu.com:80 --recv 7F0CEB10
RUN echo 'deb http://downloads-distro.mongodb.org/repo/ubuntu-upstart dist 10gen' > /etc/apt/sources.list.d/mongodb.list
# Re-Update from custom list
RUN apt-get update
RUN apt-get install -y mongodb-org
RUN rm -rf /var/lib/apt/lists/*
RUN mkdir -p /data/mongodb

#===  NODEJS / NPM  ============================================================
RUN apt-get install -qy nodejs
RUN apt-get install -qy nodejs-legacy
RUN apt-get install -qy npm

#===  NPM packages =============================================================
RUN npm install -g strongloop bower grunt grunt-cli
RUN npm install --save loopback-connector-postgresql

#===  RUN SCRIPT  ==============================================================
ADD run_script /etc/otl/
CMD sudo bash /etc/otl/run_script
