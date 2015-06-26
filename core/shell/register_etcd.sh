#!/bin/bash
source ./log.sh
source ./shflags

DEFINE_string 'route' ''\
        'the route to this service. For example /services/html/auth/1' 'r'
DEFINE_string 'ip' "${COREOS_PUBLIC_IPV4}" 'the public ip address of this service' 'i'
DEFINE_integer 'port' -1\
        'the public port of this service (before docker mapping)' 'n'
DEFINE_integer 'delay' 20 'the sleep delay before re-registering the etcd key'
DEFINE_integer 'ttl' 30 'the time-to-live for the etcd key'

# parse the command-line
FLAGS "$@" || exit $?
eval set -- "${FLAGS_ARGV}"

[ "${FLAGS_route}" = '' ] && EROR 'You much specify a route'
[ "${FLAGS_ip}" = '' ] && EROR 'You much specify an ip'
[ ${FLAGS_port} -eq -1 ] && EROR 'You much specify a port'

INFO "Registering ETCD path [ ${FLAGS_route} ] as ip: [ ${FLAGS_ip} ], port: [ ${FLAGS_port} ]"
while true; do
  curl -s -L -X PUT\
    http://$COREOS_PUBLIC_IPV4:4001/v2/keys/${FLAGS_route}?ttl=${FLAGS_ttl}\
    -d value="{\"ip\":\"${FLAGS_ip}\", \"port\":${FLAGS_port}}"
  sleep ${FLAGS_delay}
done
