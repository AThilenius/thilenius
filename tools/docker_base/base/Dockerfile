#===============================================================================
#===  THILENIUS_BASE  ==========================================================
#===============================================================================
FROM ubuntu:14.04

#===  UPDATE apt-get  ==========================================================
RUN apt-get update -qq

#===  CURL  ====================================================================
RUN apt-get install -y curl

#===  COPY_SHARED  =============================================================
# For now this is going to be a hardcoded path because I can't get to ~
ADD core /root/thilenius/core
RUN echo '. /root/thilenius/core/shell/base' >> /root/.bashrc
RUN echo 'PATH=$PATH:/root/thilenius/core/tools' >> /root/.bashrc
