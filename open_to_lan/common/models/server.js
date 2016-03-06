// Copyright 2015 Alec Thilenius
// All rights reserved.

var Docker = require('dockerode');

module.exports = function(Server) {

  var nextPort = 25566;
  var docker = new Docker();

  // Disable upsert and update
  Server.disableRemoteMethod('upsert', true);
  Server.disableRemoteMethod('updateAll', true);
  Server.disableRemoteMethod('updateAttributes', false);

  Server.observe('before save', function filterProperties(ctx, next) {
    Server.app.models.World.findById(ctx.instance.worldId, function(err,
                                                                    instance) {
      if (!instance) {
        console.log('Failed to find World. INSERT ERROR HERE.');
        return;
      }
      var port = nextPort++;
      ctx.instance.state = 'STARTING';
      ctx.instance.ttl = 60 * 60 * 2;
      ctx.instance.created = Date.now();
      ctx.instance.ip = '172.0.0.1';
      ctx.instance.port = port;
      docker.createContainer(
          {
            Image: 'athilenius/minecraft',
            Cmd: ['/bin/bash', '/root/minecraft/launch'],
            ExposedPorts: {'25565/tcp': {}},
            Volumes: {'/root/minecraft/world': {}},
            Env: ['WORLD_GUID=' + instance.world_guid],
            HostConfig: {
              PortBindings: {'25565/tcp': [{'HostPort': port.toString()}]}
            }
          },
          function(err, container) {
            ctx.instance.container_uuid = container.id;
            console.log('Starting contaner ' + container.id);
            container.start(function(startErr, startData) {
              // TODO(athilenius): Handle errors
            });
            return next();
          });
    });
  });

  Server.observe('before delete', function(ctx, next) {
    Server.app.models.Server.findById(ctx.where.id, function(err, instance) {
      if (!instance) {
        console.log('Failed to find Server. INSERT ERROR HERE.');
        return;
      }
      var container = docker.getContainer(instance.container_uuid);
      if (container) {
        container.stop(function(stopErr, stopData) {
          console.log('Container stopped.');
          container.remove(function(removeErr, removeData) {
            console.log('Container removed.');
            return next();
          });
        });
      }
    });
  });
};
