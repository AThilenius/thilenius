var Docker = require('dockerode');

module.exports = function(Server) {

  var nextPort = 25566;
  var docker = new Docker();

  // Disable upsert and update
  Server.disableRemoteMethod('upsert', true);
  Server.disableRemoteMethod('updateAll', true);
  Server.disableRemoteMethod('updateAttributes', false);

  Server.observe('before save', function filterProperties(ctx, next) {
    console.log('Before Save Context: ' + JSON.stringify(ctx, null, 2));
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
            Volumes: {'/root/minecraft/world' : {}},
            HostConfig: {
              Binds: [instance.resource_path + ':/root/minecraft/world'],
              PortBindings: {'25565/tcp': [{'HostPort': port.toString()}]}
            }
          },
          function(err, container) {
            ctx.instance.container_uuid = container.id;
            console.log('Starting contaner: ' +
                        JSON.stringify(container, null, 2));
            container.start(function(startErr, startData) {
              console.log('Container err: ' +
                          JSON.stringify(startErr, null, 2));
              console.log('Container data: ' +
                          JSON.stringify(startData, null, 2));
            });
            return next();
          });
    });
  });

  Server.observe('before delete', function filterProperties(ctx, next) {
    console.log('Before Remove for server model. Kill Docker container here.');
    console.log('Ctx.data: ' + JSON.stringify(ctx, null, 2));
    var container = docker.getContainer(ctx.instance.container_uuid);
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
};
